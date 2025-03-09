#include <SDL2/SDL.h>
#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include "Element.hpp"
#include "Sprite.hpp"
#include "GameHandler.hpp"
#include "Animation.hpp"
#include "SDL_FontCache.h"
#include "cJSON.h"
#include "IO.hpp"
#include "Lang.hpp"
#include "Board.hpp"

// Constructor
DraggableElement::DraggableElement(std::string elemID, int mX, int mY) {
	SDL_Rect* newElemBox = (SDL_Rect*) malloc(sizeof(struct SDL_Rect)); //Allocate space for new element's rectangle

	box = newElemBox; //Add box to new element
	newElemBox->x = mX;
	newElemBox->y = mY; //Position element's rectangle

	z = 1;
	id = elemID; //Get numerical ID

	scale = 0.0;
	anim = ANIM_GROW; //Make element grow on spawn
	name = Text::getElemName(elemID);

	texture = Board::loadTexture(id, &(newElemBox->w), &(newElemBox->h)); //Get texture

	anim::animInProgress = true; //Start animations
}

DraggableElement::~DraggableElement() {
	free(this->box);
}

// Checks if two elements are colliding and combines them if possible
void DraggableElement::makeCombo(std::vector<std::string> &elementsUnlocked) {
	std::vector<std::unique_ptr<DraggableElement>>* draggableElems = Board::getDraggableElems();
	DraggableElement* matchingElem; //The element successfully merged with
	bool comboMade = false; //This indicates if a valid combination was made and allows the function to unlock that element

	cJSON* elemJSON = cJSON_GetObjectItem(Game::getComboData(), this->id.c_str());
	cJSON* elemCombos = cJSON_GetObjectItem(elemJSON, "combos"); //Get combinations for first selected element
	if (elemCombos == NULL) { return; }
	int startY = this->box->y;

	std::vector<DraggableElement*> collidedElements; //A list of every element this could match with
	for (auto &d : *draggableElems) {
		if (d.get() != this && SDL_HasIntersection(this->box, d->box) == SDL_TRUE) {
			comboMade = (cJSON_GetObjectItem(elemCombos, d->id.c_str()) != NULL); //Check if combination is valid (TODO REMOVE OR FIX ID SYSTEM, IT'S CURRENTLY A WASTE)
			if (comboMade) { collidedElements.push_back(d.get()); }
		}
	}
	if (!collidedElements.empty()) {
		std::stable_sort(collidedElements.begin(), collidedElements.end(), [this]
		(DraggableElement* d1, DraggableElement* d2) {
			SDL_Rect* d1Intersect; SDL_IntersectRect(this->box, d1->box, d1Intersect);
			SDL_Rect* d2Intersect; SDL_IntersectRect(this->box, d2->box, d2Intersect); //The intersecting rectangles between the combined elements. The combination with the largest intersection area will be the combined element
			return d1->z > d2->z && (d1Intersect->x * d1Intersect->y) > (d2Intersect->x * d2Intersect->y);
		});
		matchingElem = collidedElements.back(); //Match with the element with the lowest Z-index
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
				if (std::find(elementsUnlocked.begin(), elementsUnlocked.end(), newElemID) == elementsUnlocked.end()) {
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
			Board::spawnDraggable(newX, newY, newElemNames[i]); //Add elements to screen
		}

		// Queue combined elements to start despawning
		this->anim = ANIM_SHRINK;
		matchingElem->anim = ANIM_SHRINK;
		anim::animInProgress = true; //Start animations
	}
}

void DraggableElement::deleteElem(std::vector<std::unique_ptr<DraggableElement>> &draggableElems, DraggableElement* elem) {
	draggableElems.erase(std::remove_if(draggableElems.begin(), draggableElems.end(), [&](std::unique_ptr<DraggableElement> &e) {
		return (e.get() == elem);
	}));
}

namespace Board {
std::unordered_map<std::string, SDL_Texture*> textureIndex;

// Loads an element texture from the texture index and optionally returns the width and height
SDL_Texture* loadTexture(std::string id, int* width, int* height) {
	SDL_Texture* newTexture = NULL;
	if (textureIndex.find(id) == textureIndex.end()) {
		std::string textureName = Game::getTextureDir() + "elems/" + id + ".png"; //Get filename for texture
		newTexture = IMG_LoadTexture(renderer, textureName.c_str());
		if (newTexture == NULL) {
			std::string dummyTexDir = Game::getTextureDir() + "elems/_dummy.png";
			newTexture = IMG_LoadTexture(renderer, dummyTexDir.c_str());
		}
		textureIndex.insert(std::make_pair(id, newTexture)); //Add texture to index if it isn't already there
	} else {
		newTexture = textureIndex[id]; //If the texture is already in the index, load the texture from there
	}
	SDL_QueryTexture(textureIndex[id], NULL, NULL, width, height); //Get width from textures
	return newTexture;
}
}
