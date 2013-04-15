#include "InputHandler.h"

InputHandler::InputHandler()
{
	mx = -16; //size of cursor is 16x16, hide cursor by default
	my = -16;
	mPressed = false;
	mMoved = false;
}

void InputHandler::Clear()
{
	mx = -16;
	my = -16;
	mPressed = false;
	mMoved = false;

	keysPressed = 0;
	memset((void*)keys, 0, sizeof(bool) * 323);
}

bool InputHandler::Update()
{
	bool cont = true;
	mMoved = false;

	while (SDL_PollEvent(&ev))
	{
		if (ev.type == SDL_QUIT)
		{
			cont = false;
			break;
		}

		if (ev.type == SDL_MOUSEBUTTONDOWN)
			mPressed = true;
		else if (ev.type == SDL_MOUSEBUTTONUP)
			mPressed = false;

		if (ev.type == SDL_MOUSEMOTION)
		{
			mx = ev.motion.x;
			my = ev.motion.y;
			mMoved = true;
		}

		if (ev.type == SDL_KEYDOWN)
		{
			keys[ev.key.keysym.sym] = true;
			keysPressed++;

			ctrlPressed = (ev.key.keysym.mod & KMOD_CTRL) > 0;
			altPressed = (ev.key.keysym.mod & KMOD_ALT) > 0;
			shftPressed = (ev.key.keysym.mod & KMOD_SHIFT) > 0;
		}
		else if (ev.type == SDL_KEYUP)
		{
			if (ev.key.keysym.mod & KMOD_ALT && ev.key.keysym.sym == SDLK_F4) //alt+f4
			{
				cont = false;
				break;
			}
			else
				keys[ev.key.keysym.sym] = false;

			keysPressed--;

			if (keysPressed < 0)
				keysPressed = 0;
			
			ctrlPressed = (ev.key.keysym.mod & KMOD_CTRL) > 0;
			altPressed = (ev.key.keysym.mod & KMOD_ALT) > 0;
			shftPressed = (ev.key.keysym.mod & KMOD_SHIFT) > 0;
		}
	}

	return cont;
}

bool InputHandler::IsKeyPressed()
{
	return keysPressed > 0;
}