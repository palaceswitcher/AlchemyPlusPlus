#include <SDL3/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include "Sprite.hpp"
#include "Element.hpp"
#include "Animation.hpp"

SDL_FRect Anim::applyScale(SDL_FRect box, float scale) {
	if (scale == 1.0) {
		return box;
	}
	SDL_FRect centerScaledRect;
	int scaledW = (box.w * scale);
	int scaledH = (box.h * scale);
	centerScaledRect.x = box.x + ((box.w - scaledW) / 2);
	centerScaledRect.y = box.y + ((box.h - scaledH) / 2); //Scale relative to its center
	centerScaledRect.w = scaledW;
	centerScaledRect.h = scaledH;
	return centerScaledRect;
}
