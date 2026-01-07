#ifndef BOARD_HPP_
#define BOARD_HPP_
#include "Element.hpp"
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

namespace Board {

void unfocus();
void focus();

// Returns true if board is focused
bool isFocused();

// Deselects the currently selected element
void deselectElem();
// Returns true if an element is currently selected
bool elemSelected();
// Returns the currently selected element
DraggableElement* getSelectedElem();
// Selects the passed element
void selectElem(DraggableElement* elem);

/**
 * Spawns a draggable on the board
 * @param x X position
 * @param y Y position
 * @param id Element id
 */
void spawnDraggable(SDL_Renderer* ren, int x, int y, int id);

std::vector<std::unique_ptr<DraggableElement>>* getDraggableElems();

void clearQueuedElements(bool& deleteUnfinished);
}

#endif
