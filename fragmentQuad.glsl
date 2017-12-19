#version 150 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;
uniform sampler2D texPos;

uniform vec4 in_lightDir;
uniform int time;
uniform int render_mode;

const float ambient = 0.0;

void main() {
    vec4 raw_color = texture(texDiffuse, Texcoord);
    vec4 raw_normal = texture(texNormal, Texcoord);
    vec4 raw_pos = texture(texPos, Texcoord);

    if (render_mode == 0) {
        // vec3 lightDir = -normalize(in_lightDir.xyz);
        vec3 lightDir = normalize(-texture(texPos, Texcoord).xyz);

        vec3 normal = raw_normal.xyz;
        vec3 pos = raw_pos.xyz;

        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = -normalize(pos);

        float diffuse = clamp(dot(lightDir, normal), 0, 1);
        float specular = clamp(dot(reflectDir, viewDir), 0, 1) * 0.3;

        float dist = length(texture(texPos, Texcoord).xyz);
        dist = clamp(dist*dist - 12 + 2 * sin(time/1000.f), 1, 100);
        outColor = 1/dist * (raw_color * (diffuse + ambient) + specular * vec4(1, 1, 1, 1)) * vec4(1, 0.8, 0.5, 1.0);
    } else if (render_mode == 1) {
        outColor = raw_color;
    } else if (render_mode == 2) {
        outColor = raw_normal;
    } else if (render_mode == 3) {
        outColor = raw_pos;
    }
}
