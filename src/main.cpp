#define SDL2_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <memory>
#include "GameHandler.hpp"
#include "Element.hpp"
#include "ElementMenu.hpp"
#include "Animation.hpp"
#include "SDL_FontCache.h"
#include "Lang.hpp"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char* argv[])
{
	// SDL Init
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// Initialize window
	window = SDL_CreateWindow("Alchemy++ alpha v0.3", 64, 64, 800, 600, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	} else {
		int imgFlags = IMG_INIT_PNG;
		if(!(IMG_Init(imgFlags) & imgFlags)) {
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		}
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		if (window != NULL) {
			SDL_DestroyWindow(window);
		}
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Load default game
	if (!Game::loadGameData("default")) {
		std::cerr << "ERROR: Game data \"" << "default" << "\" is missing or malformed.\n";
	}
	Text::loadAll("en-us"); //Load game text

	// Set up ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  //Enable Gamepad Controls
	ImGui::StyleColorsDark(); //Use dark mode by default
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f); //Center title bars
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer); //Init for SDL renderer

	// Load fonts
	std::string fontPath = Game::getFontDir() + "Open-Sans.ttf"; //Get font directory
	ImFont* guiFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f); //Load font in ImGui

	TTF_Init();
	FC_Font* font = FC_CreateFont();
	FC_LoadFont(font, renderer, fontPath.c_str(), 12, FC_MakeColor(255,255,255,255), TTF_STYLE_NORMAL); // Load font in SDL2

	std::string texDir = Game::getTextureDir() + "backgrounds/emptyuniverse.png";
	SDL_Texture* tex = IMG_LoadTexture(renderer, texDir.c_str());
	std::string addBtnDir = Game::getTextureDir() + "buttons/addBtn.png";
	SDL_Texture* addBtn = IMG_LoadTexture(renderer, addBtnDir.c_str());
	if (tex == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Game loop init
	std::vector<std::string> elemsUnlocked = {"air", "earth", "fire", "water"};
	std::vector<std::unique_ptr<DraggableElement>> draggables; //List of draggable elements on screen
	std::vector<std::string> elementsUnlocked = {"air", "earth", "fire", "water"}; //Every element the user has unlocked

	elem::spawnDraggable(draggables, 288, 208, "air");
	elem::spawnDraggable(draggables, 50, 50, "fire");

	DraggableElement* selectedElem = NULL; //Currently selected draggable

	bool leftClickDown = false; //Left click state, used to track drag and drop
	long leftClickTick = 0; //The tick when left was clicked, used to detect double clicks
	bool rightClickDown = false; //Middle click state
	SDL_Point mousePos; //Mouse position
	SDL_Point clickOffset; //Point in the element box clicked relative to its boundary
	int winWidth, winHeight; //Window size
	std::string elementMenuInputBuf; //Element menu text box string
	std::vector<std::string> matchingElemIDs; //Any elements matching the search query
	ImDrawList* elementSearchResults; //

	auto endTick = Clock::now();
	double deltaTime = 1.0/60;
	// Game flags
	bool deleteNeeded = false; //Used to determine if any elements need to be deleted
	bool zSortNeeded = false; //Used to indicate if elements need to be sorted
	bool elementMenuOpen = true;
	bool quit = false; //Main loop exit flag
	while (!quit) {
		SDL_GetWindowSize(window, &winWidth, &winHeight); //Get screen size
		auto startTick = Clock::now();

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);
			if (e.type == SDL_QUIT) {
				quit = true;
				break; //Close the program if X is clicked
			}
			if(!io.WantCaptureMouse) {
				switch (e.type) {
				case SDL_MOUSEMOTION: {
					mousePos = {e.motion.x, e.motion.y}; //Get mouse position
					if (leftClickDown && selectedElem != NULL) {
						selectedElem->box->x = mousePos.x - clickOffset.x;
						selectedElem->box->y = mousePos.y - clickOffset.y; //Update rectangle position while its being dragged
					}
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					if (leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
						leftClickDown = false;

						if (selectedElem != NULL) {
							selectedElem->z++; //Move behind
							selectedElem->makeCombo(draggables, elementsUnlocked); //See if combination was made with another element
						}
						selectedElem = NULL; //Release selected rectangle when left is released
						zSortNeeded = true; //Re-sort Z-index after element was dropped
					}
					// Remove if right clicked
					if (rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
						rightClickDown = false;
						if (selectedElem != NULL) {
							selectedElem->anim = ANIM_SHRINK;
							anim::animInProgress = true;
							selectedElem = nullptr;
						}
					}
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					// Get element clicked regardless of mouse button
					std::vector<DraggableElement*> clickMatches; //Every element that the cursor clicked on
					if (selectedElem == NULL) {
						for (auto &d : draggables) {
							if (SDL_PointInRect(&mousePos, d->box)) {
								clickMatches.push_back(d.get());
							}
						}
						if (!clickMatches.empty()) {
							zSortNeeded = true; //Z-index will need to be resorted as this element will be moved to the front
							std::stable_sort(clickMatches.begin(), clickMatches.end(), [](DraggableElement* d1, DraggableElement* d2) {
								return d1->z > d2->z;
							});

							selectedElem = clickMatches.back(); //Select rectangle with highest Z-index
							selectedElem->z = 0; //Move to front of z-index
						}
					}
					if (!leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
						leftClickDown = true;
						if (selectedElem == NULL) {
							if (!clickMatches.empty()) {
								clickOffset.x = mousePos.x - selectedElem->box->x;
								clickOffset.y = mousePos.y - selectedElem->box->y; //Get clicked point in element box relative to its boundary
							}
						} else {
							clickOffset.x = mousePos.x - selectedElem->box->x;
							clickOffset.y = mousePos.y - selectedElem->box->y; //Don't look for a new element to select if one is already selected
						}
						// Spawn new elements on double click
						if (SDL_GetTicks64() > leftClickTick && SDL_GetTicks64() <= leftClickTick + 250) { //Double clicks have to be within 1/4 second of each other
							if (selectedElem == NULL) {
								elem::spawnDraggable(draggables, mousePos.x, mousePos.y+40, "air");
								elem::spawnDraggable(draggables, mousePos.x, mousePos.y-40, "earth");
								elem::spawnDraggable(draggables, mousePos.x-40, mousePos.y, "fire");
								elem::spawnDraggable(draggables, mousePos.x+40, mousePos.y, "water");
							} else {
								int hSpawnOffset = ((selectedElem->box->x + selectedElem->box->w/2) > mousePos.x) ? -40 : 40;
								int vSpawnOffset = ((selectedElem->box->y + selectedElem->box->h/2) > mousePos.y) ? -40 : 40; //Duplocate the element to the corner it was clicked
								elem::spawnDraggable(draggables, selectedElem->box->x+hSpawnOffset, selectedElem->box->y+vSpawnOffset, selectedElem->id); //Duplicate element if it's double clicked
							}
						}
						leftClickTick = SDL_GetTicks64(); //Get next click tick
					}

					if (!rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
						rightClickDown = true;
					}
					break;
				}
				}
			}
		}
		// Sort draggable elements by z index when animations finish
		if (zSortNeeded && !anim::animInProgress) {
			std::stable_sort(draggables.begin(), draggables.end(), []
			(const std::unique_ptr<DraggableElement> &d1, const std::unique_ptr<DraggableElement> &d2) {
				return d1->z > d2->z;
			});
			zSortNeeded = false;
		}
		// Apply animations
		if (anim::animInProgress) {
			bool animDone = false;
			for (auto &d : draggables) {
				if (d->anim == ANIM_SHRINK) {
					anim::animateShrink(d.get(), deltaTime);
					animDone = true;
				} else if (d->anim == ANIM_SHRINK_END) {
					d->queuedForDeletion = true; //Queue element for deletion when it finishes shrinking
					deleteNeeded = true; //Remove elements when they finish shrinking
					animDone = true;
				} else if (d->anim == ANIM_GROW) {
					anim::animateGrow(d.get(), deltaTime);
					animDone = true;
				}
				anim::animInProgress = animDone; //Stop animations if none are running
			}
		}
		// Remove elements that are queued for deletion if needed
		if (deleteNeeded) {
			draggables.erase(std::remove_if(draggables.begin(), draggables.end(), []
			(const std::unique_ptr<DraggableElement> &d) { return d->queuedForDeletion; }
			), draggables.end());
			deleteNeeded = false;
		}

		SDL_RenderClear(renderer);

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (elementMenuOpen)
		{
			ImGui::SetNextWindowFocus();
			ImGui::SetNextWindowSize(ImVec2(0.5*winWidth, 0.75*winHeight), ImGuiCond_Always);
			ImGui::SetNextWindowPos(ImVec2(winWidth*0.5, winHeight*0.5), ImGuiCond_Always, ImVec2(0.5f,0.5f)); //Center window
			ImGui::Begin("Add Element", &elementMenuOpen, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(63,63,63,255));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(95,95,95,255)); //Style selectables
			if (ImGui::InputTextWithHint("##elementMenuSearchBox", "Element", &elementMenuInputBuf, ImGuiInputTextFlags_CallbackEdit)) {
				matchingElemIDs = getElemSearchResults(elementMenuInputBuf, elementsUnlocked);
			}
			int selectedItemInd = 0;
			if (!matchingElemIDs.empty()) {
				int i = 0; //Element index counter
				for (auto id : matchingElemIDs) {
					std::string elemName = Text::getElemName(id);
					if (elemName == "") { elemName = "##"; }
					bool selected = (selectedItemInd == i);
					auto pos = ImGui::GetCursorPos(); //Get position of selectable box so items can be placed on it
					ImGui::SetNextItemAllowOverlap();
					std::string selectableId = "##" + id; //Unique ID for the element's selection box
					if (ImGui::Selectable(selectableId.c_str(), selected, ImGuiSelectableFlags_AllowOverlap, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 2))) {
						selectedItemInd = i;
					}

					ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
					ImGui::Text(elemName.c_str());
					std::string idText = "id: " + id;
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(127,127,127,255));
					ImGui::Text(idText.c_str());
					ImGui::PopStyleColor();
					ImGui::Separator();
					i++;
				}
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleColor(); //Pop selectable styles
			ImGui::End();
		}

		SDL_RenderCopy(renderer, tex, NULL, NULL); //Render background
		SDL_Rect r{winWidth/2-32, winHeight-80, 64, 64};
		SDL_RenderCopy(renderer, addBtn, NULL, &r); //Render add button

		//Render text
		FC_Draw(font, renderer, 0, 0, "Alchemy++ alpha v0.3");

		FC_Draw(font, renderer, 20, winHeight-20, "elems: %d", draggables.size());

		// Render every draggable element
		for (auto &d : draggables) {
			if ((int)d->scale != 1) {
				SDL_Rect* scaledRect = anim::applyScale(d.get()); //Get scaled rect
				SDL_RenderCopy(renderer, d->texture, NULL, scaledRect);
				free(scaledRect); //Free it to avoid leak
			} else {
				SDL_RenderCopy(renderer, d->texture, NULL, d->box);
			}

			float textWidth = FC_GetWidth(font, d->name.c_str());
			float textHeight = FC_GetHeight(font, d->name.c_str());
			FC_DrawScale(font, renderer,
				(d->box->x + (d->box->w - textWidth)/2) + (textWidth - textWidth * d->scale)/2, //Text X position
				(d->box->y + d->box->w) + (textHeight - textHeight * d->scale), //Text Y position
				{d->scale, d->scale}, d->name.c_str());
		}

		FC_Draw(font, renderer, winWidth-170, 10, "FPS: %f", 1000/deltaTime);

		ImGui::Render(); //Render ImGui stuff
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);

		endTick = Clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(endTick - startTick).count() / 1000.0; //Get the time the frame took in ms
	}

	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	printf("\n"); //Break after debug messages

	return EXIT_SUCCESS;
}
