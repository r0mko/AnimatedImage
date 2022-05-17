#version 400 core
out vec4 fragColor;
in vec2 texCoord;
uniform sampler3D texture1;
uniform float t1;
uniform float t2;
uniform int depth;
uniform bool blur;

void main() {
    if (t1 == t2 || !blur) {
        float t = t1;
        if (!blur) {
            t = (round(t1 * depth) - 0.5) / depth;
        }
        fragColor = texture(texture1, vec3(texCoord, t));
    } else {
        vec4 col = texture(texture1, vec3(texCoord, t1));
        int c = 1;
        float increment = 1.0 / depth;
        float tf = ceil(t1 * depth) / depth;
        float tl = floor(t2 * depth) / depth;
        while (tf < tl) {
            col += texture(texture1, vec3(texCoord, tf));
            tf += increment;
            ++c;
        }
        col += texture(texture1, vec3(texCoord, t2));
        ++c;
        fragColor = col / c;
    }

//    fragColor = vec4(0.0, 0.7, 0.0, 1.0);
}
