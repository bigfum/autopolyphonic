#include "gl/shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

static std::string readFile(const std::string &path)
{
   std::ifstream file(path);
   if (!file)
   {
      std::fprintf(stderr, "Cannot open %s\n", path.c_str());
      std::exit(1);
   }
   std::stringstream text;
   text << file.rdbuf();
   return text.str();
}

static GLuint compile(GLenum type, const std::string &path)
{
   const std::string source = readFile(path);
   const char *text = source.c_str();
   GLuint shader = glCreateShader(type);
   glShaderSource(shader, 1, &text, nullptr);
   glCompileShader(shader);

   GLint ok;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
   if (!ok)
   {
      char log[4096];
      glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
      std::fprintf(stderr, "Error compiling %s:\n%s\n", path.c_str(), log);
      std::exit(1);
   }
   return shader;
}

Shader::Shader(const std::string &vertPath, const std::string &fragPath)
{
   GLuint vert = compile(GL_VERTEX_SHADER, vertPath);
   GLuint frag = compile(GL_FRAGMENT_SHADER, fragPath);
   program = glCreateProgram();
   glAttachShader(program, vert);
   glAttachShader(program, frag);
   glLinkProgram(program);
   glDeleteShader(vert);
   glDeleteShader(frag);

   GLint ok;
   glGetProgramiv(program, GL_LINK_STATUS, &ok);
   if (!ok)
   {
      char log[4096];
      glGetProgramInfoLog(program, sizeof(log), nullptr, log);
      std::fprintf(stderr, "Error linking %s + %s:\n%s\n", vertPath.c_str(), fragPath.c_str(), log);
      std::exit(1);
   }
}

Shader::~Shader()
{
   glDeleteProgram(program);
}

void Shader::use() const
{
   glUseProgram(program);
}

void Shader::set(const char *name, float value) const
{
   glUniform1f(glGetUniformLocation(program, name), value);
}

void Shader::set(const char *name, const glm::vec2 &value) const
{
   glUniform2fv(glGetUniformLocation(program, name), 1, glm::value_ptr(value));
}

void Shader::set(const char *name, const glm::vec3 &value) const
{
   glUniform3fv(glGetUniformLocation(program, name), 1, glm::value_ptr(value));
}

void Shader::set(const char *name, const glm::mat3 &value) const
{
   glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const char *name, const glm::mat4 &value) const
{
   glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(value));
}
