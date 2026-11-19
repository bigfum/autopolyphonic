#pragma once
#include "gl/mesh.h"
#include "gl/shader.h"
#include "render/model.h"
#include <vector>

class Stage
{
public:
   void add(const Model &model, const glm::mat4 &placement = glm::mat4(1), int channel = 0);
   void strike(int channel, int note, float time);
   void draw(const Shader &shader, float time) const;

private:
   struct Part
   {
      Mesh mesh;
      Material material;
      glm::mat4 model;
      int channel;
      int note;
      Motion motion;
      float wobble;
      float struck;
   };

   std::vector<Part> parts;
};

Model buildDeck(float width, float depth, float height);

Model buildCurtain(float width, float height);
