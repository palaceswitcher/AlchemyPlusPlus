#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Sprite.hpp"
#include "Element.hpp"
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include "Progress.hpp"
#include "Text.hpp"
#include "GameHandler.hpp"
#include "Animation.hpp"
#include "IO.hpp"
#include "Board.hpp"

// Constructor
DraggableElement::DraggableElement(SDL_Renderer* ren, int elemId, int mX, int mY) {
	SDL_Rect newElemBox;
	float w, h;
	texture = Board::loadTexture(ren, elemId, &w, &h); // Get texture

	box = {(float)mX, (float)mY, w, h}; // Add box to new element
	newElemBox.x = mX;
	newElemBox.y = mY; // Position element's rectangle

	z = 1;
	id = elemId;

	scale = 0.0f;
	animQueue.push_back({ANIM_SCALE, 1.0f, 0.25f}); // Set up animation
}

// Checks if two elements are colliding and combines them if possible
void DraggableElement::makeCombo(SDL_Renderer* ren) {
	std::vector<std::unique_ptr<DraggableElement>>* draggableElems = Board::getDraggableElems();

	auto comboData = Game::getComboData(this->id); // Get combinations for first selected element
	// if (comboData->empty()) { return; }

	std::vector<DraggableElement*> collidedElements; // A list of every element this could match with
	std::vector<int> collidedElemIds; // A list of every element this could match with
	for (auto &d : *draggableElems) {
		if (d.get() != this && SDL_HasRectIntersectionFloat(&this->box, &d->box) == true) {
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
			if (std::includes(collidedElemIds.begin(), collidedElemIds.begin()+combo.first.size(), // Only check for elements with the highest z-index
							combo.first.begin(), combo.first.end())) {
				resultElems = combo.second;
				// int ind = std::distance(collidedElemIds.begin(),
				// 		std::find(collidedElemIds.begin(), collidedElemIds.end(), combo.first[0])); // Find starting point of 
				matchingElems = {collidedElements.begin(), collidedElements.begin()+combo.first.size()};
				break;
			}
		}

		// Position new elements on-screen
		if (!resultElems.empty()) {
			std::vector<int> matchingElemXs = {(int)this->box.x};
			std::vector<int> matchingElemYs = {(int)this->box.y};
			this->addAnim({ANIM_SCALE, 0.0f, 0.25f}); // Begin shrinking this element to despawn it
			this->queuedForDeletion = true;
			for (auto result : matchingElems) {
				result->addAnim({ANIM_SCALE, 0.0f, 0.25f}); // Shrink all matching elements involved in the combination until they despawn
				result->queuedForDeletion = true;
				matchingElemXs.push_back(result->box.x);
				matchingElemYs.push_back(result->box.y);
			}

			int minX = *std::min_element(matchingElemXs.begin(), matchingElemXs.end());
			int minY = *std::min_element(matchingElemYs.begin(), matchingElemYs.end());
			int maxX = *std::max_element(matchingElemXs.begin(), matchingElemXs.end());
			int maxY = *std::max_element(matchingElemYs.begin(), matchingElemYs.end());
			for (int i = 0; i < resultElems.size(); i++) {
				int newX = minX + ((i+1) * (maxX-minX) / (resultElems.size()+1)); // TODO POSITION THEM SO THEY CANNOT OVERLAP
				int newY = minY + ((i+1) * (maxY-minY) / (resultElems.size()+1)); // Position new elements between the combined elements
				Board::spawnDraggable(ren, newX, newY, resultElems[i]); // Add elements to screen
				Progress::UnlockElement(resultElems[i]);
			}
		}
	}
}

namespace Board {
std::unordered_map<int, SDL_Texture*> textureIndex;

// Loads an element texture from the texture index and optionally returns the width and height
SDL_Texture* loadTexture(SDL_Renderer* ren, int id, float* width, float* height) {
	SDL_Texture* newTexture = NULL;
	if (!textureIndex.contains(id)) {
		std::string textureName = Game::getTextureDir() + "elems/" + Game::getElementStrId(id) + ".png"; // Get filename for texture
		newTexture = IMG_LoadTexture(ren, textureName.c_str());
		if (newTexture == NULL) {
			std::string dummyTexDir = Game::getTextureDir() + "elems/_dummy.png";
			newTexture = IMG_LoadTexture(ren, dummyTexDir.c_str());
		}
		textureIndex.insert({id, newTexture}); // Add texture to index if it isn't already there
	} else {
		newTexture = textureIndex[id]; // If the texture is already in the index, load the texture from there
	}
	SDL_GetTextureSize(textureIndex[id], width, height); // Get width from textures
	return newTexture;
}
}
