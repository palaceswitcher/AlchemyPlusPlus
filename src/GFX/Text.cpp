#include "Text.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "GraphicsContext.hpp"
#include "Resource.hpp"
#include "GameHandler.hpp"
#include "RenderDefs.hpp"
#include <string>
#include <unordered_map>
#include <iostream>

std::unordered_map<int, GfxResource> elementTextTextureBuf; // Every texture for an element's strings

void GFX::renderText(std::string text, float x, float y, SDL_Color color, float opacity, int flags) {
	SDL_Renderer* ren = GFX::renderer;
	SDL_Surface* textSurf = TTF_RenderText_Blended(Game::getFont(), text.c_str(), 0, color);
	GfxResource res(ren, textSurf);
	SDL_SetTextureScaleMode(res.texture, SDL_SCALEMODE_NEAREST); // Text shouldn't blur when scaled

	if ((flags & ORIGIN_RIGHT) != 0) {
		x -= res.w;
	}
	if ((flags & ORIGIN_BOTTOM) != 0) {
		y -= res.h;
	}
	SDL_FRect textBox = {x, y, res.w, res.h};
	SDL_RenderTexture(ren, res.texture, nullptr, &textBox); // Draw text
	SDL_DestroyTexture(res.texture);
}

void GFX::renderTextToRes(GfxResource& res, std::string text, SDL_Color color) {
	SDL_Surface* textSurf = TTF_RenderText_Blended(Game::getFont(), text.c_str(), 0, color);
	res.loadTexture(GFX::renderer, textSurf);
	SDL_SetTextureScaleMode(res.texture, SDL_SCALEMODE_NEAREST); // Text shouldn't blur when scaled
}

GfxResource GFX::getElemNameTexture(int id) {
	if (!elementTextTextureBuf.contains(id)) {
		elementTextTextureBuf[id] = GfxResource();
		renderTextToRes(elementTextTextureBuf[id], Game::getElementName(id), {255,255,255,255});
	}
	return elementTextTextureBuf[id];
}
