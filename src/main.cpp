#define SDL2_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <memory>
#include "Game/Element.hpp"
#include "GFX/Animation.hpp"
#include "GFX/SDL_FontCache.h"
#include "Menu/Lang.hpp"
typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); //Always use OpenGL
	SDL_GLContext glContext;
	SDL_Event e;

	// SDL Init
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	// Initialize window
	SDL_Window* win = SDL_CreateWindow("Alchemy++ alpha v0.1.2", 64, 64, 800, 600, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	if (win == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	} else {
		int imgFlags = IMG_INIT_PNG;
		if(!(IMG_Init(imgFlags) & imgFlags)) {
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		}
	}
	glContext = SDL_GL_CreateContext(win);

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		if (win != NULL) {
			SDL_DestroyWindow(win);
		}
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Texture* tex = IMG_LoadTexture(ren, "gamedata/default/textures/backgrounds/emptyuniverse.png");
	SDL_Texture* addBtn = IMG_LoadTexture(ren, "gamedata/default/textures/buttons/addBtn.png");
	if (tex == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	// Initialize font
	TTF_Init();
	FC_Font* font = FC_CreateFont();
	FC_LoadFont(font, ren, "gamedata/default/font/Droid-Sans.ttf", 12, FC_MakeColor(255,255,255,255), TTF_STYLE_NORMAL);

	elem::JSONInit(); //Initialize JSON
	Text::loadAll("en-us"); //Load game text

	// Game loop init
	std::vector<std::string> elemsUnlocked = {"air", "earth", "fire", "water"};
	std::vector<std::unique_ptr<DraggableElement>> draggables; //List of draggable elements on screen
	std::vector<std::string> elementsUnlocked; //Every element the user has unlocked

	elem::spawnDraggable(draggables, 288, 208, "air");
	elem::spawnDraggable(draggables, 50, 50, "fire");
	DraggableElement* selectedElem = NULL; //Currently selected draggable

	bool leftClickDown = false; //Left click state, used to track drag and drop
	long leftClickTick = 0; //The tick when left was clicked, used to detect double clicks
	bool rightClickDown = false; //Middle click state
	SDL_Point mousePos; //Mouse position
	SDL_Point clickOffset; //Point in the element box clicked relative to its boundary
	int winWidth, winHeight; //Window size

	auto endTick = Clock::now();
	double deltaTime = 1.0/60;
	bool zSortNeeded = false; //Used to indicate if elements need to be sorted
	bool quit = false; //Main loop exit flag
	while (!quit) {
		SDL_GetWindowSize(win, &winWidth, &winHeight); //Get screen size
		auto startTick = Clock::now();

		//deltaTime = endTick-startTick;

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break; //Close the program if X is clicked
				case SDL_MOUSEMOTION:
					mousePos = {e.motion.x, e.motion.y}; //Get mouse position
					if (leftClickDown && selectedElem != NULL) {
						selectedElem->box->x = mousePos.x - clickOffset.x;
						selectedElem->box->y = mousePos.y - clickOffset.y; //Update rectangle position while its being dragged
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
						leftClickDown = false;

						if (selectedElem != NULL) {
							selectedElem->z++; //Move behind
							selectedElem->makeCombo(draggables, elementsUnlocked); //See if combination was made with another element
						}
						selectedElem = NULL; //Release selected rectangle when left is released
						zSortNeeded = true; //Re-sort Z-index after element was dropped
					}
					if (rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
						rightClickDown = false;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (!leftClickDown && e.button.button == SDL_BUTTON_LEFT) {
						leftClickDown = true;

						std::vector<DraggableElement*> clickMatches; //Every element that the mouse clicked on
						if (selectedElem == NULL) {
							for (auto &d : draggables) {
								if (SDL_PointInRect(&mousePos, d->box)) {
									clickMatches.push_back(d.get());
								}
							}
							if (!clickMatches.empty()) {
								zSortNeeded = true; //Z-index will need to be resorted as this element will be moved to the front
								std::stable_sort(clickMatches.begin(), clickMatches.end(), compareZIndexRaw); //Sort matching elements by Z-index, with the highest coming last

								selectedElem = clickMatches.back(); //Select rectangle with highest Z-index
								selectedElem->z = 0; //Move to front of z-index
								clickOffset.x = mousePos.x - selectedElem->box->x;
								clickOffset.y = mousePos.y - selectedElem->box->y; //Get clicked point in element box relative to its boundary
							}
						} else {
							clickOffset.x = mousePos.x - selectedElem->box->x;
							clickOffset.y = mousePos.y - selectedElem->box->y; //Don't look for a new element to select if one is already selected
						}
						//Spawn new elements on double click
						if (SDL_GetTicks64() > leftClickTick && SDL_GetTicks64() <= leftClickTick + 250) { //Double clicks have to be within 3/8 second of each other
							elem::spawnDraggable(draggables, mousePos.x, mousePos.y+32, "air");
							elem::spawnDraggable(draggables, mousePos.x, mousePos.y-32, "earth");
							elem::spawnDraggable(draggables, mousePos.x-32, mousePos.y, "fire");
							elem::spawnDraggable(draggables, mousePos.x+32, mousePos.y, "water");
						}
						leftClickTick = SDL_GetTicks64(); //Get next click tick
					}
					if (!rightClickDown && e.button.button == SDL_BUTTON_RIGHT) {
						rightClickDown = true;
						if (selectedElem != NULL) {
							std::cout << "Selected: " << selectedElem->id << std::endl;
						}
					}
					break;
			}
		}
		// Sort draggable elements by z index when animations finish
		if (zSortNeeded && !anim::animInProgress) {
			std::stable_sort(draggables.begin(), draggables.end(), [](const std::unique_ptr<DraggableElement> &d1, const std::unique_ptr<DraggableElement> &d2) {
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
				} else if (d->anim == ANIM_GROW) {
					anim::animateGrow(d.get(), deltaTime);
					animDone = true;
				}
				anim::animInProgress = animDone; //Stop animations if none are running
			}
			// Remove combined elements when they finish shrinking
			if (elem::firstParentElem != NULL && elem::secondParentElem != NULL && elem::firstParentElem->anim == ANIM_SHRINK_END && elem::secondParentElem->anim == ANIM_SHRINK_END) {
				//elem::firstParentElem->queuedForDeletion = true;
				//elem::secondParentElem->queuedForDeletion = true;
				DraggableElement::deleteElem(draggables, elem::firstParentElem);
				DraggableElement::deleteElem(draggables, elem::secondParentElem); //Delete combo elements when they finish shrinking
				elem::firstParentElem = NULL;
				elem::secondParentElem = NULL;
			}
		}
		// Load textures
		for (auto &d : draggables) {
			elem::loadTexture(ren, d.get());
		}
		SDL_RenderClear(ren);

		SDL_RenderCopy(ren, tex, NULL, NULL); //Render background
		SDL_Rect r{winWidth/2-32, winHeight-80, 64, 64};
		SDL_RenderCopy(ren, addBtn, NULL, &r); //Render add button

		//Render text
		FC_Draw(font, ren, 0, 0, "Alchemy++ alpha v0.1.2");

		FC_Draw(font, ren, 20, winHeight-20, "elems: %d", draggables.size());

		// Render every draggable element
		for (auto const& d : draggables) {
			if ((int)d->scale != 1) {
				SDL_Rect* scaledRect = anim::applyScale(d.get()); //Get scaled rect
				SDL_RenderCopy(ren, d->texture, NULL, scaledRect);
				free(scaledRect); //Free it to avoid leak
			} else {
				SDL_RenderCopy(ren, d->texture, NULL, d->box);
			}

			float textWidth = FC_GetWidth(font, d->name.c_str());
			float textHeight = FC_GetHeight(font, d->name.c_str());
			FC_DrawScale(font, ren,
				(d->box->x + (d->box->w - textWidth)/2) + (textWidth - textWidth * d->scale)/2, //Text X position
				(d->box->y + d->box->w) + (textHeight - textHeight * d->scale), //Text Y position
				{d->scale, d->scale}, d->name.c_str());
		}

		FC_Draw(font, ren, winWidth-170, 10, "FPS: %f", 1000/deltaTime);

		SDL_RenderPresent(ren);
		endTick = Clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(endTick - startTick).count() / 1000.0; //Get the time the frame took in ms
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	printf("\n"); //Break after debug messages

	return EXIT_SUCCESS;
}
