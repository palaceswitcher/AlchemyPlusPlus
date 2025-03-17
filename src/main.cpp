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
#include <unordered_map>
#include <cmath>
#include "GameHandler.hpp"
#include "Element.hpp"
#include "Board.hpp"
#include "ElementMenu.hpp"
#include "Sprite.hpp"
#include "Animation.hpp"
#include "SDL_FontCache.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"); //Don't disable compositor.
	// SDL Init
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// Initialize window
	window = SDL_CreateWindow("Alchemy++ alpha v0.3", 64, 64, DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	} else {
		int imgFlags = IMG_INIT_PNG;
		if(!(IMG_Init(imgFlags) & imgFlags)) {
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		}
	}
	SDL_SetWindowMinimumSize(window, 640, 480);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
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
	SDL_Texture* addButtonTex = IMG_LoadTexture(renderer, addBtnDir.c_str());
	if (tex == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Game loop init
	std::vector<std::string> elementsUnlocked = {"air", "earth", "fire", "water"}; //Every element the user has unlocked
	SDL_Rect addButtonRect = {DEFAULT_WIDTH/2-32, DEFAULT_HEIGHT-80, 64, 64};
	Sprite addButton = {addButtonRect, addButtonTex, ANIM_NONE, 1.0f}; //Element add button

	// Spawn default elements
	Board::spawnDraggable(DEFAULT_WIDTH/2-16, DEFAULT_HEIGHT/2+24, Game::getElementNumId("air"));
	Board::spawnDraggable(DEFAULT_WIDTH/2-16, DEFAULT_HEIGHT/2-56, Game::getElementNumId("earth"));
	Board::spawnDraggable(DEFAULT_WIDTH/2-56, DEFAULT_HEIGHT/2-16, Game::getElementNumId("fire"));
	Board::spawnDraggable(DEFAULT_WIDTH/2+24, DEFAULT_HEIGHT/2-16, Game::getElementNumId("water"));

	DraggableElement* selectedElem = NULL; //Currently selected draggable

	bool leftClickDown = false; //Left click state, used to track drag and drop
	bool rightClickDown = false; //Middle click state
	long leftClickTick = 0; //The tick when left was clicked, used to detect double clicks
	bool addButtonClicked = false;
	SDL_Point mousePos; //Mouse position
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
						selectedElem->box.x = mousePos.x - clickOffset.x;
						selectedElem->box.y = mousePos.y - clickOffset.y; //Update rectangle position while its being dragged
					}
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					if (leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
						leftClickDown = false;

						if (selectedElem != NULL) {
							selectedElem->z++; //Move behind
							selectedElem->makeCombo(elementsUnlocked); //See if combination was made with another element
							deleteNeeded = selectedElem->queuedForDeletion = (selectedElem->box.x >= winWidth || selectedElem->box.y >= winHeight); //Delete element if it goes off-screen
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
						// Check if circle around the add button is clicked
						if (((mousePos.x-(addButton.box.x+32))*(mousePos.x-(addButton.box.x+32)) + (mousePos.y-(addButton.box.y+32))*(mousePos.y-(addButton.box.y+32))) < 32*32) {
							addButtonClicked = true;
							UI::openElementMenu();
						} else {
							for (auto& d : *(Board::getDraggableElems())) {
								if (!d->queuedForDeletion && SDL_PointInRect(&mousePos, &d->box)) {
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
						if (SDL_GetTicks64() > leftClickTick && SDL_GetTicks64() <= leftClickTick + 250) { //Double clicks have to be within 1/4 second of each other
							if (selectedElem == NULL) {
								Board::spawnDraggable(mousePos.x, mousePos.y+40, Game::getElementNumId("air"));
								Board::spawnDraggable(mousePos.x, mousePos.y-40, Game::getElementNumId("earth"));
								Board::spawnDraggable(mousePos.x-40, mousePos.y, Game::getElementNumId("fire"));
								Board::spawnDraggable(mousePos.x+40, mousePos.y, Game::getElementNumId("water"));
							} else {
								int hSpawnOffset = ((selectedElem->box.x + selectedElem->box.w/2) > mousePos.x) ? -40 : 40;
								int vSpawnOffset = ((selectedElem->box.y + selectedElem->box.h/2) > mousePos.y) ? -40 : 40; //Duplicate the element to the corner it was clicked
								Board::spawnDraggable(selectedElem->box.x+hSpawnOffset, selectedElem->box.y+vSpawnOffset, selectedElem->id); //Duplicate element if it's double clicked
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
			// Respond to window resize
			if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					int prevWinWidth = winWidth;
					int prevWinHeight = winHeight;
					SDL_GetWindowSize(window, &winWidth, &winHeight); //Get new screen size
					addButtonRect = {winWidth/2-32, winHeight-80, 64, 64}; //Position add button to the center of the screen
					// Adjust draggable elements relative to their previous position
					for (auto& d : *(Board::getDraggableElems())) {
						d->box.x = round(d->box.x * (double)winWidth/prevWinWidth);
						d->box.y = round(d->box.y * (double)winHeight/prevWinHeight);
					}
				}
			}
		}
		// Sort draggable elements by z index when animations finish
		if (zSortNeeded && !anim::animInProgress) {
			auto draggableElems = Board::getDraggableElems();
			std::stable_sort(draggableElems->begin(), draggableElems->end(), []
			(const std::unique_ptr<DraggableElement> &d1, const std::unique_ptr<DraggableElement> &d2) {
				return d1->z > d2->z;
			});
			zSortNeeded = false;
		}
		// Apply animations
		if (anim::animInProgress) {
			bool animDone = false;
			for (auto& d : *(Board::getDraggableElems())) {
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
			Board::clearQueuedElements();
			deleteNeeded = false;
		}

		SDL_RenderClear(renderer);

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		UI::renderElemMenu(elementsUnlocked);

		if (true) {
			SDL_RenderCopy(renderer, tex, NULL, NULL); //Render background
			SDL_RenderCopy(renderer, addButton.texture, NULL, &addButton.box); //Render add button
	
			//Render text
			FC_Draw(font, renderer, 0, 0, "Alchemy++ alpha v0.3"); //Version text
			FC_Draw(font, renderer, 20, winHeight-20, "elems: %d", Board::getDraggableElems()->size()); //Element count
	
			// Render every draggable element
			for (auto& d : *(Board::getDraggableElems())) {
				if ((int)d->scale != 1) {
					SDL_Rect scaledRect = anim::applyScale(d.get()); //Get scaled rect
					SDL_RenderCopy(renderer, d->texture, NULL, &scaledRect);
				} else {
					SDL_RenderCopy(renderer, d->texture, NULL, &(d->box));
				}
	
				std::string elemName = Game::getElementName(d->id);
				float textWidth = FC_GetWidth(font, elemName.c_str());
				float textHeight = FC_GetHeight(font, elemName.c_str());
				FC_DrawScale(font, renderer,
					(d->box.x + (d->box.w - textWidth)/2) + (textWidth - textWidth * d->scale)/2, //Text X position
					(d->box.y + d->box.h) + (textHeight - textHeight * d->scale)+2, //Text Y position
					{d->scale, d->scale}, elemName.c_str());
			}
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
