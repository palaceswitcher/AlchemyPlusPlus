#ifndef PROGRESS_HPP_
#define PROGRESS_HPP_
#include <set>

namespace Progress {
// Unlock an element with a numerical id
void UnlockElement(int id);
std::set<int> GetUnlockedElements();
};

#endif
