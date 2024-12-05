#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <vector>
#include "cJSON.h"
#ifndef GAMEHANDLER_HPP_ //Include guard
#define GAMEHANDLER_HPP_

extern SDL_Renderer* renderer;
extern SDL_Window* window;

namespace Game {
	std::string getFontDir();
	std::string getTextureDir();
	std::string getTextDir();
	cJSON* getComboData();
	bool loadGameData(std::string id);
}

#endif
