#ifndef BUTTON_HPP_
#define BUTTON_HPP_
#include <SDL3/SDL.h>
#include <functional>
#include "Sprite.hpp"

class Button : public Sprite {
public:
	bool wasClicked = false; // Whether or not a button was clicked on
	bool awaitingResponse();
	//std::function<void()> onClick; // Function when clicked
	using Sprite::Sprite;
};

#endif
