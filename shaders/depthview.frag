#version 330 core

in vec2 vUV;
out vec4 fragColor;

uniform sampler2D uDepth;

void main()
{
   fragColor = vec4(vec3(texture(uDepth, vUV).r), 1.0);
}
