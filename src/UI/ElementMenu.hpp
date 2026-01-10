#ifndef ELEMENTMENU_HPP_
#define ELEMENTMENU_HPP_
#include <SDL3/SDL.h>
#include "GameHandler.hpp"
#include <string>
#include <vector>

// Get every unlocked element that matches the query
std::vector<int> getElemSearchResults(std::string query);
namespace UI {
void openElementMenu();
void renderElemMenu();
}

#endif
