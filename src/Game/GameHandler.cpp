#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "cJSON.h"
#include "IO.hpp"
#include "GameHandler.hpp"

// Global SDL variables
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;

// Directories
const std::string GAME_DATA_DIR = "gamedata/";
std::string gameID; //ID of the current name, used to determine its directory
std::string currentGameDir; //Current game's root directory
std::string fontDir;
std::string textureDir;
std::string textDir; //Current game's asset directories
// Game data
cJSON* comboData = NULL; //Root of combination JSON data

namespace Game {
	std::string getFontDir() { return fontDir; }
	std::string getTextureDir() { return textureDir; }
	std::string getTextDir() { return textDir; }
	cJSON* getComboData() { return comboData; }

	// Loads game data and returns true if it was loaded successfully
	bool loadGameData(std::string id) {
		gameID = id;
		fontDir = GAME_DATA_DIR + gameID  + "/font/";
		textureDir = GAME_DATA_DIR + gameID + "/textures/";
		textDir = GAME_DATA_DIR + gameID + "/lang/";
		std::string comboJSONPath = GAME_DATA_DIR + gameID + "/combos.json";
		std::string comboJSONStr = loadFile(comboJSONPath);
		comboData = cJSON_Parse(comboJSONStr.c_str());
		if (comboData == NULL) {
			std::cerr << "ERROR: Game \"" << gameID << "\" has missing or malformed combination data." << std::endl;
			return false;
		}
		return true;
	}
}
