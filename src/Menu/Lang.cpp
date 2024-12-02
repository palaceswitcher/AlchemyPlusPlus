#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../Game/GameHandler.hpp"
#include "../Misc/IO.hpp"
#include "../JSON/cJSON.h"
#include "Lang.hpp"

std::string menuStrings[64]; //Strings for all menu text
std::unordered_map<std::string, std::string> elementStrings; //Strings for all element names
namespace Text {
	void loadAll(std::string language) {
		cJSON* comboJSONRoot = Game::getComboData();

		language = Game::getTextDir() + language + ".json";
		std::string textJSONStr = loadFile(language);
		cJSON* textJSONRoot = cJSON_Parse(textJSONStr.c_str());

		cJSON* temp = comboJSONRoot->child;
		while (temp != NULL) {
			std::string key = temp->string;
			std::string value;
			if (cJSON_GetObjectItem(textJSONRoot, key.c_str()) != NULL) {
				value = cJSON_GetObjectItem(textJSONRoot, key.c_str())->valuestring;
			}

			elementStrings.insert(std::make_pair(key, value)); //Add to element text cache

			temp = temp->next; //Go to next element
		}
	}
	std::string getElemName(std::string name) {
		return elementStrings[name];
	}
}
