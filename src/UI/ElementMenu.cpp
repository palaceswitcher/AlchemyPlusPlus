#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Board.hpp"
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include "cJSON.h"
#include "Lang.hpp"
#include "GameHandler.hpp"
#include "ElementMenu.hpp"
#include "Animation.hpp"

std::string elementMenuInputBuf; //Element menu text box string
std::vector<std::string> matchingElemIDs; //Any elements matching the search query
int elementMenuSpawnCount; //Number of elements spawned in the element menu. Used to determine where an element should be placed
std::unordered_map<std::string, int> elementMenuSelectCounts; //The amount of times each element was selected in the element menu
bool elementMenuOpen = false; //Whether element menu is opened or not
bool elementSearchSelected; //Used to determine if an element is selected in the element search menu

void UI::openElementMenu() {
	elementMenuOpen = true;
}

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

void UI::renderElemMenu(std::vector<std::string> elementsUnlocked) {
	float winWidth = ImGui::GetWindowWidth();
	float winHeight = ImGui::GetWindowHeight();
	if (elementMenuOpen) {
		int columnCount = (int)(ImGui::GetWindowWidth() / 64);
		int rowCount = round((float)elementsUnlocked.size() / columnCount); //Get column and row count
		//ImLerp(255.0f, 0.0f, (ImGui::GetTime() - start_time) / duration_seconds)
		ImGui::SetNextWindowFocus();
		ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(winWidth*0.5, winHeight*0.5), ImGuiCond_Always, ImVec2(0.5f,0.5f)); //Center window
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(15,15,15,255)); //Set color
		ImGui::Begin("Add Element", &elementMenuOpen, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(63,63,63,255));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(95,95,95,255)); //Style selectables
		if (ImGui::InputTextWithHint("##elementMenuSearchBox", "Element", &elementMenuInputBuf, ImGuiInputTextFlags_CallbackEdit)) {
			matchingElemIDs = getElemSearchResults(elementMenuInputBuf, elementsUnlocked);
		}
		// Create map of selection counts for each unlocked element if needed
		if (elementMenuSelectCounts.empty()) {
			for (auto id : elementsUnlocked) {
				elementMenuSelectCounts.insert(std::make_pair(id, 0));
			}
		}

		if (!matchingElemIDs.empty() && ImGui::BeginTable("##elementMenuSearchGrid", columnCount)) {
			for (auto id : matchingElemIDs) {
				ImGui::TableNextColumn();
				std::string elemName = Text::getElemName(id);
				auto pos = ImGui::GetCursorPos(); //Get position of selectable box so items can be placed on it
				std::string selectableId = "##" + id; //Unique ID for the element's selection box
				if (ImGui::Selectable(selectableId.c_str(), elementSearchSelected, 0, ImVec2(64.0f, 64.0f))) {
					elementMenuSelectCounts[id]++;
					elementMenuSpawnCount++;
				}
				int elemW, elemH;
				SDL_Texture* elemTex = Board::loadTexture(id, &elemW, &elemH); //Load texture and its dimensions
				ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
				ImGui::Text("%d", elementMenuSelectCounts[id]); //Display the number of the element spawned
				ImGui::SetCursorPos(ImVec2(pos.x+(64.0-elemW)/2, pos.y));
				ImGui::Image((ImTextureID)(intptr_t) Board::textureIndex[id], ImVec2((float)elemW, (float)elemH));
				ImGui::SetCursorPos(ImVec2(pos.x, pos.y+32));
				ImGui::PushTextWrapPos(pos.x+64);
				ImGui::TextWrapped("%s", elemName.c_str());
				//ImGui::PopTextWrapPos();
			}
			ImGui::EndTable();
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor(); //Pop selectable styles
		ImGui::End();
	// Only render everything else when the menu is closed
	} else {
		for (auto &pair : elementMenuSelectCounts) {
			for (int i = 0; i < pair.second; i++) {
				Board::spawnDraggable(288, 208, pair.first);
			}
		}
		elementMenuSpawnCount = 0; //Reset spawned element count when the element menu is closed
		elementMenuSelectCounts.clear(); //Clear element selection counter
	}
}