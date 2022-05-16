#version 320 es
out lowp vec4 fragColor;
uniform highp sampler3D texture1;
in highp vec2 texCoord;
uniform highp float tPos;

void main() {
//    FragColor = texture(texture1, vec3(texCoord, tPos));
    fragColor = vec4(1.0, 0.7, 0.0, 1.0);
}
