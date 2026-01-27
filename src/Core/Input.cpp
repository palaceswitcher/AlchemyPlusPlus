#include "Input.hpp"
#include "Board.hpp"
#include "GraphicsContext.hpp"
#include "GameHandler.hpp"
#include <cmath>

int pressedKeys = 0;
int releasedKeys = 0;
float mouseX = 0.0f;
float mouseY = 0.0f;

bool leftClickDown = false; // Left click state, used to track drag and drop
bool rightClickDown = false; // Middle click state
bool middleClickDown = false;
long leftClickTick = 0; // The tick when left was clicked, used to detect double clicks
SDL_FPoint mousePos; // Mouse position

bool keyPressed(int keys) {
	return pressedKeys & keys != 0;
}
bool keyReleased(int keys) {
	return releasedKeys & keys != 0;
}

float getMouseX() {
	return mouseX;
}
float getMouseY() {
	return mouseY;
}

long getLastLeftClickTick() {
	return leftClickTick;
}

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
				Board::deleteElem(d.get());
			};
		}
		break;
	}
	case SDL_EVENT_MOUSE_MOTION: {
		mousePos = {e.motion.x, e.motion.y}; // Get mouse position
		mouseX = e.motion.x;
		mouseY = e.motion.y;
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

		if (e.button.button == SDL_BUTTON_LEFT) {
			pressedKeys &= ~KEY_MOUSE_LEFT;
			releasedKeys |= KEY_MOUSE_LEFT;
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			pressedKeys &= ~KEY_MOUSE_RIGHT;
			releasedKeys |= KEY_MOUSE_RIGHT;
		} else if (e.button.button == SDL_BUTTON_MIDDLE) {
			pressedKeys &= ~KEY_MOUSE_MIDDLE;
			releasedKeys |= KEY_MOUSE_MIDDLE;
		}
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		if (!Board::isFocused()) {
			break;
		}
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
		if (e.button.button == SDL_BUTTON_LEFT) {
			releasedKeys &= ~KEY_MOUSE_LEFT;
			pressedKeys |= KEY_MOUSE_LEFT;
			leftClickTick = SDL_GetTicks(); // Get next click tick
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			releasedKeys &= ~KEY_MOUSE_RIGHT;
			pressedKeys |= KEY_MOUSE_RIGHT;
		} else if (e.button.button == SDL_BUTTON_MIDDLE) {
			releasedKeys &= ~KEY_MOUSE_MIDDLE;
			pressedKeys |= KEY_MOUSE_MIDDLE;
		}

		if (e.button.button == SDL_BUTTON_LEFT) {
			
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
