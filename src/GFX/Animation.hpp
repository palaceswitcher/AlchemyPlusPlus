#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_
#include <SDL3/SDL.h>
#include "Sprite.hpp"

enum AnimationType {
	ANIM_NONE,
	ANIM_SCALE = 1,
	ANIM_OPACITY = 1 << 1
};

struct Animation {
	int animFlags; // Properties being modified (bitmask)
	float end; // Animation end value
	float duration = 1.0f; // The amount of time to complete the animation in seconds
};

namespace Anim {
SDL_FRect applyScale(SDL_FRect box, float scale);
}

#endif
