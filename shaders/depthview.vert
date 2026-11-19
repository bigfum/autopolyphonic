#version 330 core

out vec2 vUV;

void main()
{
   vUV = vec2(gl_VertexID & 1, gl_VertexID >> 1);
   gl_Position = vec4(2.0 * vUV - 1.0, 0.0, 1.0);
}
