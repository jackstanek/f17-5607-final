#version 150 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texDiffuse;
uniform int time;

void main() {
    outColor = texture(texDiffuse, vec2(1 - Texcoord.x, Texcoord.y));
}
