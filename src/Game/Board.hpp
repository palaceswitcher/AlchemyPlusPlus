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
// Selects an element at a specified position, returns true if an element exists at that position
bool selectElem(SDL_FPoint mousePos);

// Deletes an element
void deleteElem(DraggableElement* elem);
// Deletes the currently selected element
void deleteSelectedElem();

// Moves an element anchored to the cursor
void moveElemCursor(DraggableElement* elem, float x, float y);

// Signal that a z-sort is needed
void queueZSort();

/**
 * Spawns a draggable on the board
 * @param x X position
 * @param y Y position
 * @param id Element id
 */
void spawnDraggable(int x, int y, int id);

std::vector<std::unique_ptr<DraggableElement>>* getDraggableElems();

// Handles any pending changes needed for elements, such as z-sorting them or deleting elements that need to be deleted
void updateElems();
}

#endif
