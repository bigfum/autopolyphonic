#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vColor;

void main()
{
   //  debug view: map the normal from [-1, 1] to a color in [0, 1]
   vColor = 0.5 * aNormal + 0.5;
   gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
