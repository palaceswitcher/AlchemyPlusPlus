#ifndef ELEMENT_HPP_
#define ELEMENT_HPP_
#include <stdbool.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "cJSON.h"
#include "SDL_FontCache.h"
#include "Sprite.hpp"
#include "IO.hpp"

#define ELEM_SIZE 32

class DraggableElement : public Sprite {
public:
	DraggableElement() : Sprite({NULL, NULL, ANIM_NONE, 1.0}) {};
	DraggableElement(std::string elemID, int mX, int mY);
	~DraggableElement();
	std::string id;
	std::string name; //Displayed name of the element
	int z = 0; //Z-index
	bool queuedForDeletion = false;

	void makeCombo(std::vector<std::string> &elementsUnlocked);
	static void deleteElem(std::vector<std::unique_ptr<DraggableElement>> &draggables, DraggableElement* elem);
};

namespace Board {
	extern std::unordered_map<std::string, SDL_Texture*> textureIndex;
	SDL_Texture* loadTexture(std::string id, int* width, int* height);
}

#endif
