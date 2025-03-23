#ifndef ELEMENTMENU_HPP_
#define ELEMENTMENU_HPP_
#include <SDL3/SDL.h>
#include "GameHandler.hpp"
#include <string>
#include <vector>

// Get every unlocked element that matches the query
std::vector<std::string> getElemSearchResults(std::string query, std::vector<std::string> elementsUnlocked);
namespace UI {
void openElementMenu();
void renderElemMenu(SDL_Renderer* ren, std::vector<std::string> elementsUnlocked);
}

#endif
