#version 400 core
layout (location = 0) in vec3 vCoord;
layout (location = 1) in vec2 uv;

out highp vec2 texCoord;

void main()
{
   gl_Position = vec4(vCoord.x, vCoord.y, vCoord.z, 1.0);
   texCoord = uv;
}
