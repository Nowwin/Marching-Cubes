#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "skybox.hpp"
#include "MarchingCubes.hpp"

#include "glm/gtc/matrix_transform.hpp"

// Include the 'Renderer.hpp' which deteremines what
// the graphics API is going to be for OpenGL

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

void printMatrix(const glm::mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

Camera g_camera;

int numberOfMetaballs;

float skyboxVertices[] = {
    // positions          
     -10.0f,  10.0f, -10.0f,
  -10.0f, -10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
   10.0f,  10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f,
  
  -10.0f, -10.0f,  10.0f,
  -10.0f, -10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f,
  -10.0f,  10.0f,  10.0f,
  -10.0f, -10.0f,  10.0f,
  
   10.0f, -10.0f, -10.0f,
   10.0f, -10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
   
  -10.0f, -10.0f,  10.0f,
  -10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f, -10.0f,  10.0f,
  -10.0f, -10.0f,  10.0f,
  
  -10.0f,  10.0f, -10.0f,
   10.0f,  10.0f, -10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
  -10.0f,  10.0f,  10.0f,
  -10.0f,  10.0f, -10.0f,
  
  -10.0f, -10.0f, -10.0f,
  -10.0f, -10.0f,  10.0f,
   10.0f, -10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
  -10.0f, -10.0f,  10.0f,
   10.0f, -10.0f,  10.0f
};

float cubeVertices[] =
{
    // Coordinates
    -0.5f, -0.5f,  0.5f, // 7--------6
     0.5f, -0.5f,  0.5f, // |        |
     0.5f, -0.5f, -0.5f, // |        |
    -0.5f, -0.5f, -0.5f, // |        |
    -0.5f,  0.5f,  0.5f, // |        |
     0.5f,  0.5f,  0.5f, // 4--------5
     0.5f,  0.5f, -0.5f, // |        |
    -0.5f,  0.5f, -0.5f  // 3--------2
};


unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};


// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h, int argc, char** argv, int& success){

    if (argc <= 2)
    {
        std::cout << "Too few arguments" << std::endl;
        std::cout << "./program <number of metaballs> <size of the cube for marching algorithm>" << std::endl;
        std::cout << "Example: ./program 3 0.15" << std::endl;
        success = -1;
        return;
    } else {
        try {
            numberOfMetaballs = std::stoi(argv[1]); 
            cubeSize = std::stof(argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            success = -1;
            return;
        }  
    }
    

	// The window we'll be rendering to
	m_window = NULL;

    m_width = w;
    m_height = h;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
	}
    //Use OpenGL 3.3 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    // We want to request a double buffer for smooth updating.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //Create window
    m_window = SDL_CreateWindow( "Lab",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            m_width,
                            m_height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

    // Check if Window did not create.
    if( m_window == NULL ){
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }

    //Create an OpenGL Graphics Context
    m_openGLContext = SDL_GL_CreateContext( m_window );
    if( m_openGLContext == NULL){
        std::cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }

    // Initialize GLAD Library
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        std::cerr << "Failed to iniitalize GLAD\n";
        exit(EXIT_FAILURE);
    }

    // If initialization succeeds then print out a list of errors in the constructor.
    SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL errors detected during initialization\n\n");

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}


