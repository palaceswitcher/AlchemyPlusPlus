#include <stdlib.h>
#include <string>
#include <vector>
#include "GameHandler.hpp"
#ifndef ELEMENTMENU_HPP_ //Include guard
#define ELEMENTMENU_HPP_

// Get every unlocked element that matches the query
std::vector<std::string> getElemSearchResults(std::string query, std::vector<std::string> elementsUnlocked);

#endif
