#version 330 core

//  Interpolated across the triangle from the three vertex colors
in vec3 vColor;

out vec4 fragColor;

void main()
{
   fragColor = vec4(vColor, 1.0);
}
