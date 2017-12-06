#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;

out vec4 outColor;

uniform vec3 lightPos;
const float ambient = 0.2;

void main() {
    vec3 lightDir = normalize(pos - lightPos);
    vec3 diffuseC = Color * clamp(dot(lightDir, normal), 0, 1);

    vec3 viewDir = normalize(pos); //We know the eye is at (0,0)
    vec3 reflectDir = -reflect(lightDir, normal);
    vec3 halfVec = normalize(viewDir + lightDir);
    float spec = 0.0;

    if (dot(lightDir, normal) > 0.0) {
        spec = clamp(dot(normal, halfVec), 0, 1);
        //spec = clamp(dot(reflectDir, viewDir), 0, 1);
    }

    vec3 specularC = vec3(1, 1, 1) * pow(spec, 16);
    vec3 ambC = Color * ambient;
    outColor = vec4(diffuseC + specularC + ambC, 1.0);
}
