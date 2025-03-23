#ifndef SPRITE_HPP_ //Include guard
#define SPRITE_HPP_
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <vector>

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
	SDL_FRect box;
	SDL_Texture* texture {NULL};
	int anim = 0;
	float scale = 1.0f;
};

#endif
