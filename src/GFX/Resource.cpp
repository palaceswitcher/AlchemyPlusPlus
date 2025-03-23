#include "Resource.hpp"
#include <SDL3/SDL.h>
#include <string>

GfxResource::GfxResource(SDL_Renderer* ren, SDL_Surface* surf) {
	texture = SDL_CreateTextureFromSurface(ren, surf);
	SDL_GetTextureSize(texture, &w, &h);
	SDL_DestroySurface(surf);
}

void GfxResource::loadTexture(SDL_Renderer* ren, SDL_Surface* surf) {
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(ren, surf);
	SDL_GetTextureSize(texture, &w, &h);
	SDL_DestroySurface(surf);
}

GfxResource::~GfxResource() {
	//SDL_DestroyTexture(texture);
}
