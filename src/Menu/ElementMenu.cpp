#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../JSON/cJSON.h"
#include "ElementMenu.hpp"

bool elementMenuOpen = false;

void openElementMenu(std::vector<std::string> elementsUnlocked) {

}

void closeElementMenu() {
	elementMenuOpen = false;
}