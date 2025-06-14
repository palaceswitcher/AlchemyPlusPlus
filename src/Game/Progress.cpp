#include "Progress.hpp"
#include "GameHandler.hpp"
#include <set>
#include <string>

std::set<int> unlockedElementIds;

std::set<int> Progress::GetUnlockedElements() {
	return unlockedElementIds;
}

void Progress::UnlockElement(int id) {
	unlockedElementIds.insert(id);
}
