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
#include "Rendering.hpp"
#include "RenderDefs.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <cmath>
#include "Input.hpp"
#include "Text.hpp"
#include "GameHandler.hpp"
#include "Element.hpp"
#include "Board.hpp"
#include "Button.hpp"
#include "ElementMenu.hpp"
#include "Animation.hpp"
#include "GraphicsContext.hpp"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char* argv[]) {
	SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"); // Don't disable compositor.
	// SDL Init
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}
	// Initialize window
	Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	GFX::window = SDL_CreateWindow("Alchemy++ alpha v0.4", DEFAULT_WIDTH, DEFAULT_HEIGHT, window_flags);
	if (GFX::window == nullptr) {
		std::cerr << stderr << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
		return -1;
	}
	SDL_SetWindowMinimumSize(GFX::window, 640, 480);

	GFX::renderer = SDL_CreateRenderer(GFX::window, nullptr);
	SDL_SetRenderVSync(GFX::renderer, 1); // VSYNC (TODO: MAKE THIS AN OPTION)
	if (GFX::renderer == nullptr) {
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());;
		return -1;
	}
	SDL_SetWindowPosition(GFX::window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(GFX::window);

	TTF_Init();
	// Load default game
	if (!Game::loadGameData("default")) {
		std::cerr << "ERROR: Game data \"" << "default" << "\" is missing or malformed.\n";
	}

	// Set up ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	ImGui::StyleColorsDark(); // Use dark mode by default
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f); // Center title bars
	ImGui_ImplSDL3_InitForSDLRenderer(GFX::window, GFX::renderer);
	ImGui_ImplSDLRenderer3_Init(GFX::renderer); // Init for SDL renderer

	// Load fonts
	std::string fontPath = Game::getFontDir() + "Open-Sans.ttf"; // Get font directory
	ImFont* guiFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f); // Load font in ImGui

	std::string texDir = Game::getTextureDir() + "backgrounds/emptyuniverse.png";
	SDL_Texture* tex = IMG_LoadTexture(GFX::renderer, texDir.c_str());
	std::string addBtnDir = Game::getTextureDir() + "buttons/addBtn.png";
	SDL_Texture* addButtonTex = IMG_LoadTexture(GFX::renderer, addBtnDir.c_str());
	if (tex == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(GFX::renderer);
		SDL_DestroyWindow(GFX::window);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	GfxResource fpsDisplayTexture;
	GfxResource versStringTexture;
	GFX::renderTextToRes(versStringTexture, "Alchemy++ alpha v0.4", {255,255,255,255});

	// Game loop init
	SDL_FRect addButtonRect = {DEFAULT_WIDTH/2-32, DEFAULT_HEIGHT-80, 64, 64};
	Button addButton = Button(addButtonRect, addButtonTex); // Element add button

	// Spawn default elements
	Board::spawnDraggable(DEFAULT_WIDTH/2-16, DEFAULT_HEIGHT/2+24, Game::getElementNumId("air"));
	Board::spawnDraggable(DEFAULT_WIDTH/2-16, DEFAULT_HEIGHT/2-56, Game::getElementNumId("earth"));
	Board::spawnDraggable(DEFAULT_WIDTH/2-56, DEFAULT_HEIGHT/2-16, Game::getElementNumId("fire"));
	Board::spawnDraggable(DEFAULT_WIDTH/2+24, DEFAULT_HEIGHT/2-16, Game::getElementNumId("water"));

	DraggableElement* infoElem = NULL; // Currently selected draggable

	int winWidth, winHeight; // Window size

	auto endTick = Clock::now();
	double deltaTime = 1.0/60;
	// Game flags
	bool addButtonClicked = false;
	bool settingsButtonClicked = false;
	bool quit = false; // Main loop exit flag
	while (!quit) {
		auto startTick = Clock::now(); // Get start tick
		SDL_GetWindowSize(GFX::window, &winWidth, &winHeight); // Get new screen size

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL3_ProcessEvent(&e);
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
				break; // Close the program if X is clicked
			}
			handleInput(e);
		}

		Board::updateElems();

		SDL_RenderClear(GFX::renderer);

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		if (addButton.awaitingResponse()) {
			addButton.wasClicked = false;
			Board::unfocus();
			UI::openElementMenu();
		}
		UI::renderElemMenu();

		SDL_RenderTexture(GFX::renderer, tex, nullptr, nullptr); // Render background

		GFX::renderSprite(deltaTime, &addButton); // Render add button

		// Render text
		SDL_FRect versStringRect = {0, 0, versStringTexture.w, versStringTexture.h};
		SDL_RenderTexture(GFX::renderer, versStringTexture.texture, nullptr, &versStringRect);

		std::string elemCountStr = std::format("Elems: {}", Board::getDraggableElems()->size());
		GFX::renderText(elemCountStr, winWidth, winHeight, {255,255,255,255}, 1.0f, ORIGIN_BOTTOM|ORIGIN_RIGHT);

		std::string fpsStr = std::format("FPS: {:.2f}", 1000/deltaTime);
		GFX::renderTextToRes(fpsDisplayTexture, fpsStr.c_str(), {255,255,255,255});
		SDL_FRect fpsStringBox = {0, winHeight-fpsDisplayTexture.h, fpsDisplayTexture.w, fpsDisplayTexture.h};
		SDL_RenderTexture(GFX::renderer, fpsDisplayTexture.texture, nullptr, &fpsStringBox); // Draw fps display

		// Render every draggable element
		for (auto& d : *(Board::getDraggableElems())) {
			GFX::renderSprite(deltaTime, d.get());

			GfxResource elemNameTexture = GFX::getElemNameTexture(d->id);
			SDL_SetTextureAlphaModFloat(elemNameTexture.texture, d->opacity);
			SDL_FRect elemTextBox = {
				roundf(d->box.x + (d->box.w - elemNameTexture.w)/2), // Text X position
				roundf(d->box.y + d->box.h), // Text Y position
				elemNameTexture.w, elemNameTexture.h
			};

			// Get scaled text rect
			if (d->scale != 1.0) {
				int scaledW = (elemTextBox.w * d->scale);
				int scaledH = (elemTextBox.h * d->scale);
				elemTextBox.x = roundf(elemTextBox.x + ((elemTextBox.w - scaledW) / 2));
				elemTextBox.y = roundf(elemTextBox.y + ((elemTextBox.h - scaledH) / 2)); // Scale relative to its center
				elemTextBox.w = scaledW;
				elemTextBox.h = scaledH;
			}

			SDL_RenderTexture(GFX::renderer, elemNameTexture.texture, nullptr, &elemTextBox);
		}

		ImGui::Render(); // Render ImGui stuff
		SDL_SetRenderDrawColorFloat(GFX::renderer, 0, 0, 0, 255);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), GFX::renderer);
		SDL_RenderPresent(GFX::renderer);
		SDL_RenderClear(GFX::renderer);

		endTick = Clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(endTick - startTick).count() / 1000.0; // Get the time the frame took in ms
	}

	// Cleanup
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(GFX::renderer);
	SDL_DestroyWindow(GFX::window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
