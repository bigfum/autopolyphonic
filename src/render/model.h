#pragma once
#include "geometry/shapes.h"
#include "render/material.h"
#include <utility>
#include <vector>

//  how a part moves when struck
enum class Motion
{
   None,
   Swing,  //  rock about local x axis
   Bounce, //  dips down and rings back up, rocking about local x too
};

struct Model
{
   struct Part
   {
      MeshData mesh;
      Material material;
      glm::vec3 position; //  rest position. animation will offset from here
      int note;
      Motion motion;
      float wobble; //  radians
   };

   std::vector<Part> parts;

   void add(MeshData mesh, const Material &material, const glm::vec3 &position = glm::vec3(0), int note = -1,
            Motion motion = Motion::None, float wobble = 0)
   {
      parts.push_back({std::move(mesh), material, position, note, motion, wobble});
   }
};
