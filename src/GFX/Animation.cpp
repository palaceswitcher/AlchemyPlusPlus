#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include "Sprite.hpp"
#include "Element.hpp"
#include "SDL_FontCache.h"
#include "Animation.hpp"

namespace anim {
	bool animInProgress = false;
	SDL_Rect* applyScale(Sprite* spr) {
		SDL_Rect* centerScaledRect = (SDL_Rect*) malloc(sizeof(struct SDL_Rect));
		int scaledW = (spr->box->w * spr->scale);
		int scaledH = (spr->box->h * spr->scale);
		centerScaledRect->x = spr->box->x + ((spr->box->w - scaledW) / 2);
		centerScaledRect->y = spr->box->y + ((spr->box->h - scaledH) / 2); //Scale relative to its center
		centerScaledRect->w = scaledW;
		centerScaledRect->h = scaledH;
		return centerScaledRect;
	}
	// Performs the shrink animation for a sprite. Is run every frame
	void animateShrink(Sprite* spr, double deltaTime) {
		double scaleAmount = 4 * deltaTime / 1000; //Animation should take 250ms
		if (spr->scale > 0.01) {
			spr->scale -= scaleAmount;
		} else {
			spr->anim = ANIM_SHRINK_END; //Stop shrinking animation when element gets too small
		}
	}
	// Performs the grow animation for a sprite. Is run every frame
	void animateGrow(Sprite* spr, double deltaTime) {
		double scaleAmount = 4 * deltaTime / 1000; //Animation should take 200ms
		if (spr->scale < 1.0) {
			spr->scale += scaleAmount;
		} else {
			spr->anim = ANIM_GROW_END;
			spr->scale = 1.0; //End grow animation and set to default scale
		}
	}
}
