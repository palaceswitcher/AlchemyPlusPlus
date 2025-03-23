#include "Resource.hpp"
#include <SDL3/SDL.h>
#include <string>

int n = 0;
GfxResource::GfxResource(SDL_Renderer* ren, SDL_Surface* surf) {
	texture = SDL_CreateTextureFromSurface(ren, surf);
	SDL_GetTextureSize(texture, &w, &h);
	std::string str = std::to_string(n);
	SDL_FRect box = {0, 32*n, w, h};
	SDL_RenderTexture(ren, texture, nullptr, &box);
	//SDL_SaveBMP(surf, str.c_str());
	SDL_DestroySurface(surf);
	n++;
}

GfxResource::~GfxResource() {
	SDL_DestroyTexture(texture);
}
