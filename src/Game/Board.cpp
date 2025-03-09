#include "Board.hpp"
#include "Element.hpp"
#include <algorithm>
#include <vector>
#include <memory>

std::vector<std::unique_ptr<DraggableElement>> draggableElems; //List of draggable elements on screen

void Board::spawnDraggable(int x, int y, std::string id) {
	draggableElems.push_back(std::make_unique<DraggableElement>(id, x, y)); //Add to list of elements
}

std::vector<std::unique_ptr<DraggableElement>>* Board::getDraggableElems() {
	return &draggableElems;
}

void Board::clearQueuedElements() {
	draggableElems.erase(std::remove_if(draggableElems.begin(), draggableElems.end(),
	[](const std::unique_ptr<DraggableElement> &d) { return d->queuedForDeletion; }
	), draggableElems.end());
}
