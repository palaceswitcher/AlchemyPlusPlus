#ifndef GAMEHANDLER_HPP_
#define GAMEHANDLER_HPP_
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <utility>

extern SDL_Renderer* renderer;
extern SDL_Window* window;

struct ComboData {
	std::vector<std::string> elems;
	std::vector<std::string> results;
};

namespace Game {
std::string getFontDir();
std::string getTextureDir();
std::string getTextDir();

// Returns combination data for an element
std::vector<std::pair<std::vector<int>, std::vector<int>>>* getComboData(int id);

// Returns the name of element
std::string getElementStrId(int id);
std::string getElementName(int id);
std::string getElementName(std::string id);

// Returns the names of every element
std::vector<std::string>* getElementNames();

// Returns the string ID of an element
int getElementNumId(std::string id);

bool loadGameData(std::string id);
}

#endif
