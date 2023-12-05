#version 410 core
layout(location = 0) in vec3 vertexPosition; // Position of the vertex
layout(location = 1) in vec3 vertexNormal;   // Normal of the vertex
layout(location = 2) in vec3 vertexColor;    // Color of the vertex

out vec3 fragPosition; // Position for the fragment shader
out vec3 fragNormal;   // Normal for the fragment shader
out vec3 fragColor;    // Color for the fragment shader

uniform mat4 model;      // Model matrix
uniform mat4 view;       // View matrix
uniform mat4 projection; // Projection matrix

void main()
{
    fragPosition = vec3(model * vec4(vertexPosition, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * vertexNormal;
    fragColor = vertexColor;
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
