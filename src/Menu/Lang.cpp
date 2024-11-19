#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../Misc/IO.hpp"
#include "../JSON/cJSON.h"
#include "Lang.hpp"

std::string menuStrings[64]; //Strings for all menu text
std::unordered_map<std::string, std::string> elementStrings; //Strings for all element names
namespace Text {
	void loadAll(std::string language) {
		char* comboJSONString = loadFile("combos.json");
		cJSON* comboJSONRoot = cJSON_Parse(comboJSONString);
		free(comboJSONString); //Load combination data

		language += ".json";
		char* textJSONString = loadFile(language.c_str());
		cJSON* textJSONRoot = cJSON_Parse(textJSONString);

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
