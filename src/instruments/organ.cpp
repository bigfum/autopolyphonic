#include "instruments/organ.h"
#include "geometry/lathe.h"
#include "music/pitch.h"
#include <cmath>

namespace
{
   constexpr float speedOfSound = 343.0f;
   constexpr float footHeight = 0.25f;
   constexpr float lowDiameter = 0.15f;
   constexpr float halvingSemitones = 16; //  diameter halves every 16 semitones, a common organ scale
   constexpr float wall = 0.002f;
   constexpr float gap = 0.01f;

   MeshData pipeMesh(float radius, float length)
   {
      const float top = footHeight + length;
      return lathe({{0, 0},
                    {0.25f * radius, 0},
                    {radius, footHeight},
                    {radius, top},
                    {radius - wall, top},
                    {radius - wall, footHeight},
                    {0, footHeight}},
                   32);
   }
}

Model buildPipeOrgan(int lowNote, int highNote)
{
   Model organ;
   float left = 0;
   float right = 0;
   for (int note = lowNote; note <= highNote; note++)
   {
      //  open at both ends: half a wavelength
      const float length = speedOfSound / (2 * midiToHz(note));
      const float radius = 0.5f * lowDiameter * std::pow(2.0f, -(note - lowNote) / halvingSemitones);

      float x = 0;
      if (note == lowNote)
      {
         left = -radius;
         right = radius;
      }
      else if ((note - lowNote) % 2 == 1)
      {
         x = right + gap + radius;
         right = x + radius;
      }
      else
      {
         x = left - gap - radius;
         left = x - radius;
      }
      organ.add(pipeMesh(radius, length), materials::tin, glm::vec3(x, 0, 0), note);
   }
   return organ;
}
