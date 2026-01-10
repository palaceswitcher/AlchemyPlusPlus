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
	DraggableElement(int elemId, int mX, int mY);
	int id;
	int z = 0; // Z-index
	bool queuedForDeletion = false;

	// Checks if two elements are colliding and combines them if possible
	void makeCombo();

	static float getWidth(int id);
	static float getHeight(int id);
};

namespace Board {
	extern std::unordered_map<int, SDL_Texture*> textureIndex;
	SDL_Texture* loadTexture(int id, float* width, float* height);
}

#endif
