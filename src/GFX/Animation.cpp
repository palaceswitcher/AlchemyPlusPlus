#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <vector>
#include "../Game/Sprite.hpp"
#include "../Game/Element.hpp"
#include "SDL_FontCache.h"
#include "Animation.hpp"

namespace anim {
	bool animInProgress = false;
	void applyScale(Sprite* spr) {
		int origW = spr->box->w;
		int origH = spr->box->h; //Copy original size for later calculations
		spr->box->w = (ELEM_SIZE * spr->scale);
		spr->box->h = (ELEM_SIZE * spr->scale); //Scale width and height
		if (spr->box->h % 2 == 0 && spr->box->w % 2 == 0) {
			spr->box->x += (origH - spr->box->h);
			spr->box->y += (origW - spr->box->w);
		}
	}
	void adjustElementLabelPosition(DraggableElement* element, FC_Font* font) {
		float origW = FC_GetWidth(font, element->name.c_str());
		float origH = FC_GetHeight(font, element->name.c_str());
		if (element->box->h % 2 == 0 && element->box->w % 2 == 0) {
			element->fontX += (origW - element->box->w * element->scale);
			element->fontY += (origH - element->box->h * element->scale);
		}
	}
	// Performs the shrink animation for a sprite. Is run every frame
	void animateShrink(Sprite* spr, double deltaTime) {
		double scaleAmount = 4 * deltaTime / 1000; //Animation should take 250ms
		if (spr->scale > 0.01) {
			spr->scale -= scaleAmount;
			applyScale(spr);
		} else {
			spr->anim = ANIM_SHRINK_END; //Stop shrinking animation when element gets too small
		}
	}
	// Performs the grow animation for a sprite. Is run every frame
	void animateGrow(Sprite* spr, double deltaTime) {
		double scaleAmount = 4 * deltaTime / 1000; //Animation should take 200ms
		if (spr->scale < 1.0) {
			spr->scale += scaleAmount;
			applyScale(spr);
		} else {
			spr->anim = ANIM_GROW_END;
			spr->scale = 1.0; //End grow animation and set to default scale
			applyScale(spr);
		}
	}
}
