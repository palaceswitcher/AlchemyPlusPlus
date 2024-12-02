#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <vector>
#include "../JSON/cJSON.h"
#ifndef GAMEHANDLER_HPP_ //Include guard
#define GAMEHANDLER_HPP_

namespace Game {
	std::string getFontDir();
	std::string getTextureDir();
	std::string getTextDir();
	cJSON* getComboData();
	bool loadGameData(std::string id);
}

#endif
