#include "GameHandler.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "glaze/glaze.hpp"
#include "glaze/json.hpp"
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <utility>
#include "IO.hpp"
#include "Progress.hpp"

// Directories
const std::string GAME_DATA_DIR = "gamedata/";
std::string gameID; //ID of the current name, used to determine its directory
std::string currentGameDir; //Current game's root directory
std::string fontDir;
std::string textureDir;
std::string textDir; //Current game's asset directories

// Game data
std::unordered_map<std::string, int> elemKeyLookback;
std::vector<std::string> elementStringIds; //String ID for every element in order of serialization
std::vector<std::vector<std::pair<std::vector<int>, std::vector<int>>>> elementComboData; //Element combination data
std::vector<std::string> elementNames; //Name of every element in order
TTF_Font* font; //Game font (//TODO EXPAND)

template <>
struct glz::meta<ComboData> {
	static constexpr auto value = object(
		"elems", &ComboData::elems,
		"results", &ComboData::results
	);
};

/**
 * Loads and serializes element data for the current game
 * @return Success (true if failed, false if successful)
 */
bool initComboData(std::string comboDataDir) {
	std::string jsonStr = loadFile(comboDataDir);
	// Serialize string IDs
	std::vector<std::pair<std::string, glz::json_t>> root;
	if (glz::read_json(root, jsonStr) != glz::error_code::none) {
		std::cerr << "Element JSON parsing error.\n";
		return true;
	}

	int id = 0;
	for (std::pair<std::string, glz::json_t>& elem : root) {
		elemKeyLookback[elem.first] = id; //Add string ID lookback
		elementStringIds.push_back(elem.first); //Add to list of string IDs
		id++;
	}
	for (std::pair<std::string, glz::json_t>& elem : root) {
		std::vector<ComboData> combos;
		if (glz::read_json(combos, elem.second["combos"]) != glz::error_code::none) {
			std::cerr << "Element combo JSON parsing error.\n";
			return true;
		}

		std::vector<std::pair<std::vector<int>, std::vector<int>>> serializedComboData;
		for (auto combo : combos) {
			// Serialize recipe
			std::vector<int> serializedRecipes;
			for (auto recipe : combo.elems) {
				serializedRecipes.push_back(elemKeyLookback[recipe]);
			}
			// Serialize results
			std::vector<int> serializedResults;
			for (auto result : combo.results) {
				serializedResults.push_back(elemKeyLookback[result]);
			}
			serializedComboData.push_back({serializedRecipes, serializedResults});
		}
		elementComboData.push_back(serializedComboData);
	}
	return false;
}

/**
 * Loads the element names for a current language
 * @return Success (true if failed, false if successful)
 */
bool loadElementNames(std::string language) {
	elementNames.clear();
	std::string jsonStr = loadFile(textDir+language+".json");
	std::unordered_map<std::string, std::string> elemStrings; //Name for each element
	if (glz::read_json(elemStrings, jsonStr) != glz::error_code::none) {
		std::cerr << "Error parsing text JSON for language: " << language << '\n';
		return true;
	}
	// Get names for each element
	for (const auto& id : elementStringIds) {
		if (elemStrings.contains(id)) {
			elementNames.push_back(elemStrings[id]);
		} else {
			elementNames.push_back(id); //Set name to plain ID if no name is specified for the element
		}
	}
	return false;
}

std::string Game::getFontDir() { return fontDir; }
std::string Game::getTextureDir() { return textureDir; }
std::string Game::getTextDir() { return textDir; }
std::vector<std::pair<std::vector<int>, std::vector<int>>>* Game::getComboData(int id) { return &elementComboData[id]; }

std::string Game::getElementStrId(int id) { return elementStringIds[id]; }
std::string Game::getElementName(int id) { return elementNames[id]; }
std::string Game::getElementName(std::string id) { return elementNames[elemKeyLookback[id]]; }
std::vector<std::string>* Game::getElementNames() { return &elementNames; }
int Game::getElementNumId(std::string id) { return elemKeyLookback[id]; }
TTF_Font* Game::getFont() { return font; }

// Loads game data and returns true if it was loaded successfully
bool Game::loadGameData(std::string id) {
	gameID = id;
	fontDir = GAME_DATA_DIR + gameID  + "/font/";
	textureDir = GAME_DATA_DIR + gameID + "/textures/";
	textDir = GAME_DATA_DIR + gameID + "/lang/";
	initComboData(GAME_DATA_DIR + gameID + "/combos.json");
	loadElementNames("en-us");
	std::string fontPath = fontDir+"Open-Sans.ttf";
	font = TTF_OpenFont(fontPath.c_str(), 12.0f);
	Progress::UnlockElement(elemKeyLookback["air"]);
	Progress::UnlockElement(elemKeyLookback["earth"]);
	Progress::UnlockElement(elemKeyLookback["fire"]);
	Progress::UnlockElement(elemKeyLookback["water"]);
	return true;
}
