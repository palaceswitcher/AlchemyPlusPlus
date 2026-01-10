#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_
#include <SDL3/SDL.h>

namespace GFX {
extern SDL_Renderer* renderer;
extern SDL_Window* window;
int getWindowWidth();
int getWindowHeight();
}

#endif
