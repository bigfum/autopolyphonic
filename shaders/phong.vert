#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat3 uNormalMatrix;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpace;

out vec3 vWorldPos;
out vec3 vNormal;
out vec4 vLightPos;

void main()
{
   vec4 world = uModel * vec4(aPosition, 1.0);
   vWorldPos = world.xyz;
   vNormal = uNormalMatrix * aNormal;
   vLightPos = uLightSpace * world;
   gl_Position = uProjection * uView * world;
}
