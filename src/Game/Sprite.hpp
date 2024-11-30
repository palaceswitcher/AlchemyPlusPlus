#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <vector>
#ifndef SPRITE_HPP_ //Include guard
#define SPRITE_HPP_

struct Sprite {
	SDL_Rect* box;
	SDL_Texture* texture {NULL};
	int anim {0};
	double scale {1.0};
};

#endif
