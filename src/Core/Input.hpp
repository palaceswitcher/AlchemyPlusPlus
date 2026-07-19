#ifndef INPUT_HPP_
#define INPUT_HPP_
#include <SDL3/SDL.h>

enum InputFlags {
	KEY_MOUSE_LEFT = 1 << 0,
	KEY_MOUSE_RIGHT = 1 << 1,
	KEY_MOUSE_MIDDLE = 1 << 2
};

// Returns currently pressed keys
bool keyPressed(int keys);
// Returns currently held keys
bool keyHeld(int keys);
// Returns keys that were just released
bool keyReleased(int keys);

// Returns the tick of the last left click
long getPrevLeftClickTick();

// Return X position of mouse
float getMouseX();
// Return Y position of mouse
float getMouseY();

float getMouseDeltaX();
float getMouseDeltaY();

SDL_FPoint getMousePos();

void flushInput();

// Parses input
void handleInput(const SDL_Event &event);

#endif
