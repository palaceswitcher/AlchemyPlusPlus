#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_
#include <stdbool.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "Sprite.hpp"
#include "IO.hpp"

#define ELEM_SIZE 32

class DraggableElement : public Sprite {
public:
	DraggableElement() : Sprite({{}, nullptr, ANIM_NONE, 1.0f}) {};
	DraggableElement(SDL_Renderer* ren, int elemId, int mX, int mY);
	int id;
	int z = 0; //Z-index
	bool queuedForDeletion = false;

	void makeCombo(SDL_Renderer* ren, std::vector<std::string> &elementsUnlocked);
};

namespace Board {
	extern std::unordered_map<int, SDL_Texture*> textureIndex;
	SDL_Texture* loadTexture(SDL_Renderer* ren, int id, float* width, float* height);
}

#endif
