#include "Button.hpp"

bool Button::awaitingResponse() {
	return animQueue.empty() && wasClicked;
}
