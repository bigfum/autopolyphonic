#pragma once
#include <glad/glad.h>
#include <string>

class Shader
{
public:
   Shader(const std::string &vertPath, const std::string &fragPath);
   ~Shader();
   Shader(const Shader &) = delete;
   Shader &operator=(const Shader &) = delete;

   void use() const;
   GLuint id() const { return program; }

private:
   GLuint program = 0;
};
