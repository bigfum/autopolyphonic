#include "render/stage.h"
#include "geometry/extrude.h"
#include "geometry/profile.h"
#include <glm/gtc/matrix_transform.hpp>

void Stage::add(const Model &model, const glm::mat4 &placement)
{
   for (const Model::Part &p : model.parts)
      parts.push_back({Mesh(p.mesh), p.material, glm::translate(placement, p.position), p.note});
}

void Stage::draw(const Shader &shader) const
{
   for (const Part &p : parts)
   {
      shader.set("uModel", p.model);
      shader.set("uNormalMatrix", glm::mat3(glm::transpose(glm::inverse(p.model))));
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
