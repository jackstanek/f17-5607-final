#version 150 core

in vec3 position;
in vec3 inNormal;

out vec3 Color;
out vec3 normal;
out vec3 pos;

uniform vec3 inColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    Color = inColor;
    gl_Position = proj * view * model * vec4(position,1.0);
    vec4 norm4 = transpose(inverse(model)) * vec4(inNormal, 1.0);
    normal = normalize(norm4.xyz);
    pos = (model * vec4(position, 1.0)).xyz;
}
