#version 330 core

layout(location = 0) in vec2 aPixel;
layout(location = 1) in vec2 aUV;

//  drawable size in pixels
uniform vec2 uViewport;

out vec2 vUV;

void main()
{
   vUV = aUV;
   //  pixels (origin bottom left) to normalized device coordinates
   gl_Position = vec4(2.0 * aPixel / uViewport - 1.0, 0.0, 1.0);
}
