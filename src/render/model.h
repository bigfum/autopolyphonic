#pragma once
#include "geometry/shapes.h"
#include "render/material.h"
#include <utility>
#include <vector>

//  an instrument (or any object) as CPU-side parts
//  parts that sound a note stay separate so they can have their own visual fx
struct Model
{
   struct Part
   {
      MeshData mesh;
      Material material;
      glm::vec3 position; //  rest position. animation will offset from here
      int note;           //  MIDI note this part plays, or -1 for structure
   };

   std::vector<Part> parts;

   void add(MeshData mesh, const Material &material, const glm::vec3 &position = glm::vec3(0), int note = -1)
   {
      parts.push_back({std::move(mesh), material, position, note});
   }
};
