#include <SDL2/SDL.h>
#include <stdbool.h>
#include "SDL_FontCache.h"
#include "Sprite.hpp"
#include "Element.hpp"
#ifndef ANIMATION_HPP_ //Include guard
#define ANIMATION_HPP_

namespace anim {
	extern bool animInProgress;
	SDL_Rect* applyScale(Sprite* spr);
	void animateShrink(Sprite* spr, double deltaTime);
	void animateGrow(Sprite* spr, double deltaTime);
}

#endif
