#include "Board.hpp"
#include "Element.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <cmath>

bool boardFocused = true; // Is the board focused?
std::vector<std::unique_ptr<DraggableElement>> draggableElems; // List of draggable elements on screen
DraggableElement* selectedElem;

void Board::deselectElem() {
	selectedElem = nullptr;
}
bool Board::elemSelected() {
	return selectedElem != nullptr;
}
DraggableElement* Board::getSelectedElem() {
	return selectedElem;
}
void Board::selectElem(DraggableElement* elem) {
	selectedElem = elem;
}

void Board::unfocus() {
	boardFocused = false;
}

void Board::focus() {
	boardFocused = true;
}

bool Board::isFocused() {
	return boardFocused;
}

void Board::spawnDraggable(SDL_Renderer* ren, int x, int y, int id) {
	int elemW = DraggableElement::getWidth(ren, id);
	int elemH = DraggableElement::getHeight(ren, id);

	SDL_Window* win = SDL_GetRenderWindow(ren);
	int winWidth, winHeight;
	SDL_GetWindowSize(win, &winWidth, &winHeight); // Get window dimensions

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
