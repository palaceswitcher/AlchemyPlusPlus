#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
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
#include "IO.hpp"
#include "Board.hpp"

// Constructor
DraggableElement::DraggableElement(int elemId, int mX, int mY) {
	SDL_Rect newElemBox;

	int w, h;
	texture = Board::loadTexture(elemId, &w, &h); //Get texture

	box = {mX, mY, w, h}; //Add box to new element
	newElemBox.x = mX;
	newElemBox.y = mY; //Position element's rectangle

	z = 1;
	id = elemId;

	scale = 0.0;
	anim = ANIM_GROW; //Make element grow on spawn

	anim::animInProgress = true; //Start animations
}

// Checks if two elements are colliding and combines them if possible
void DraggableElement::makeCombo(std::vector<std::string> &elementsUnlocked) {
	std::vector<std::unique_ptr<DraggableElement>>* draggableElems = Board::getDraggableElems();

	auto comboData = Game::getComboData(this->id); //Get combinations for first selected element
	//if (comboData->empty()) { return; }

	std::vector<DraggableElement*> collidedElements; //A list of every element this could match with
	std::vector<int> collidedElemIds; //A list of every element this could match with
	for (auto &d : *draggableElems) {
		if (d.get() != this && SDL_HasIntersection(&this->box, &d->box) == SDL_TRUE) {
			collidedElements.push_back(d.get());
			collidedElemIds.push_back(d->id);
		}
	}

	if (!collidedElemIds.empty()) {
		std::sort(collidedElemIds.begin(), collidedElemIds.end(), []
		(int d1, int d2) {
			return d1 < d2;
		});
		if (collidedElemIds.size() > 10) {
			collidedElemIds = {collidedElemIds.begin(), collidedElemIds.begin()+10};
		}
		/*std::sort(collidedElements.begin(), collidedElements.end(), []
		(DraggableElement* d1, DraggableElement* d2) {
			return d1->id < d2->id;
		})*/;

		std::vector<int> resultElems;
		std::vector<DraggableElement*> matchingElems;
		for (auto& combo : *comboData) {
			if (std::includes(collidedElemIds.begin(), collidedElemIds.begin()+combo.first.size(), //Only check for elements with the highest z-index
							combo.first.begin(), combo.first.end())) {
				resultElems = combo.second;
				//int ind = std::distance(collidedElemIds.begin(),
				//		std::find(collidedElemIds.begin(), collidedElemIds.end(), combo.first[0])); //Find starting point of 
				matchingElems = {collidedElements.begin(), collidedElements.begin()+combo.first.size()};
				break;
			}
		}

		// Position new elements on-screen
		if (!resultElems.empty()) {
			std::vector<int> matchingElemXs = {this->box.x};
			std::vector<int> matchingElemYs = {this->box.y};
			this->anim = ANIM_SHRINK; //Begin shrinking this element to despawn it
			for (auto result : matchingElems) {
				result->anim = ANIM_SHRINK; //Shrink all matching elements involved in the combination until they despawn
				matchingElemXs.push_back(result->box.x);
				matchingElemYs.push_back(result->box.y);
			}
			anim::animInProgress = true; //Start animations

			int minX = *std::min_element(matchingElemXs.begin(), matchingElemXs.end());
			int minY = *std::min_element(matchingElemYs.begin(), matchingElemYs.end());
			int maxX = *std::max_element(matchingElemXs.begin(), matchingElemXs.end());
			int maxY = *std::max_element(matchingElemYs.begin(), matchingElemYs.end());
			for (int i = 0; i < resultElems.size(); i++) {
				int newX = minX + ((i+1) * (maxX-minX) / (resultElems.size()+1)); //TODO POSITION THEM SO THEY CANNOT OVERLAP
				int newY = minY + ((i+1) * (maxY-minY) / (resultElems.size()+1)); //Position new elements between the combined elements
				Board::spawnDraggable(newX, newY, resultElems[i]); //Add elements to screen
			}
		}
	}
}

namespace Board {
std::unordered_map<int, SDL_Texture*> textureIndex;

// Loads an element texture from the texture index and optionally returns the width and height
SDL_Texture* loadTexture(int id, int* width, int* height) {
	SDL_Texture* newTexture = NULL;
	if (!textureIndex.contains(id)) {
		std::string textureName = Game::getTextureDir() + "elems/" + Game::getElementStrId(id) + ".png"; //Get filename for texture
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
