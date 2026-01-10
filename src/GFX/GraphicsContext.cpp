#include "GraphicsContext.hpp"

SDL_Renderer* GFX::renderer;
SDL_Window* GFX::window;

int GFX::getWindowWidth() {
	int width = 0;
	SDL_GetWindowSize(window, &width, NULL);
	return width;
}
int GFX::getWindowHeight() {
	int height = 0;
	SDL_GetWindowSize(window, NULL, &height);
	return height;
}
