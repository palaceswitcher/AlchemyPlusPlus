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
	float elemBoxSize = 64.0f;
	float winWidth = ImGui::GetWindowWidth();
	float winHeight = ImGui::GetWindowHeight();
	if (elementMenuOpen) {
		int columnCount = (int)(ImGui::GetWindowWidth() / 64);
		int rowCount = round((float)elementsUnlocked.size() / columnCount); //Get column and row count
		//ImLerp(255.0f, 0.0f, (ImGui::GetTime() - start_time) / duration_seconds)
		ImGui::SetNextWindowFocus();
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always); //Center window
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(15,15,15,255)); //Set color of background
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(63,63,63,255));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(95,95,95,255));
		ImGui::Begin("Add Element", &elementMenuOpen, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize); //Style selectables
		if (ImGui::InputTextWithHint("##elementMenuSearchBox", "Element", &elementMenuInputBuf, ImGuiInputTextFlags_CallbackEdit)) {
			matchingElemIDs = getElemSearchResults(elementMenuInputBuf, elementsUnlocked);
		}
		// Create map of selection counts for each unlocked element if needed
		if (elementMenuSelectCounts.empty()) {
			for (auto id : elementsUnlocked) {
				elementMenuSelectCounts.insert(std::make_pair(id, 0));
			}
		}

		if (!matchingElemIDs.empty()) {
			int colCount = floor(winWidth / elemBoxSize);
			for (int i = 0; i < matchingElemIDs.size(); i++) {
				if (i % colCount != 0) { ImGui::SameLine(); }
				std::string elemName = Text::getElemName(matchingElemIDs[i]);
				ImGui::PushID(i);
				ImVec2 pos = ImGui::GetCursorPos(); //Get position of selectable box so items can be placed on it
				if (ImGui::Selectable("##", false, ImGuiSelectableFlags_None, ImVec2(elemBoxSize, elemBoxSize))) {
					int incAmount = 1;
					if (ImGui::IsKeyDown(ImGuiKey_ModCtrl)) {
						incAmount = 5;
					}
					if (elementMenuSelectCounts[matchingElemIDs[i]] + incAmount > 99) {
						elementMenuSpawnCount += (99 - elementMenuSelectCounts[matchingElemIDs[i]]);
						elementMenuSelectCounts[matchingElemIDs[i]] = 99;
					} else {
						elementMenuSpawnCount += incAmount;
						elementMenuSelectCounts[matchingElemIDs[i]] += incAmount;
					}
				}
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					int decAmount = 1;
					if (ImGui::IsKeyDown(ImGuiKey_ModCtrl)) {
						decAmount = 5;
					}
					if (elementMenuSelectCounts[matchingElemIDs[i]] - decAmount < 0) {
						elementMenuSpawnCount -= (decAmount - elementMenuSelectCounts[matchingElemIDs[i]]);
						elementMenuSelectCounts[matchingElemIDs[i]] = 0;
					} else {
						elementMenuSpawnCount -= decAmount;
						elementMenuSelectCounts[matchingElemIDs[i]] -= decAmount;
					}
				}
				else if (ImGui::IsItemClicked(ImGuiMouseButton_Middle)) {
					elementMenuSpawnCount -= elementMenuSelectCounts[matchingElemIDs[i]];
					elementMenuSelectCounts[matchingElemIDs[i]] = 0;
				}
				auto afterPos = ImGui::GetCursorPos();
				ImGui::PopID();
				int elemW, elemH;
				SDL_Texture* elemTex = Board::loadTexture(matchingElemIDs[i], &elemW, &elemH); //Load texture and its dimensions
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();
				ImVec2 center = ImVec2(min.x + ceil((max.x - min.x - elemW) * 0.5f), min.y + (elemBoxSize * 0.125f));
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				// Draw icon
				drawList->AddImage((ImTextureID)(intptr_t) Board::textureIndex[matchingElemIDs[i]], center, ImVec2(center.x + elemW, center.y + elemH));
				// Draw spawn count
				std::string spawnCountTxt = std::to_string(elementMenuSelectCounts[matchingElemIDs[i]]);
				ImVec2 textSize = ImGui::CalcTextSize(spawnCountTxt.c_str(), nullptr, false, elemBoxSize);
				float spawnCountX = pos.x + (elemBoxSize - textSize.x);
				float spawnCountY = pos.y;
				drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
									ImVec2(spawnCountX, spawnCountY),
									IM_COL32(191,191,191,255),
									spawnCountTxt.c_str(), nullptr, elemBoxSize);
				textSize = ImGui::CalcTextSize(elemName.c_str(), nullptr, false, elemBoxSize);
				float textStartX = pos.x + ((elemBoxSize - textSize.x) * 0.5f);
				float textStartY = pos.y + (elemBoxSize * 0.375f) + textSize.y;
				drawList->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
									ImVec2(textStartX, textStartY),
									ImGui::GetColorU32(ImGuiCol_Text),
									elemName.c_str(), nullptr, elemBoxSize);
			}
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