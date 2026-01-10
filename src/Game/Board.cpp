#include "Board.hpp"
#include "Element.hpp"
#include "GraphicsContext.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>
#include <memory>
#include <cmath>

bool boardFocused = true; // Is the board focused?
bool isDeleteQueued = false;
bool zSortNeeded = false;
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

void Board::deleteElem(DraggableElement* elem) {
	isDeleteQueued = true; // This is used to avoid unnecessary deletion checking
	elem->queuedForDeletion = true;
}
void Board::deleteSelectedElem() {
	isDeleteQueued = true;
	if (selectedElem != nullptr) {
		selectedElem->queuedForDeletion = true;
	}
}

void Board::queueZSort() {
	zSortNeeded = true;
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

void Board::spawnDraggable(int x, int y, int id) {
	int elemW = DraggableElement::getWidth(id);
	int elemH = DraggableElement::getHeight(id);

	int winWidth = GFX::getWindowWidth();
	int winHeight = GFX::getWindowHeight();

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

	draggableElems.push_back(std::make_unique<DraggableElement>(id, x, y)); // Add to list of elements
}

std::vector<std::unique_ptr<DraggableElement>>* Board::getDraggableElems() {
	return &draggableElems;
}

void Board::updateElems() {
	if (zSortNeeded) {
		auto draggableElems = Board::getDraggableElems();
		std::stable_sort(draggableElems->begin(), draggableElems->end(), []
		(const std::unique_ptr<DraggableElement> &d1, const std::unique_ptr<DraggableElement> &d2) {
			return d1->z > d2->z;
		});
		zSortNeeded = false;
	}

	if (isDeleteQueued) {
		draggableElems.erase(std::remove_if(draggableElems.begin(), draggableElems.end(),
		[](const std::unique_ptr<DraggableElement> &d) {
			return d->queuedForDeletion && d->animQueueEmpty(); // Elements can't be deleted until all animations finish
		}
		), draggableElems.end());
		isDeleteQueued = false; // Deletion no longer needed once all elements are deleted
	}
}
