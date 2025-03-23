#include "Board.hpp"
#include "Element.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>
#include <memory>

std::vector<std::unique_ptr<DraggableElement>> draggableElems; //List of draggable elements on screen

void Board::spawnDraggable(SDL_Renderer* ren, int x, int y, int id) {
	draggableElems.push_back(std::make_unique<DraggableElement>(ren, id, x, y)); //Add to list of elements
}

std::vector<std::unique_ptr<DraggableElement>>* Board::getDraggableElems() {
	return &draggableElems;
}

void Board::clearQueuedElements() {
	draggableElems.erase(std::remove_if(draggableElems.begin(), draggableElems.end(),
	[](const std::unique_ptr<DraggableElement> &d) { return d->queuedForDeletion; }
	), draggableElems.end());
}
