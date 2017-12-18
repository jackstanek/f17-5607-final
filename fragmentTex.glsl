#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform int texID;
uniform int pass_mode; /* 0 = diffuse, 1 = normal */

void main() {
    if (pass_mode == 0) {
        if (texID == -1) {
            outColor = vec4(Color, 1.0);
        } else if (texID == 0) {
            outColor = texture(tex0, texcoord);
        } else if (texID == 1) {
            outColor = texture(tex1, texcoord);
        } else {
            outColor = vec4(1,0,0,1);
            return; //This was an error, stop lighting!
        }
    } else if (pass_mode == 1) {
        outColor = vec4(normal, 1.0);
    } else if (pass_mode == 2) {
        outColor = vec4(pos, 1.0);
    }
}
