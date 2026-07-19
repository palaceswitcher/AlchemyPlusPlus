#include "Board.hpp"
#include "Element.hpp"
#include "GraphicsContext.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>
#include <memory>

bool boardFocused = true; // Is the board focused?
bool isDeleteQueued = false;
bool zSortNeeded = false;
std::vector<std::unique_ptr<DraggableElement>> draggableElems; // List of draggable elements on screen
float selectedElemAnchorX;
float selectedElemAnchorY;
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
bool Board::selectElem(SDL_FPoint mousePos) {
	std::vector<DraggableElement*> clickMatches; // Every element that the cursor clicked on
	if (!elemSelected()) {
		for (auto& d : *(getDraggableElems())) {
			if (!d->queuedForDeletion && SDL_PointInRectFloat(&mousePos, &d->box)) {
				clickMatches.push_back(d.get());
			}
		}

		if (!clickMatches.empty()) {
			zSortNeeded = true;
			std::stable_sort(clickMatches.begin(), clickMatches.end(), [](DraggableElement* d1, DraggableElement* d2) {
				return d1->z > d2->z;
			});

			selectedElem = clickMatches.back();
			selectedElem->z = 0;
			selectedElemAnchorX = mousePos.x - selectedElem->box.x;
			selectedElemAnchorY = mousePos.y - selectedElem->box.y;
			return true;
		}
	}
	return false;
}

void Board::deleteElem(DraggableElement* elem) {
	elem->addAnim(ANIM_SCALE, 0.0f, 0.25f); // Despawn animation
	isDeleteQueued = true; // This is used to avoid unnecessary deletion checking
	elem->queuedForDeletion = true;
}
void Board::deleteSelectedElem() {
	deleteElem(selectedElem);
	deselectElem();
}

void Board::moveElemCursor(DraggableElement* elem, float x, float y) {
	Board::getSelectedElem()->box.x = x - selectedElemAnchorX;
	Board::getSelectedElem()->box.y = y - selectedElemAnchorY;
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
			bool canDelete = d->queuedForDeletion && d->animQueueEmpty(); // Elements can't be deleted until all animations finish

			isDeleteQueued ^= canDelete; // We don't need to check for queued elements once we know they're deleted
			return canDelete;
		}
		), draggableElems.end());
	}
}
