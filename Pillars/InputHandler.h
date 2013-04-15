#pragma once

#include <SDL/SDL.h>

struct InputHandler
{
	int mx, my;
	bool mPressed;
	bool mMoved;

	bool keys[323];
	bool altPressed;
	bool ctrlPressed;
	bool shftPressed;

	InputHandler();

	bool Update();
	bool IsKeyPressed();
	void Clear();

private:
	SDL_Event ev;
	int keysPressed;
};

