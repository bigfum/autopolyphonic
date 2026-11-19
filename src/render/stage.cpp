#include "render/stage.h"
#include "geometry/extrude.h"
#include "geometry/profile.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
   float ring(float t, float hz, float decay)
   {
      return std::exp(-decay * t) * std::sin(glm::two_pi<float>() * hz * t);
   }

   glm::mat4 motionAt(Motion motion, float wobble, float t)
   {
      if (t < 0 || t > 4)
         return glm::mat4(1);
      switch (motion)
      {
      case Motion::Swing:
         return glm::rotate(glm::mat4(1), 0.1f * ring(t, 1.5f, 1.5f), glm::vec3(1, 0, 0));
      case Motion::Bounce:
      {
         const float r = ring(t, 6, 5);
         return glm::rotate(glm::translate(glm::mat4(1), glm::vec3(0, -0.005f * r, 0)), wobble * r, glm::vec3(1, 0, 0));
      }
      default:
         return glm::mat4(1);
      }
   }
}

void Stage::add(const Model &model, const glm::mat4 &placement, int channel)
{
   for (const Model::Part &p : model.parts)
      parts.push_back({Mesh(p.mesh), p.material, glm::translate(placement, p.position), channel, p.note, p.motion, p.wobble, -1e9f});
}

void Stage::strike(int channel, int note, float time)
{
   for (Part &p : parts)
      if (p.channel == channel && p.note == note)
         p.struck = time;
}

void Stage::draw(const Shader &shader, float time) const
{
   for (const Part &p : parts)
   {
      const glm::mat4 model = p.model * motionAt(p.motion, p.wobble, time - p.struck);
      shader.set("uModel", model);
      shader.set("uNormalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
      p.material.apply(shader);
      p.mesh.draw();
   }
}

Model buildDeck(float width, float depth, float height)
{
   const glm::mat4 m(glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, -1, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 0, 1));
   MeshData mesh;
   mesh.append(extrude(roundedRect(width, depth, 0.15f, 2), height), m);
   Model deck;
   deck.add(std::move(mesh), materials::stage);
   return deck;
}

Model buildCurtain(float width, float height)
{
   const float amplitude = 0.08f;
   const float foldWidth = 0.3f;
   const float thickness = 0.02f;
   const int steps = static_cast<int>(width / foldWidth) * 12;

   std::vector<glm::vec2> outline;
   for (int i = 0; i <= steps; i++)
   {
      const float x = width * i / steps - 0.5f * width;
      outline.push_back({x, amplitude * std::sin(glm::two_pi<float>() * x / foldWidth) - 0.5f * thickness});
   }
   for (int i = steps; i >= 0; i--)
   {
      const float x = width * i / steps - 0.5f * width;
      outline.push_back({x, amplitude * std::sin(glm::two_pi<float>() * x / foldWidth) + 0.5f * thickness});
   }

   const glm::mat4 m(glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, -1, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 0, 1));
   MeshData mesh;
   mesh.append(extrude(outline, height), m);
   Model curtain;
   curtain.add(std::move(mesh), materials::velvet);
   return curtain;
}
