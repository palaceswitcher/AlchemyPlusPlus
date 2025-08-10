#include "Board.hpp"
#include "Element.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>
#include <memory>

bool boardFocused = true; // Is the board focused?
std::vector<std::unique_ptr<DraggableElement>> draggableElems; // List of draggable elements on screen

void Board::unfocus() {
	boardFocused = false;
}

void Board::focus() {
	boardFocused = true;
}

bool Board::isFocused() {
	return boardFocused;
}

void Board::spawnDraggable(SDL_Renderer* ren, int x, int y, int id, bool allowOverlap) {
	int elemW = DraggableElement::getWidth(ren, id);
	int elemH = DraggableElement::getHeight(ren, id);

	SDL_Window* win = SDL_GetRenderWindow(ren);
	int winWidth, winHeight;
	SDL_GetWindowSize(win, &winWidth, &winHeight); // Get window dimensions

	if (!allowOverlap) {
		for (auto &d : draggableElems) {
			if (!d->queuedForDeletion) {
				 // Spawn next to either left or right halves
				if (x + elemW >= d->box.x && x <= d->box.x + d->box.w &&
					y + elemH >= d->box.y && y <= d->box.y + d->box.h) {
					int x1Center = x + elemW/2;
					int y1Center = y + elemH/2; // Center of spawned element
					int x2Center = d->box.x + d->box.w/2;
					int y2Center = d->box.y + d->box.h/2; // Center of colliding element
					int xDist = x2Center - x1Center;
					int yDist = y2Center - y1Center;
					if (abs(xDist) > abs(yDist)) {
						if (xDist < 0) {
							x = d->box.x - elemW * 1.5f;
						} else {
							x = d->box.x + elemW * 1.5f;
						}
					} else {
						if (yDist < 0) {
							y = d->box.y - elemH * 1.5f;
						} else {
							y = d->box.y + elemH * 1.5f;
						}
					}
				}
			}
		}
	}

	if (x + elemW >= winWidth) {
		x = winWidth - elemW;
	} else if (x < 0) {
		x = 0;
	}
	if (y + elemH >= winHeight) {
		y = winHeight - elemH;
	} else if (y < 0) {
		y = 0;
	}

	draggableElems.push_back(std::make_unique<DraggableElement>(ren, id, x, y)); // Add to list of elements
}

std::vector<std::unique_ptr<DraggableElement>>* Board::getDraggableElems() {
	return &draggableElems;
}

void Board::clearQueuedElements(bool& deleteUnfinished) {
	draggableElems.erase(std::remove_if(draggableElems.begin(), draggableElems.end(),
	[&deleteUnfinished](const std::unique_ptr<DraggableElement> &d) {
		bool wasDeleted = d->queuedForDeletion && d->animQueueEmpty();
		if (deleteUnfinished && wasDeleted) {
			deleteUnfinished = false;
		}
		return d->queuedForDeletion && d->animQueueEmpty(); // Elements can't be deleted until all animations finish
	}
	), draggableElems.end());
}
