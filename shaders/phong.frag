#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;

out vec4 fragColor;

//  material (linear colors)
uniform vec3 uAlbedo;
uniform float uSpecular;
uniform float uShininess;

//  one directional light, pointing toward the light
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform vec3 uAmbient;

uniform vec3 uEye;

//  debug: color by world-space normal instead of lighting
uniform bool uNormalView;

void main()
{
   //  interpolated normals are no longer unit length
   vec3 N = normalize(vNormal);
   if (uNormalView)
   {
      //  x, y, z in [-1, 1] -> red, green, blue in [0, 1]
      fragColor = vec4(0.5 * N + 0.5, 1.0);
      return;
   }
   vec3 L = normalize(uLightDirection);
   vec3 V = normalize(uEye - vWorldPos);
   //  Blinn-Phong: specular from the half vector between light and eye
   vec3 H = normalize(L + V);

   float diffuse = max(dot(N, L), 0.0);
   float specular = diffuse > 0.0 ? pow(max(dot(N, H), 0.0), uShininess) : 0.0;
   vec3 color = uAmbient * uAlbedo + uLightColor * (uAlbedo * diffuse + uSpecular * specular);

   //  linear to sRGB for the display
   fragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
