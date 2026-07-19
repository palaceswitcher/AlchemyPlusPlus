#include "Input.hpp"
#include "Board.hpp"
#include "GraphicsContext.hpp"
#include <cmath>

int pressedKeys = 0;
int heldKeys = 0;
int releasedKeys = 0;
float mouseX = 0.0f;
float mouseY = 0.0f;
float mouseDeltaX = 0.0f; // Difference in mouse movement
float mouseDeltaY = 0.0f;

bool leftClickDown = false; // Left click state, used to track drag and drop
bool rightClickDown = false; // Middle click state
bool middleClickDown = false;

long leftClickTick = 0;
long rightClickTick = 0;
long middleClickTick = 0;

long prevLeftClickTick = 0;
long prevRightClickTick = 0;
long prevMiddleClickTick = 0;
SDL_FPoint mousePos; // Mouse position

bool keyPressed(int keys) {
	return (pressedKeys & keys) != 0;
}
bool keyHeld(int keys) {
	return (heldKeys & keys) != 0;
}
bool keyReleased(int keys) {
	return (releasedKeys & keys) != 0;
}

float getMouseX() {
	return mouseX;
}
float getMouseY() {
	return mouseY;
}

float getMouseDeltaX() {
	return mouseDeltaX;
}
float getMouseDeltaY() {
	return mouseDeltaY;
}

SDL_FPoint getMousePos() {
	return mousePos;
}

long getPrevLeftClickTick() {
	return prevLeftClickTick;
}

void flushInput() {
	pressedKeys = 0;
	prevLeftClickTick = leftClickTick;
	prevRightClickTick = rightClickTick;
	prevMiddleClickTick = middleClickTick; // There's probably a better way of doing this
	mouseDeltaX = 0.0f;
	mouseDeltaY = 0.0f;
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
		mouseDeltaX = e.motion.xrel;
		mouseDeltaY = e.motion.yrel;
		mousePos = {e.motion.x, e.motion.y}; // Get mouse position
		mouseX = e.motion.x;
		mouseY = e.motion.y;
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_UP: {
		if (e.button.button == SDL_BUTTON_LEFT) {
			heldKeys &= ~KEY_MOUSE_LEFT;
			releasedKeys |= KEY_MOUSE_LEFT;
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			heldKeys &= ~KEY_MOUSE_RIGHT;
			releasedKeys |= KEY_MOUSE_RIGHT;
		} else if (e.button.button == SDL_BUTTON_MIDDLE) {
			heldKeys &= ~KEY_MOUSE_MIDDLE;
			releasedKeys |= KEY_MOUSE_MIDDLE;
		}
		break;
	}
	case SDL_EVENT_MOUSE_BUTTON_DOWN: {
		if (e.button.button == SDL_BUTTON_LEFT) {
			pressedKeys |= KEY_MOUSE_LEFT;
			heldKeys |= KEY_MOUSE_LEFT;
			releasedKeys &= ~KEY_MOUSE_LEFT;
			leftClickTick = SDL_GetTicks();
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			pressedKeys |= KEY_MOUSE_RIGHT;
			heldKeys |= KEY_MOUSE_RIGHT;
			releasedKeys &= ~KEY_MOUSE_RIGHT;
			rightClickTick = SDL_GetTicks();
		} else if (e.button.button == SDL_BUTTON_MIDDLE) {
			pressedKeys |= KEY_MOUSE_MIDDLE;
			heldKeys |= KEY_MOUSE_MIDDLE;
			releasedKeys &= ~KEY_MOUSE_MIDDLE;
			middleClickTick = SDL_GetTicks();
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
