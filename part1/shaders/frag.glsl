#version 410 core
out vec4 FragColor;

in vec3 FragPos;

void main() {
    vec3 redColor = vec3(1.0, 0.0, 0.0); // RGB Red
    FragColor = vec4(redColor, 1.0); // Full opacity
}
