#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL_FontCache.h"
#include "../Game/Sprite.hpp"
#include "../Game/Element.hpp"
#ifndef ANIMATION_HPP_ //Include guard
#define ANIMATION_HPP_

enum ObjectAnimationType {
	ANIM_NONE,
	ANIM_SHRINK,
	ANIM_SHRINK_END,
	ANIM_GROW,
	ANIM_GROW_END,
	ANIM_FADE_IN,
	ANIM_FADE_IN_END
};

namespace anim {
	extern bool animInProgress;
	SDL_Rect* applyScale(Sprite* spr);
	void animateShrink(Sprite* spr, double deltaTime);
	void animateGrow(Sprite* spr, double deltaTime);
}

#endif
