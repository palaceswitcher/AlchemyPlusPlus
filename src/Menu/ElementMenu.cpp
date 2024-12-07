#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "cJSON.h"
#include "Lang.hpp"
#include "GameHandler.hpp"
#include "ElementMenu.hpp"


// Get every unlocked element that matches the query
std::vector<std::string> getElemSearchResults(std::string query, std::vector<std::string> elementsUnlocked) {
	std::vector<std::string> matchingElementIDs; //String IDs of every element that matches the query
	std::copy_if(elementsUnlocked.begin(), elementsUnlocked.end(), std::back_inserter(matchingElementIDs), [query]
	(std::string const& s) {
		std::string name = Text::getElemName(s);
		return name.size() > 0 && name.find(query) != std::string::npos;
	});
	if (matchingElementIDs.empty()) { matchingElementIDs.push_back(""); }
	return matchingElementIDs;
}
