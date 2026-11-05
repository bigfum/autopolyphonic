#pragma once
#include "geometry/shapes.h"
#include <vector>

//  concert marimba in meters: centered on x, player on +z, feet on y = 0.
//  naturals form the front row, accidentals the raised back row
struct Marimba
{
   struct Bar
   {
      int note;
      glm::vec3 center;
      float thickness;
      MeshData mesh;
   };

   std::vector<Bar> bars;
   MeshData resonators;
   MeshData frame;
};

//  default range is the common 4.3 octave instrument, A2 to C7
Marimba buildMarimba(int lowNote = 45, int highNote = 96);
