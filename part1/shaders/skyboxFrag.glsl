#version 410 core
in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox; // The cubemap texture
uniform int faceIndex; // Debug: Index to determine which face to display

void main()
{   
    vec3 direction;
    
    if(faceIndex == 1) {
        direction = vec3(TexCoords.x, -0.1, TexCoords.z);
    } else if(faceIndex == 2) {
        direction = vec3(TexCoords.x, 1, TexCoords.z);
    }
     else {
        direction = TexCoords;
        
    }

    FragColor = texture(skybox, direction);
}
