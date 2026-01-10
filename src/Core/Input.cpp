#include "Input.hpp"
#include "Board.hpp"
#include "GraphicsContext.hpp"
#include "GameHandler.hpp"
#include <cmath>

bool leftClickDown = false; // Left click state, used to track drag and drop
bool rightClickDown = false; // Middle click state
long leftClickTick = 0; // The tick when left was clicked, used to detect double clicks
SDL_FPoint mousePos; // Mouse position
SDL_Point clickOffset; // Point in the element box clicked relative to its boundary
void handleInput(const SDL_Event &e) {
	int winWidth = GFX::getWindowWidth();
	int winHeight = GFX::getWindowHeight();
	switch (e.type) {
	case SDL_EVENT_KEY_DOWN: {
		if (e.key.scancode == SDL_SCANCODE_F1) {
			for (auto& d : *(Board::getDraggableElems())) {
				d->scale = 1.0f;
			}
		} else if (e.key.scancode == SDL_SCANCODE_F2) {
			for (auto& d : *(Board::getDraggableElems())) {
				d->addAnim({ANIM_SCALE, 0.0f, 0.25f});
				Board::deleteElem(d.get());
			};
		}
		break;
	}
	case SDL_EVENT_MOUSE_MOTION: {
		mousePos = {e.motion.x, e.motion.y}; // Get mouse position
		if (leftClickDown && Board::elemSelected()) {
			Board::getSelectedElem()->box.x = roundf(mousePos.x - clickOffset.x);
			Board::getSelectedElem()->box.y = roundf(mousePos.y - clickOffset.y); // Update rectangle position while its being dragged
		}
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP: {
		if (!Board::isFocused()) {
			break;
		}
		if (leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
			leftClickDown = false;

			if (Board::elemSelected()) {
				Board::getSelectedElem()->z++; // Move behind
				Board::getSelectedElem()->makeCombo(); // See if combination was made with another element
				if (!Board::getSelectedElem()->queuedForDeletion) {
					if (Board::getSelectedElem()->box.x + Board::getSelectedElem()->box.w/2 >= GFX::getWindowWidth() ||
						Board::getSelectedElem()->box.y + Board::getSelectedElem()->box.h/2 >= GFX::getWindowHeight()) {
							Board::deleteSelectedElem(); // Delete element if it goes off-screen
						}
				}
			} else {
				// TODO: CODE THIS ELSEWHERE
				/*if (addButtonClicked)  {
					addButtonClicked = false;
					addButton.addAnim({ANIM_SCALE, 1.0f, 0.1875f});
					addButton.wasClicked = true;
				}*/
			}
			Board::deselectElem(); // Release selected rectangle when left is released
			Board::queueZSort();
		}
		// Remove if right clicked
		if (rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
			rightClickDown = false;
			if (Board::elemSelected()) {
				Board::getSelectedElem()->addAnim({ANIM_SCALE, 0.0f, 0.25f});
				Board::deleteSelectedElem();
				Board::deselectElem();
			}
		}
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		if (!Board::isFocused()) {
			break;
		}
		// Get element clicked regardless of mouse button
		std::vector<DraggableElement*> clickMatches; // Every element that the cursor clicked on
		if (!Board::elemSelected()) {
			for (auto& d : *(Board::getDraggableElems())) {
				if (!d->queuedForDeletion && SDL_PointInRectFloat(&mousePos, &d->box)) {
					clickMatches.push_back(d.get());
				}
			}
			if (!clickMatches.empty()) {
				Board::queueZSort(); // Z-index will need to be resorted as this element will be moved to the front
				std::stable_sort(clickMatches.begin(), clickMatches.end(), [](DraggableElement* d1, DraggableElement* d2) {
					return d1->z > d2->z;
				});

				Board::selectElem(clickMatches.back()); // Select rectangle with highest Z-index
				Board::getSelectedElem()->z = 0; // Move to front of z-index
			}
		}
		if (!leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
			leftClickDown = true;
			if (!Board::elemSelected()) {
				if (!clickMatches.empty()) {
					clickOffset.x = mousePos.x - Board::getSelectedElem()->box.x;
					clickOffset.y = mousePos.y - Board::getSelectedElem()->box.y; // Get clicked point in element box relative to its boundary
				} else {
					// Check if circle around the add button is clicked
					/*if (((mousePos.x-(addButton.box.x+32))*(mousePos.x-(addButton.box.x+32)) + (mousePos.y-(addButton.box.y+32))*(mousePos.y-(addButton.box.y+32))) < 32*32) {
						addButton.addAnim({ANIM_SCALE, 1.25f, 0.1875f});
						addButtonClicked = true;
					}*/
				}
			} else {
				clickOffset.x = mousePos.x - Board::getSelectedElem()->box.x;
				clickOffset.y = mousePos.y - Board::getSelectedElem()->box.y; // Don't look for a new element to select if one is already selected
			}
			// Spawn new elements on double click
			if (SDL_GetTicks() > leftClickTick && SDL_GetTicks() <= leftClickTick + 250) { // Double clicks have to be within 1/4 second of each other
				if (!Board::elemSelected()) {
					Board::spawnDraggable(mousePos.x, mousePos.y+40, Game::getElementNumId("air"));
					Board::spawnDraggable(mousePos.x, mousePos.y-40, Game::getElementNumId("earth"));
					Board::spawnDraggable(mousePos.x-40, mousePos.y, Game::getElementNumId("fire"));
					Board::spawnDraggable( mousePos.x+40, mousePos.y, Game::getElementNumId("water"));
				} else {
					Board::spawnDraggable(mousePos.x, mousePos.y, Board::getSelectedElem()->id); // Duplicate element if it's double clicked
					Board::deselectElem();
				}
			}
			leftClickTick = SDL_GetTicks(); // Get next click tick
		}

		if (!rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
			rightClickDown = true;
		}
		break;
	}
	// Respond to window resize
	case SDL_EVENT_WINDOW_RESIZED: {
		int prevWinWidth = winWidth;
		int prevWinHeight = winHeight;
		//addButton.box = {(float)winWidth/2-32, (float)winHeight-80, 64, 64}; // Position add button to the center of the screen
		// Adjust draggable elements relative to their previous position
		for (auto& d : *(Board::getDraggableElems())) {
			d->box.x = round(d->box.x * (double)winWidth/prevWinWidth);
			d->box.y = round(d->box.y * (double)winHeight/prevWinHeight);
		}
	}
	}
}
