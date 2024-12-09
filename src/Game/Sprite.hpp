#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <vector>
#ifndef SPRITE_HPP_ //Include guard
#define SPRITE_HPP_

enum ObjectAnimationType {
	ANIM_NONE,
	ANIM_SHRINK,
	ANIM_SHRINK_END,
	ANIM_GROW,
	ANIM_GROW_END,
	ANIM_FADE_IN,
	ANIM_FADE_IN_END
};

struct Sprite {
	SDL_Rect* box;
	SDL_Texture* texture {NULL};
	int anim {0};
	float scale {1.0};
};

#endif
