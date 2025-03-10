#ifndef BOARD_HPP_
#define BOARD_HPP_
#include "Element.hpp"

namespace Board {

/**
 * Spawns a draggable on the board
 * @param x X position
 * @param y Y position
 * @param id Element id
 */
void spawnDraggable(int x, int y, std::string id);

std::vector<std::unique_ptr<DraggableElement>>* getDraggableElems();

void clearQueuedElements();
}

#endif
