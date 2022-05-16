#version 400 core
out vec4 fragColor;
uniform sampler3D texture1;
in vec2 texCoord;
uniform float tPos;

void main() {
    fragColor = texture(texture1, vec3(texCoord, tPos));
//    fragColor = vec4(0.0, 0.7, 0.0, 1.0);
}
