#ifndef RESOURCE_HPP_
#define RESOURCE_HPP_
#include <SDL3/SDL.h>

class GfxResource {
public:
	float w; //Texture width
	float h; //Texture height
	SDL_Texture* texture = nullptr; //Texture

	// Creates a resource from a surface and destroys the surface
	GfxResource(SDL_Renderer* ren, SDL_Surface* surf);
	GfxResource() {};
	void loadTexture(SDL_Renderer* ren, SDL_Surface* surf);
	~GfxResource();
};

#endif