//Loops forever!
void SDLGraphicsProgram::SetLoopCallback(std::function<void(void)> callback){

    g_camera.SetCameraEyePosition(0.0f, 0.0f, 0.0f);
    
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // Set the camera speed for how fast we move.
    float cameraSpeed = 1.0f;

    //Setting up the graphics pipeline
    glViewport(0, 0, m_width, m_height);

    //Setting up the SkyBox

    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    // Position attribute for the vertices of the Skybox
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, remember: do NOT unbind the EBO, keep it bound to this VAO)
    glBindVertexArray(0);

    //Setting up metaballs
    std::vector<Metaball> metaballs = createRandomMetaballs(numberOfMetaballs);

    std::vector<GLfloat> vertices, normals, colors;
    generateMesh(metaballs, fTargetValue, vertices, normals, colors);

    // Create buffer IDs
    GLuint VBOs[3], VAO;
    glGenBuffers(3, VBOs);
    glGenVertexArrays(1, &VAO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); // Vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1); // Normal vector attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // Colors
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(2); // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // Unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Setting up the cube

    /* GLuint cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
    glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    */

    Shader skyboxShader;
    Shader metaballShader;
    //Shader cubeShader;

    skyboxShader.CreateShader(skyboxShader.LoadShader("./shaders/skyboxVert.glsl"), skyboxShader.LoadShader("./shaders/skyboxFrag.glsl"));
    //cubeShader.CreateShader(cubeShader.LoadShader("./shaders/vert.glsl"), cubeShader.LoadShader("./shaders/frag.glsl"));
    metaballShader.CreateShader(metaballShader.LoadShader("./shaders/vertMeta.glsl"), metaballShader.LoadShader("./shaders/fragMeta.glsl"));
    
    

    std::vector<std::string> facemap =
	{
		"./assets/textures/skybox/Box_Right.bmp",
		"./assets/textures/skybox/Box_Left.bmp",
		"./assets/textures/skybox/Box_Top.bmp",
		"./assets/textures/skybox/Box_Bottom.bmp",
		"./assets/textures/skybox/Box_Front.bmp",
		"./assets/textures/skybox/Box_Back.bmp"
	};

    unsigned int cubemapTexture = loadCubemap(facemap);

    // Center our mouse
    SDL_WarpMouseInWindow(m_window,m_width/2,m_height/2);

    // Get a pointer to the keyboard state
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    int state = 0;

    glm::vec3 lightDir = glm::vec3(0.0f, -1.0f, -1.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 0.95f, 0.8f);
    
    Uint32 frameStart;
    int frameTime;

    // While application is running
    while(!quit){

        frameStart = SDL_GetTicks();

        // Invoke(i.e. call) the callback function
        //callback();

        //Handle events on queue
        while(SDL_PollEvent( &e ) != 0){

            glGetError();
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if(e.type == SDL_QUIT){
                quit = true;
            }
            if(e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q)){
			    std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
                quit = true;
            }
            // Handle keyboard input for the camera class
            if(e.type==SDL_MOUSEMOTION){
                // Handle mouse movements
                std::cout << "Mouse Motion" << std::endl;
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                //renderer->GetCamera(0)->MouseLook(mouseX, mouseY);
                g_camera.MouseLook(mouseX, mouseY);
            }
        } // End SDL_PollEvent loop.

        

        // Move left or right
        if(keyboardState[SDL_SCANCODE_LEFT]){
            g_camera.MoveLeft(cameraSpeed);
            std::cout << "Pressed Left Key\n";
        }else if(keyboardState[SDL_SCANCODE_RIGHT]){
            g_camera.MoveRight(cameraSpeed);
            std::cout << "Pressed Right Key\n";
        }

        // Move forward or back
        if(keyboardState[SDL_SCANCODE_UP]){
            g_camera.MoveForward(cameraSpeed);
        }else if(keyboardState[SDL_SCANCODE_DOWN]){
            g_camera.MoveBackward(cameraSpeed);
        }

        // Move up or down
        if(keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT])   {
            g_camera.MoveUp(cameraSpeed);
        }else if(keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL]){
            g_camera.MoveDown(cameraSpeed);
        }

        if(keyboardState[SDL_SCANCODE_1]){
            std::cout << "Pressed 1\n";
            state = 1;
        }else if(keyboardState[SDL_SCANCODE_2]){
            std::cout << "Pressed 2\n";
            state = 2;
        }

        if( keyboardState[ SDL_SCANCODE_W ] )
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }else{
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }

        //Updating the metaballs
        for (size_t i = 0; i < metaballs.size(); i++)
        {
            metaballs[i].update(0.3f);
        }
        
        std::vector<GLfloat> vertices_update, normals_update, colors_update;
        generateMesh(metaballs, fTargetValue, vertices_update, normals_update, colors_update);


        glBindVertexArray(VAO);

        // Update Vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices_update.size() * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW); // Allocate buffer
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_update.size() * sizeof(GLfloat), vertices_update.data()); // Update buffer

        // Update Normals
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, normals_update.size() * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normals_update.size() * sizeof(GLfloat), normals_update.data());

        // Update Colors
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, colors_update.size() * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, colors_update.size() * sizeof(GLfloat), colors_update.data());

        // Unbind the VBO and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
       // glCullFace(GL_FRONT);
        //glFrontFace(GL_CCW);
		
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        
        skyboxShader.SetUniform1i("faceIndex", state);

        glm::mat4 view = glm::mat4(glm::mat3(g_camera.GetWorldToViewmatrix()));
        //glm::mat4 view = g_camera.GetWorldToViewmatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),((float)m_width)/((float)m_height),0.1f,512.0f);

        skyboxShader.SetUniformMatrix4fv("view", &view[0][0]);
        skyboxShader.SetUniformMatrix4fv("projection", &projection[0][0]);
        skyboxShader.SetUniform1i("skybox", 0);

        glBindVertexArray(skyboxVAO);
        
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        skyboxShader.Unbind();

        glDepthMask(GL_TRUE);

        metaballShader.Bind();
        glActiveTexture(GL_TEXTURE0); // Use texture unit 0 for the skybox
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-10.0f));
        metaballShader.SetUniformMatrix4fv("model", &model[0][0]);
        metaballShader.SetUniformMatrix4fv("view", &g_camera.GetWorldToViewmatrix()[0][0]); // Same view as skybox
        metaballShader.SetUniformMatrix4fv("projection", &projection[0][0]); // Same projection as skybox
        metaballShader.SetUniform3f("lightDir", lightDir.x, lightDir.y, lightDir.z);
        metaballShader.SetUniform3f("viewPos", g_camera.GetEyeXPosition(), g_camera.GetEyeYPosition(), g_camera.GetEyeZPosition());
        metaballShader.SetUniform3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        metaballShader.SetUniform1i("skybox", 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices_update.size()/3);
        metaballShader.Unbind();

        //cubeShader.Bind();

        /* glBindVertexArray(cubeVAO);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-5.0f)); // Modify as needed for cube transformation
        cubeShader.SetUniformMatrix4fv("model", &model[0][0]);
        cubeShader.SetUniformMatrix4fv("view", &g_camera.GetWorldToViewmatrix()[0][0]); // Same view as skybox
        cubeShader.SetUniformMatrix4fv("projection", &projection[0][0]); // Same projection as skybox

        // Draw the cube
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        cubeShader.Unbind();
        */
        
        // Delay to slow things down just a bit!
        //SDL_Delay(25);  // TODO: You can change this or implement a frame
                        // independent movement method if you like.
      	//Update screen of our specified window
        frameTime = SDL_GetTicks() - frameStart;

        if(1000 / 90 > frameTime) {
            SDL_Delay(1000 / 90 - frameTime);
        }
      	SDL_GL_SwapWindow(GetSDLWindow());
	}
    //Disable text input
    SDL_StopTextInput();

    glDeleteTextures(1, &cubemapTexture);
    //glDeleteBuffers(1, &cubeVBO);
    //glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &skyboxVAO);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(3, VBOs);

}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
