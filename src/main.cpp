//  Procedural MIDI instrument stage
//  Entry point: SDL2 window with an OpenGL 3.3 core context
#include "camera.h"
#include "gl/mesh.h"
#include "gl/shader.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
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

   if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
      fatal("gladLoadGLLoader failed");
   std::printf("OpenGL %s\n", glGetString(GL_VERSION));

   {
      Shader basic("shaders/basic.vert", "shaders/basic.frag");

      Mesh floor(makePlane(4.0f));
      Mesh ball(makeSphere(0.6f, 24, 48));
      Camera camera;

      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);

      bool running = true;
      while (running)
      {
         SDL_Event event;
         while (SDL_PollEvent(&event))
         {
            if (event.type == SDL_QUIT)
               running = false;
            else if (event.type == SDL_KEYDOWN)
            {
               switch (event.key.keysym.sym)
               {
               case SDLK_ESCAPE:
                  running = false;
                  break;
               case SDLK_RIGHT:
                  camera.th += 5;
                  break;
               case SDLK_LEFT:
                  camera.th -= 5;
                  break;
               case SDLK_UP:
                  camera.ph += 5;
                  break;
               case SDLK_DOWN:
                  camera.ph -= 5;
                  break;
               case SDLK_PAGEDOWN:
                  camera.dim += 0.1f;
                  break;
               case SDLK_PAGEUP:
                  camera.dim = std::max(camera.dim - 0.1f, 1.0f);
                  break;
               case SDLK_0:
                  camera.th = camera.ph = 0;
                  break;
               case SDLK_EQUALS:
                  camera.fov = std::min(camera.fov + 1, 179.0f);
                  break;
               case SDLK_MINUS:
                  camera.fov = std::max(camera.fov - 1, 1.0f);
                  break;
               }
            }
            else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK))
            {
               camera.th += 0.3f * event.motion.xrel;
               camera.ph += 0.3f * event.motion.yrel;
            }
         }
         camera.ph = std::clamp(camera.ph, -85.0f, 85.0f);

         int w, h;
         SDL_GL_GetDrawableSize(window, &w, &h);
         glViewport(0, 0, w, h);

         glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         basic.use();
         basic.set("uView", camera.view());
         basic.set("uProjection", camera.projection(static_cast<float>(w) / h));

         basic.set("uModel", glm::mat4(1));
         floor.draw();
         basic.set("uModel", glm::translate(glm::mat4(1), glm::vec3(0, 0.6f, 0)));
         ball.draw();
         basic.set("uModel", glm::scale(glm::translate(glm::mat4(1), glm::vec3(1.2f, 0.25f, 0.7f)), glm::vec3(0.4f)));
         ball.draw();

         SDL_GL_SwapWindow(window);
      }
   }

   SDL_GL_DeleteContext(context);
   SDL_DestroyWindow(window);
   SDL_Quit();
   return 0;
}
