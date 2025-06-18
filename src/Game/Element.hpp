#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_
#include "Sprite.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include "Animation.hpp"
#include "IO.hpp"

#define ELEM_SIZE 32

class DraggableElement : public Sprite {
public:
	DraggableElement() : Sprite() {};
	DraggableElement(SDL_Renderer* ren, int elemId, int mX, int mY);
	int id;
	int z = 0; // Z-index
	bool queuedForDeletion = false;

	void makeCombo(SDL_Renderer* ren);
};

namespace Board {
	extern std::unordered_map<int, SDL_Texture*> textureIndex;
	SDL_Texture* loadTexture(SDL_Renderer* ren, int id, float* width, float* height);
}

#endif
