#include "Sprite.hpp"
#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <iostream>

void Sprite::parseAnimations(double deltaTime) {
	deltaTime /= 1000;
	if (!animQueue.empty()) {
		Animation anim = animQueue.back();
		if ((anim.animFlags & ANIM_SCALE) != 0) {
			if (scale < anim.end) {
				scale += deltaTime/anim.duration;
				if (scale > anim.end) {
					scale = anim.end;
					anim.animFlags ^= ANIM_SCALE; //Clear bit if animation finishes
				}
			} else {
				scale -= deltaTime/anim.duration;
				if (scale < anim.end) {
					scale = anim.end;
					anim.animFlags ^= ANIM_SCALE;
				}
			}
		}
		if ((anim.animFlags & ANIM_OPACITY) != 0) {
			if (opacity < anim.end) {
				opacity += deltaTime/anim.duration;
				if (opacity > anim.end) {
					opacity = anim.end;
					anim.animFlags ^= ANIM_OPACITY;
				}
			} else {
				opacity -= deltaTime/anim.duration;
				if (opacity < anim.end) {
					opacity = anim.end;
					anim.animFlags ^= ANIM_OPACITY;
				}
			}
		}
		if (anim.animFlags == 0) {
			animQueue.pop_back(); //Remove animation from the queue if all properties have been processed and the animation is done
		}
	}
}

void Sprite::addAnim(Animation anim) {
	animQueue.push_back(anim);
}

Sprite::Sprite(SDL_FRect rect, SDL_Texture* tex) {
	box = rect;
	texture = tex;
}

bool Sprite::animQueueEmpty() {
	return animQueue.empty();
}

