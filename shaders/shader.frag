#version 400 core
out vec4 FragColor;
uniform sampler3D texture1;
in vec2 texCoord;
uniform float tPos;

void main() {
    FragColor = texture(texture1, vec3(texCoord, tPos));
}
