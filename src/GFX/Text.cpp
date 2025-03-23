#include "Text.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Resource.hpp"
#include "GameHandler.hpp"
#include <string>
#include <unordered_map>
#include <iostream>

std::unordered_map<int, GfxResource> elementTextTextureBuf; //Every texture for an element's strings

void GFX::renderText(GfxResource& res, SDL_Renderer* ren, std::string text, SDL_Color color) {
	SDL_Surface* textSurf = TTF_RenderText_Blended(Game::getFont(), text.c_str(), 0, color);
	res.loadTexture(ren, textSurf);
	SDL_SetTextureScaleMode(res.texture, SDL_SCALEMODE_NEAREST); //Text shouldn't blur when scaled
}

GfxResource GFX::getElemNameTexture(SDL_Renderer* ren, int id) {
	if (!elementTextTextureBuf.contains(id)) {
		elementTextTextureBuf[id] = GfxResource();
		renderText(elementTextTextureBuf[id], ren, Game::getElementName(id), {255,255,255,255});
	}
	return elementTextTextureBuf[id];
}
