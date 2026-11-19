#pragma once
#include "gl/mesh.h"
#include "gl/shader.h"
#include "render/model.h"
#include <vector>

class Stage
{
public:
   void add(const Model &model, const glm::mat4 &placement = glm::mat4(1));
   void draw(const Shader &shader) const;

private:
   struct Part
   {
      Mesh mesh;
      Material material;
      glm::mat4 model;
      int note;
   };

   std::vector<Part> parts;
};

Model buildDeck(float width, float depth, float height);
