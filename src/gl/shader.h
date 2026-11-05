#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
   Shader(const std::string &vertPath, const std::string &fragPath);
   ~Shader();
   Shader(const Shader &) = delete;
   Shader &operator=(const Shader &) = delete;

   void use() const;
   void set(const char *name, const glm::mat4 &value) const;
   GLuint id() const { return program; }

private:
   GLuint program = 0;
};
