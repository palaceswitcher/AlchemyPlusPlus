#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
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
#include <unordered_map>
#include <cmath>
#include "Text.hpp"
#include "GameHandler.hpp"
#include "Element.hpp"
#include "Board.hpp"
#include "ElementMenu.hpp"
#include "Sprite.hpp"
#include "Animation.hpp"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"); //Don't disable compositor.
	// SDL Init
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}
	// Initialize window
	Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	SDL_Window* window = SDL_CreateWindow("Alchemy++ alpha v0.4", DEFAULT_WIDTH, DEFAULT_HEIGHT, window_flags);
	if (window == nullptr) {
		std::cerr << stderr << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
		return -1;
	}
	SDL_SetWindowMinimumSize(window, 640, 480);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	SDL_SetRenderVSync(renderer, 1);
	if (renderer == nullptr) {
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());;
		return -1;
	}
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	TTF_Init();
	// Load default game
	if (!Game::loadGameData("default")) {
		std::cerr << "ERROR: Game data \"" << "default" << "\" is missing or malformed.\n";
	}

	// Set up ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  //Enable Gamepad Controls
	ImGui::StyleColorsDark(); //Use dark mode by default
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f); //Center title bars
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer); //Init for SDL renderer

	// Load fonts
	std::string fontPath = Game::getFontDir() + "Open-Sans.ttf"; //Get font directory
	ImFont* guiFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f); //Load font in ImGui

	std::string texDir = Game::getTextureDir() + "backgrounds/emptyuniverse.png";
	SDL_Texture* tex = IMG_LoadTexture(renderer, texDir.c_str());
	std::string addBtnDir = Game::getTextureDir() + "buttons/addBtn.png";
	SDL_Texture* addButtonTex = IMG_LoadTexture(renderer, addBtnDir.c_str());
	if (tex == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	GfxResource fpsDisplayTexture;
	GfxResource elemCountTexture;
	GfxResource versStringTexture;
	GFX::renderText(versStringTexture, renderer, "Alchemy++ alpha v0.4", {255,255,255,255});

	// Game loop init
	std::vector<std::string> elementsUnlocked = {"air", "earth", "fire", "water"}; //Every element the user has unlocked
	SDL_FRect addButtonRect = {DEFAULT_WIDTH/2-32, DEFAULT_HEIGHT-80, 64, 64};
	Sprite addButton = Sprite(addButtonRect, addButtonTex); //Element add button

	// Spawn default elements
	Board::spawnDraggable(renderer, DEFAULT_WIDTH/2-16, DEFAULT_HEIGHT/2+24, Game::getElementNumId("air"));
	Board::spawnDraggable(renderer, DEFAULT_WIDTH/2-16, DEFAULT_HEIGHT/2-56, Game::getElementNumId("earth"));
	Board::spawnDraggable(renderer, DEFAULT_WIDTH/2-56, DEFAULT_HEIGHT/2-16, Game::getElementNumId("fire"));
	Board::spawnDraggable(renderer, DEFAULT_WIDTH/2+24, DEFAULT_HEIGHT/2-16, Game::getElementNumId("water"));

	DraggableElement* selectedElem = NULL; //Currently selected draggable

	bool leftClickDown = false; //Left click state, used to track drag and drop
	bool rightClickDown = false; //Middle click state
	long leftClickTick = 0; //The tick when left was clicked, used to detect double clicks
	bool addButtonClicked = false;
	SDL_FPoint mousePos; //Mouse position
	SDL_Point clickOffset; //Point in the element box clicked relative to its boundary
	int winWidth, winHeight; //Window size

	auto endTick = Clock::now();
	double deltaTime = 1.0/60;
	// Game flags
	bool deleteNeeded = false; //Used to determine if any elements need to be deleted
	bool zSortNeeded = false; //Used to indicate if elements need to be sorted
	bool quit = false; //Main loop exit flag
	while (!quit) {
		auto startTick = Clock::now(); //Get start tick
		SDL_GetWindowSize(window, &winWidth, &winHeight); //Get new screen size

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
				break; //Close the program if X is clicked
			}
			if(!io.WantCaptureMouse) {
				switch (e.type) {
				case SDL_EVENT_MOUSE_MOTION: {
					mousePos = {e.motion.x, e.motion.y}; //Get mouse position
					if (leftClickDown && selectedElem != NULL) {
						selectedElem->box.x = mousePos.x - clickOffset.x;
						selectedElem->box.y = mousePos.y - clickOffset.y; //Update rectangle position while its being dragged
					}
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_UP: {
					if (leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
						leftClickDown = false;

						if (selectedElem != NULL) {
							selectedElem->z++; //Move behind
							selectedElem->makeCombo(renderer, elementsUnlocked); //See if combination was made with another element
							deleteNeeded = selectedElem->queuedForDeletion = (selectedElem->box.x >= winWidth || selectedElem->box.y >= winHeight); //Delete element if it goes off-screen
						}
						selectedElem = NULL; //Release selected rectangle when left is released
						zSortNeeded = true; //Re-sort Z-index after element was dropped
					}
					// Remove if right clicked
					if (rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
						rightClickDown = false;
						if (selectedElem != NULL) {
							selectedElem->addAnim({ANIM_SCALE, 0.0f, 0.25f});
							selectedElem->queuedForDeletion = true;
							deleteNeeded = true;
							selectedElem = nullptr;
						}
					}
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_DOWN: {
					// Get element clicked regardless of mouse button
					std::vector<DraggableElement*> clickMatches; //Every element that the cursor clicked on
					if (selectedElem == NULL) {
						// Check if circle around the add button is clicked
						if (((mousePos.x-(addButton.box.x+32))*(mousePos.x-(addButton.box.x+32)) + (mousePos.y-(addButton.box.y+32))*(mousePos.y-(addButton.box.y+32))) < 32*32) {
							addButtonClicked = true;
							UI::openElementMenu();
						} else {
							for (auto& d : *(Board::getDraggableElems())) {
								if (!d->queuedForDeletion && SDL_PointInRectFloat(&mousePos, &d->box)) {
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
					}
					if (!leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
						leftClickDown = true;
						if (selectedElem == NULL) {
							if (!clickMatches.empty()) {
								clickOffset.x = mousePos.x - selectedElem->box.x;
								clickOffset.y = mousePos.y - selectedElem->box.y; //Get clicked point in element box relative to its boundary
							}
						} else {
							clickOffset.x = mousePos.x - selectedElem->box.x;
							clickOffset.y = mousePos.y - selectedElem->box.y; //Don't look for a new element to select if one is already selected
						}
						// Spawn new elements on double click
						if (SDL_GetTicks() > leftClickTick && SDL_GetTicks() <= leftClickTick + 250) { //Double clicks have to be within 1/4 second of each other
							if (selectedElem == NULL) {
								Board::spawnDraggable(renderer, mousePos.x, mousePos.y+40, Game::getElementNumId("air"));
								Board::spawnDraggable(renderer, mousePos.x, mousePos.y-40, Game::getElementNumId("earth"));
								Board::spawnDraggable(renderer, mousePos.x-40, mousePos.y, Game::getElementNumId("fire"));
								Board::spawnDraggable(renderer, mousePos.x+40, mousePos.y, Game::getElementNumId("water"));
							} else {
								int hSpawnOffset = ((selectedElem->box.x + selectedElem->box.w/2) > mousePos.x) ? -40 : 40;
								int vSpawnOffset = ((selectedElem->box.y + selectedElem->box.h/2) > mousePos.y) ? -40 : 40; //Duplicate the element to the corner it was clicked
								Board::spawnDraggable(renderer, selectedElem->box.x+hSpawnOffset, selectedElem->box.y+vSpawnOffset, selectedElem->id); //Duplicate element if it's double clicked
							}
						}
						leftClickTick = SDL_GetTicks(); //Get next click tick
					}

					if (!rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
						rightClickDown = true;
					}
					break;
				}
				}
			}
			// Respond to window resize
			if (e.type == SDL_EVENT_WINDOW_RESIZED) {
				int prevWinWidth = winWidth;
				int prevWinHeight = winHeight;
				SDL_GetWindowSize(window, &winWidth, &winHeight); //Get new screen size
				addButton.box = {(float)winWidth/2-32, (float)winHeight-80, 64, 64}; //Position add button to the center of the screen
				// Adjust draggable elements relative to their previous position
				for (auto& d : *(Board::getDraggableElems())) {
					d->box.x = round(d->box.x * (double)winWidth/prevWinWidth);
					d->box.y = round(d->box.y * (double)winHeight/prevWinHeight);
				}
			}
		}
		// Sort draggable elements by z index when animations finish
		if (zSortNeeded) {
			auto draggableElems = Board::getDraggableElems();
			std::stable_sort(draggableElems->begin(), draggableElems->end(), []
			(const std::unique_ptr<DraggableElement> &d1, const std::unique_ptr<DraggableElement> &d2) {
				return d1->z > d2->z;
			});
			zSortNeeded = false;
		}
		// Remove elements that are queued for deletion if needed
		//if (deleteNeeded) {
			Board::clearQueuedElements();
			deleteNeeded = false;
		//}

		SDL_RenderClear(renderer);

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		UI::renderElemMenu(renderer, elementsUnlocked);

		SDL_RenderTexture(renderer, tex, nullptr, nullptr); //Render background
		SDL_RenderTexture(renderer, addButton.texture, nullptr, &addButton.box); //Render add button

		//Render text
		SDL_FRect versStringRect = {0, 0, versStringTexture.w, versStringTexture.h};
		SDL_RenderTexture(renderer, versStringTexture.texture, nullptr, &versStringRect);

		std::string elemCountStr = std::format("Elems: {}", Board::getDraggableElems()->size());
		GFX::renderText(elemCountTexture, renderer, elemCountStr.c_str(), {255,255,255,255});
		SDL_FRect elemCountBox = {0, winHeight-elemCountTexture.h-12.0f, elemCountTexture.w, elemCountTexture.h};
		SDL_RenderTexture(renderer, elemCountTexture.texture, nullptr, &elemCountBox); //Draw element count

		std::string fpsStr = std::format("FPS: {:.2f}", 1000/deltaTime);
		GFX::renderText(fpsDisplayTexture, renderer, fpsStr.c_str(), {255,255,255,255});
		SDL_FRect fpsStringBox = {0, winHeight-fpsDisplayTexture.h, fpsDisplayTexture.w, fpsDisplayTexture.h};
		SDL_RenderTexture(renderer, fpsDisplayTexture.texture, nullptr, &fpsStringBox); //Draw fps display

		// Render every draggable element
		for (auto& d : *(Board::getDraggableElems())) {
			GfxResource elemNameTexture = GFX::getElemNameTexture(renderer, d->id);
			SDL_FRect elemTextBox = {
				(d->box.x + (d->box.w - elemNameTexture.w)/2), //Text X position
				d->box.y + d->box.h, //Text Y position
				elemNameTexture.w, elemNameTexture.h
			};
			if (!d->animQueueEmpty()) {
				d->parseAnimations(deltaTime);
				SDL_SetTextureAlphaModFloat(d->texture, d->opacity);
				SDL_FRect scaledRect = Anim::applyScale(d->box, d->scale); //Get scaled rect
				SDL_RenderTexture(renderer, d->texture, nullptr, &scaledRect);
				
				SDL_SetTextureAlphaModFloat(elemNameTexture.texture, d->opacity);
				elemTextBox = Anim::applyScale(elemTextBox, d->scale); //Get scaled text rect
				SDL_RenderTexture(renderer, elemNameTexture.texture, nullptr, &elemTextBox);
			} else {
				SDL_SetTextureAlphaModFloat(d->texture, d->opacity);
				SDL_RenderTexture(renderer, d->texture, NULL, &(d->box));
				SDL_RenderTexture(renderer, elemNameTexture.texture, nullptr, &elemTextBox);
			}
		}

		ImGui::Render(); //Render ImGui stuff
		SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 255);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);

		endTick = Clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(endTick - startTick).count() / 1000.0; //Get the time the frame took in ms
	}

	// Cleanup
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
