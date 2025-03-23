#include "Text.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Resource.hpp"
#include "GameHandler.hpp"
#include <string>
#include <unordered_map>
#include <iostream>

std::unordered_map<int, GfxResource> elementTextTextureBuf; //Every texture for an element's strings

GfxResource GFX::renderText(SDL_Renderer* ren, std::string text, SDL_Color color) {
	std::cout << text << '\n';
	const char* textRaw = text.c_str();
	SDL_Surface* textSurf = TTF_RenderText_Blended(Game::getFont(), text.c_str(), 0, color);

	GfxResource texture = GfxResource(ren, textSurf);
	return texture;
}

GfxResource GFX::getElemNameTexture(SDL_Renderer* ren, int id) {
	if (!elementTextTextureBuf.contains(id)) {
		std::string text = Game::getElementName(id);
		std::cout << text << '\n';
		SDL_Surface* textSurf = TTF_RenderText_Blended(Game::getFont(), text.c_str(), 0, {255,255,255,255});
		GfxResource res = GfxResource(ren, textSurf);
		SDL_SetTextureScaleMode(res.texture, SDL_SCALEMODE_NEAREST);
		elementTextTextureBuf.insert({id, res});
	}
	return elementTextTextureBuf[id];
}
