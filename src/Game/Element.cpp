#include <SDL2/SDL.h>
#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <cmath>
#include "Element.hpp"
#include "Sprite.hpp"
#include "../GFX/Animation.hpp"
#include "../GFX/SDL_FontCache.h"
#include "../JSON/cJSON.h"
#include "../Misc/IO.hpp"
#include "../Menu/Lang.hpp"

bool compareZIndexRaw(DraggableElement* d1, DraggableElement* d2) { return d1->z > d2->z; } //Z-index comparison for raw pointers

// Constructor
DraggableElement::DraggableElement(std::string elemID, int mX, int mY) {
	SDL_Rect* newElemBox = (SDL_Rect*) malloc(sizeof(struct SDL_Rect)); //Allocate space for new element's rectangle

	box = newElemBox; //Add box to new element
	newElemBox->w = ELEM_SIZE;
	newElemBox->h = ELEM_SIZE;
	newElemBox->x = mX;
	newElemBox->y = mY; //Position element's rectangle

	z = 1;
	id = elemID; //Get numerical ID

	scale = 0.0;
	anim = ANIM_GROW; //Make element grow on spawn
	name = Text::getElemName(elemID);

	anim::animInProgress = true; //Start animations
}

DraggableElement::~DraggableElement() {
	free(this->box);
}

// Checks if two elements are colliding and combines them if possible
void DraggableElement::makeCombo(std::vector<std::unique_ptr<DraggableElement>> &draggables, std::vector<std::string> &elementsUnlocked) {
	elem::firstParentElem = this;
	// Place newly unlocked elements on screen
	DraggableElement* matchingElem;
	bool comboMade = false; //This indicates if a valid combination was made and allows the function to unlock that element

	cJSON* elemJSON = cJSON_GetObjectItem(elem::root, this->id.c_str());
	cJSON* elemCombos = cJSON_GetObjectItem(elemJSON, "combos"); //Get combinations for first selected element
	if (elemCombos == NULL) { return; }
	int startY = this->box->y;

	std::vector<DraggableElement*> collidedElements; //A list of every element this could match with
	for (auto &d : draggables) {
		if (d.get() != this && SDL_HasIntersection(this->box, d->box) == SDL_TRUE) {
			comboMade = (cJSON_GetObjectItem(elemCombos, d->id.c_str()) != NULL); //Check if combination is valid (TODO REMOVE OR FIX ID SYSTEM, IT'S CURRENTLY A WASTE)
			if (comboMade) { collidedElements.push_back(d.get()); }
		}
	}
	if (!collidedElements.empty()) {
		std::stable_sort(collidedElements.begin(), collidedElements.end(), compareZIndexRaw);
		matchingElem = collidedElements.back(); //Match with the element with the lowest Z-index
		elem::secondParentElem = collidedElements.back(); //Match with the element with the lowest Z-index
	}

	if (comboMade) {
		cJSON* results = cJSON_GetObjectItem(elemCombos, matchingElem->id.c_str());
		// Add result elements to unlocked elements
		std::vector<std::string> newElemNames; //Numerical IDs of resultant elements
		int numResults = 0; //Count the number of results for later
		cJSON* iterator = NULL; //Initialize iterator object
		cJSON_ArrayForEach(iterator, results) {
			numResults++;
			if (cJSON_IsString(iterator)) {
				std::string newElemID = iterator->valuestring; //Get numerical ID for new element
				std::cout << "Resultant Element: " << iterator->valuestring << std::endl;
				if (std::find(elementsUnlocked.begin(), elementsUnlocked.end(), newElemID) != elementsUnlocked.end()) {
					elementsUnlocked.push_back(newElemID); //Add resulting element to list of unlocked elements if it's new
				}
				newElemNames.push_back(iterator->valuestring); //Add to list of new elements
			}
		}

		//Position new elements on-screen
		for (int i = 0; i < numResults; i++) {
			int lowerElemXPos = this->box->x;
			int lowerElemYPos = this->box->y;
			if (this->box->x > matchingElem->box->x) lowerElemXPos = matchingElem->box->x;
			if (this->box->y > matchingElem->box->y) lowerElemYPos = matchingElem->box->y; //Offset by the elements with the lower X and Y positions
			int newX = lowerElemXPos + ((i+1) * (this->box->x - matchingElem->box->x) / (numResults+1));
			int newY = lowerElemYPos + ((i+1) * (this->box->y - matchingElem->box->y) / (numResults+1)); //Position new elements between new elements
			elem::spawnDraggable(draggables, newX, newY, newElemNames[i]); //Add elements to screen
		}

		// Queue combined elements to start despawning
		elem::firstParentElem->anim = ANIM_SHRINK;
		elem::secondParentElem->anim = ANIM_SHRINK;
		anim::animInProgress = true; //Start animations
	}
}

void DraggableElement::deleteElem(std::vector<std::unique_ptr<DraggableElement>> &draggables, DraggableElement* elem) {
	draggables.erase(std::remove_if(draggables.begin(), draggables.end(), [&](std::unique_ptr<DraggableElement> &e) {
		return (e.get() == elem);
	}));
}

namespace elem {
	std::unordered_map<std::string, SDL_Texture*> textureIndex;
	DraggableElement* firstParentElem;
	DraggableElement* secondParentElem; //First and second selected elements
	cJSON* root = NULL; //Root of combination JSON data

	void loadTexture(SDL_Renderer* ren, DraggableElement* elem) {
		if (textureIndex.find(elem->id) == textureIndex.end()) {
			std::string textureName = std::string("gamedata/default/textures/elems/") + elem->id + ".png"; //Get filename for texture
			SDL_Texture* newTexture = IMG_LoadTexture(ren, textureName.c_str());
			if (newTexture == NULL) { newTexture = IMG_LoadTexture(ren, "gamedata/default/textures/elems/_dummy.png"); }
			textureIndex.insert(std::make_pair(elem->id, newTexture)); //Add texture to index if it isn't already there
			elem->texture = newTexture; //Give element new texture
		} else {
			elem->texture = textureIndex[elem->id]; //If the texture is already in the index, load the texture from there
		}
	}
	void spawnDraggable(std::vector<std::unique_ptr<DraggableElement>> &draggables, int x, int y, std::string name) {
		draggables.push_back(std::make_unique<DraggableElement>(name, x, y)); //Add to list of elements
	}
	void JSONInit() {
		// Load JSON file
		char* comboJSONStr = loadFile("gamedata/default/combos.json");
		root = cJSON_Parse(comboJSONStr);
		free(comboJSONStr); //Save memory
	}
}
