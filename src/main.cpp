//  Procedural MIDI instrument stage
//  Entry point: SDL2 window with an OpenGL 3.3 core context
#include "gl/shader.h"
#include <SDL.h>
#include <glad/glad.h>
#include <cstdio>
#include <cstdlib>

static void fatal(const char *msg)
{
   std::fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
   std::exit(1);
}

int main(int, char **)
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
      fatal("SDL_Init failed");

   //  Request a 3.3 core context (forward compatible is required on macOS)
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

   SDL_Window *window = SDL_CreateWindow("autopolyphonic",
                                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
   if (!window)
      fatal("SDL_CreateWindow failed");

   SDL_GLContext context = SDL_GL_CreateContext(window);
   if (!context)
      fatal("SDL_GL_CreateContext failed");
   SDL_GL_SetSwapInterval(1);

   //  Load GL function pointers through SDL
   if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
      fatal("gladLoadGLLoader failed");
   std::printf("OpenGL %s\n", glGetString(GL_VERSION));

   //  Scope so GL objects (the shader) are destroyed before the context
   {
      Shader basic(SHADER_DIR "/basic.vert", SHADER_DIR "/basic.frag");

      const float vertices[] = {
          -0.5f,
          -0.5f,
          0.0f,
          1.0f,
          0.2f,
          0.2f,
          0.5f,
          -0.5f,
          0.0f,
          0.2f,
          1.0f,
          0.2f,
          0.0f,
          0.5f,
          0.0f,
          0.2f,
          0.4f,
          1.0f,
      };

      GLuint vao, vbo;
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      //  attribute 0: position, 3 floats at the start of each 6-float vertex
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
      glEnableVertexAttribArray(0);
      //  attribute 1: color, 3 floats after the position
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
      glEnableVertexAttribArray(1);
      glBindVertexArray(0);

      bool running = true;
      while (running)
      {
         SDL_Event event;
         while (SDL_PollEvent(&event))
         {
            if (event.type == SDL_QUIT)
               running = false;
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
               running = false;
         }

         int w, h;
         SDL_GL_GetDrawableSize(window, &w, &h);
         glViewport(0, 0, w, h);

         glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         basic.use();
         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 3);

         SDL_GL_SwapWindow(window);
      }

      glDeleteBuffers(1, &vbo);
      glDeleteVertexArrays(1, &vao);
   }

   SDL_GL_DeleteContext(context);
   SDL_DestroyWindow(window);
   SDL_Quit();
   return 0;
}
