#version 410 core
layout(location=0) in vec3 position; // Skybox vertices

uniform mat4 view; // View matrix, typically without translation
uniform mat4 projection; // Projection matrix

out vec3 TexCoords; // Texture coordinates for the cubemap

void main()
{
    vec4 pos = projection * view * vec4(position, 1.0f);
    gl_Position = pos; // Modify the w component to ensure the skybox is always at the farthest depth

    TexCoords = position; 

    
}
