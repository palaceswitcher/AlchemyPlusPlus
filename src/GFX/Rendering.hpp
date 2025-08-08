#ifndef RENDERING_HPP_
#define RENDERING_HPP_
#include <SDL3/SDL.h>
#include "Sprite.hpp"

namespace GFX {
void renderSprite(SDL_Renderer* ren, double deltaTime, Sprite* spr);
}

#endif
