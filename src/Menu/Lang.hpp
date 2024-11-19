#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "../GFX/SDL_FontCache.h"
#ifndef LANG_HPP_ //Include guard
#define LANG_HPP_

namespace Text {
	void loadAll(std::string language);
	std::string getElemName(std::string name);
}

#endif
