#include <stdbool.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include "../JSON/cJSON.h"
#include "../GFX/SDL_FontCache.h"
#include "Sprite.hpp"
#include "../Misc/IO.hpp"
#ifndef ELEMENT_HPP_ //Include guard
#define ELEMENT_HPP_

#define ELEM_SIZE 32

class DraggableElement : public Sprite {
public:
	DraggableElement() : Sprite({NULL, NULL, 0, 1.0}) {};
	DraggableElement(std::string elemID, int mX, int mY);
	~DraggableElement();
	std::string id;
	std::string name; //Displayed name of the element
	int z = 0; //Z-index
	bool queuedForDeletion = false;

	void makeCombo(std::vector<std::unique_ptr<DraggableElement>> &draggables, std::vector<std::string> &elementsUnlocked);
	static void deleteElem(std::vector<std::unique_ptr<DraggableElement>> &draggables, DraggableElement* elem);
};

namespace elem {
	extern std::unordered_map<std::string, SDL_Texture*> textureIndex;
	extern DraggableElement* secondParentElem; //First and second selected elements
	void loadTexture(SDL_Renderer* ren, DraggableElement* elem);
	void spawnDraggable(std::vector<std::unique_ptr<DraggableElement>> &draggables, int x, int y, std::string name);
}

#endif
