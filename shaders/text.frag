#version 330 core

in vec2 vUV;

uniform sampler2D uFont;

out vec4 fragColor;

void main()
{
   if (texture(uFont, vUV).r < 0.5)
      discard;
   fragColor = vec4(1.0);
}
