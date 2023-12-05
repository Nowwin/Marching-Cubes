#version 410 core
in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragColor;

out vec4 color;

uniform vec3 lightDir;    // Direction of the sunlight, normalized
uniform vec3 viewPos;     // Position of the camera/view
uniform vec3 lightColor;  // Color of the sunlight
uniform samplerCube skybox; // Environment map (skybox)

void main() {
    // Ambient light (reduced for metallic effect)
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse light (reduced for metallic effect)
    vec3 norm = normalize(fragNormal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular light (enhanced for metallic, silvery look)
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-viewDir, norm); // Changed to use -viewDir for reflection
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64); // Increased shininess
    vec3 specularColor = vec3(0.95, 0.95, 1.0); // Slightly cool white tone for silver
    vec3 specular = specularStrength * spec * specularColor;

    // Sample from the skybox
    vec3 envColor = texture(skybox, reflectDir).rgb;

    // Blend environment map with existing lighting
    float reflectivity = 0.5;
    vec3 result = mix((ambient + diffuse + specular), envColor, reflectivity);

    color = vec4(result, 1.0);
}
