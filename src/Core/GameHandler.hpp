#ifndef GAMEHANDLER_HPP_
#define GAMEHANDLER_HPP_
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <utility>

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

// Returns the game's font
TTF_Font* getFont();

bool loadGameData(std::string id);
}

#endif
