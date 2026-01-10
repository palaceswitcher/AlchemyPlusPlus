#ifndef TEXT_HPP_
#define TEXT_HPP_
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Resource.hpp"
#include <string>

namespace GFX {
/**
 * Draws text on the screen
 * @param ren SDL3 Renderer
 * @param text String to render
 * @param x
 * @param y
 * @param flags
 * @param color Text color
 */
void renderText(std::string text, float x, float y, SDL_Color color, float opacity = 0.0f, int flags = 0);

/**
 * Draws text to a resource
 * @param res Resource to render to
 * @param ren SDL3 Renderer
 * @param text String to render
 * @param color Text color
 */
void renderTextToRes(GfxResource& res, std::string text, SDL_Color color);

GfxResource getElemNameTexture(int id);
}

#endif
