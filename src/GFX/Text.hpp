#ifndef TEXT_HPP_
#define TEXT_HPP_
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Resource.hpp"
#include <string>

namespace GFX {
void renderText(GfxResource& res, SDL_Renderer* ren, std::string text, SDL_Color color);
GfxResource getElemNameTexture(SDL_Renderer* ren, int id);
}

#endif
