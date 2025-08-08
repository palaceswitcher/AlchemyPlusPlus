#include "Rendering.hpp"
#include "Sprite.hpp"
#include <cmath>
#include <SDL3/SDL.h>

void GFX::renderSprite(SDL_Renderer* ren, double deltaTime, Sprite* spr) {
	spr->parseAnimations(deltaTime);
	SDL_SetTextureAlphaModFloat(spr->texture, spr->opacity);
	SDL_FRect scaledRect = spr->box; // Scaled rectangle

	if (spr->scale != 1.0) {
		int scaledW = (scaledRect.w * spr->scale);
		int scaledH = (scaledRect.h * spr->scale);
		scaledRect.x = roundf(scaledRect.x + ((scaledRect.w - scaledW) / 2));
		scaledRect.y = roundf(scaledRect.y + ((scaledRect.h - scaledH) / 2)); // Scale relative to its center
		scaledRect.w = scaledW;
		scaledRect.h = scaledH;
	}

	SDL_RenderTexture(ren, spr->texture, nullptr, &scaledRect);
}
