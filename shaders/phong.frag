#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec4 vLightPos;

out vec4 fragColor;

uniform vec3 uAlbedo;
uniform float uSpecular;
uniform float uShininess;

uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform vec3 uAmbient;

uniform vec3 uEye;

uniform sampler2DShadow uShadowMap;

uniform bool uNormalView;

void main()
{
   vec3 N = normalize(vNormal);
   if (uNormalView)
   {
      fragColor = vec4(0.5 * N + 0.5, 1.0);
      return;
   }
   vec3 L = normalize(uLightDirection);
   vec3 V = normalize(uEye - vWorldPos);
   vec3 H = normalize(L + V);

   float diffuse = max(dot(N, L), 0.0);
   float specular = diffuse > 0.0 ? pow(max(dot(N, H), 0.0), uShininess) : 0.0;
   vec3 map = 0.5 * vLightPos.xyz / vLightPos.w + 0.5;
   float lit = texture(uShadowMap, map);
   vec3 color = uAmbient * uAlbedo + lit * uLightColor * (uAlbedo * diffuse + uSpecular * specular);

   fragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
