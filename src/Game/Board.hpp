#ifndef BOARD_HPP_
#define BOARD_HPP_
#include "Element.hpp"
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

namespace Board {

/**
 * Spawns a draggable on the board
 * @param x X position
 * @param y Y position
 * @param id Element id
 */
void spawnDraggable(SDL_Renderer* ren, int x, int y, int id);

std::vector<std::unique_ptr<DraggableElement>>* getDraggableElems();

void clearQueuedElements();
}

#endif
