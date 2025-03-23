#include <SDL3/SDL.h>
#include "Sprite.hpp"
#include "Element.hpp"
#ifndef ANIMATION_HPP_ //Include guard
#define ANIMATION_HPP_

namespace anim {
	extern bool animInProgress;
	SDL_FRect applyScale(Sprite* spr);
	void animateShrink(Sprite* spr, double deltaTime);
	void animateGrow(Sprite* spr, double deltaTime);
}

#endif
