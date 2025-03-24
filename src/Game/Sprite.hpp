#ifndef SPRITE_HPP_
#define SPRITE_HPP_
#include <SDL3/SDL.h>
#include "Animation.hpp"
#include <vector>

class Sprite {
protected:
	std::vector<Animation> animQueue;
public:
	float opacity = 1.0f;
	float scale = 1.0f;
	SDL_Texture* texture {NULL};
	SDL_FRect box;
	void addAnim(Animation anim);
	Sprite() {};
	Sprite(SDL_FRect rect, SDL_Texture* tex);

	void parseAnimations(double deltaTime);

	// Return true if no animations are queued
	bool animQueueEmpty();
};

#endif
