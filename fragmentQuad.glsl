#version 150 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texPos;

uniform vec4 in_lightDir;
uniform int time;

const float ambient = 0.3;

void main() {
    vec3 lightDir = -normalize(in_lightDir.xyz);

    vec3 normal = texture(texNormal, Texcoord).xyz;
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = -normalize(texture(texPos, Texcoord).xyz);

    float diffuse = clamp(dot(lightDir, normal), 0, 1);
    float specular = clamp(dot(reflectDir, viewDir), 0, 1) * 0.3;

    vec4 raw_color = texture(texDiffuse, Texcoord);
    float dist = length(texture(texPos, Texcoord).xyz);
    dist = clamp(dist*dist - 9 + 2 * sin(time/1000.f), 1, 3);
    outColor = 1/dist * (raw_color * (diffuse + ambient) + specular * vec4(1, 1, 1, 1));
}
