#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "Board.hpp"
#include "Animation.hpp"
#include "Progress.hpp"
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include "GameHandler.hpp"
#include "ElementMenu.hpp"
#include "Animation.hpp"

std::string elementMenuInputBuf; // Element menu search string
std::vector<int> matchingElemIDs; // Any elements matching the search query
int elementMenuSpawnCount; // Number of elements spawned in the element menu. Used to determine where an element should be placed
std::unordered_map<int, int> elementMenuSelectCounts; // The amount of times each element was selected in the element menu
bool elementMenuOpen = false; // Whether element menu is opened or not

void UI::openElementMenu() {
	elementMenuOpen = true;
}

// Get every unlocked element that matches the query
std::vector<int> getElemSearchResults(std::string query) {
	std::vector<int> matchingElementIDs; // IDs of every element that matches the query
	for (auto id : Progress::GetUnlockedElements()) {
		if (Game::getElementName(id).find(query) != std::string::npos) {
			matchingElementIDs.push_back(id);
		}
	}
	return matchingElementIDs;
}

void UI::renderElemMenu(SDL_Renderer* ren) {
	float elemBoxSize = 64.0f;
	if (elementMenuOpen) {
		// Close when escape key is pressed
		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape)) {
			elementMenuOpen = false;
		}

		ImGui::SetNextWindowFocus();
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always); // Center window

		// Style selectables
		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(15,15,15,255)); 
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(63,63,63,255));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(95,95,95,255));

		ImGui::Begin("Add Element", &elementMenuOpen, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize);

		// Search box
		if (ImGui::InputTextWithHint("##elementMenuSearchBox", "Element", &elementMenuInputBuf) || matchingElemIDs.empty()) {
			matchingElemIDs = getElemSearchResults(elementMenuInputBuf);
		}
		// Create map of selection counts for each unlocked element if needed
		if (elementMenuSelectCounts.empty()) {
			for (auto id : Progress::GetUnlockedElements()) {
				elementMenuSelectCounts.insert({id, 0});
			}
		}

		if (!matchingElemIDs.empty()) {
			// TODO FIX COLUMN COUNT TO ACCOUNT FOR MARGINS
			int colCount = floorf(ImGui::GetWindowWidth() / (elemBoxSize+ImGui::GetStyle().WindowPadding.x));
			for (int i = 0; i < matchingElemIDs.size(); i++) {
				if (i % colCount != 0) { ImGui::SameLine(); }
				std::string elemName = Game::getElementName(matchingElemIDs[i]);
				ImGui::PushID(i);
				ImVec2 pos = ImGui::GetCursorPos(); // Get position of selectable box so items can be placed on it
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
				float elemW, elemH;
				SDL_Texture* elemTex = Board::loadTexture(ren, matchingElemIDs[i], &elemW, &elemH); // Load texture and its dimensions
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
		ImGui::PopStyleColor(); // Pop selectable styles
		ImGui::End();
	// Only render everything else when the menu is closed
	} else {
		// Get number of elements
		int elementCount = 0;
		for (auto &pair : elementMenuSelectCounts) {
			for (int i = 0; i < pair.second; i++) {
				elementCount++;
			}
		}

		float radius = 64.f;
		float circumference = 2*M_PI*radius; // Circumference of current spawn ring

		int e = 0; // Current element index (TODO: OPTIMIZE THIS)
		for (auto &pair : elementMenuSelectCounts) {
			for (int i = 0; i < pair.second; i++) {
				SDL_Window* win = SDL_GetRenderWindow(ren); // Get window
				int centerX, centerY;
				SDL_GetWindowSize(win, &centerX, &centerY); // Get screen size (TODO REFACTOR THIS)
				centerX /= 2;
				centerY /= 2;

				float theta = 2*M_PI * e / roundf(circumference / 48.0f);
				if (roundf(circumference / 48.0f) > elementCount) {
					theta = 2*M_PI * e / (elementCount);
				}

				float x = centerX + radius * cosf(theta);
				float y = centerY + radius * sinf(theta);

				Board::spawnDraggable(ren, x - 16, y - 16, pair.first);

				e++;
				// Make another ring if needed
				if (circumference / 48.0f < e) {
					elementCount -= e;
					e = 0;
					radius += 48.0f; // Update radius for new ring
					circumference = 2*M_PI*radius; // Update circumference for new ring
				}
			}
		}
		elementMenuSpawnCount = 0; // Reset spawned element count when the element menu is closed
		matchingElemIDs.clear();
		elementMenuSelectCounts.clear(); // Clear element selection counter
		Board::focus(); // Refocus board
	}
}