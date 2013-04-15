#include "pause.h"
#include "func.h"
#include "game.h"
#include "AudioPlayer.h"

static int selectedItem = 0;

void DrawPause(SDL_Surface* s)
{
	SDL_FillRect(s, &s->clip_rect, 0);

	int xx = (s->w - 138) >> 1;
	DrawString(s, "Paused", xx, 80, true);

	//change the color of the paused logo
	for (int x = 0; x < 138; x++)
		for (int y = 0; y < 24; y++)
		{
			int n = (y + 80) * s->pitch / szInt + (x + xx);
			if (((unsigned int*)s->pixels)[n] != 0)
				((unsigned int*)s->pixels)[n] = 0x6f | ((unsigned int*)s->pixels)[n];
		}
	
	DrawString(s, "Resume", (s->w - 138) >> 1, 200, true);
	DrawString(s, "Options", (s->w - 161) >> 1, 270, true);
	DrawString(s, "Quit", (s->w - 92) >> 1, 340, true);

	if (selectedItem > -1) //draw boxes around the selected item
		DrawString(s, "#          #", (s->w - 276) >> 1, 200 + selectedItem * 70, true);
}

void UpdatePause(long time)
{
	if (data::input.keys[SDLK_ESCAPE])
	{
		data::input.keys[SDLK_ESCAPE] = false;
		data::currentState = GS_Play;
	}

	else if (data::input.keys[SDLK_DOWN])
	{
		data::input.keys[SDLK_DOWN] = false;
		++selectedItem %= 3;
	}
	else if (data::input.keys[SDLK_UP])
	{
		data::input.keys[SDLK_UP] = false;
		selectedItem--;
		if (selectedItem < 0)
			selectedItem = 2;
	}
	else if (data::input.keys[SDLK_RETURN])
	{
		data::input.keys[SDLK_RETURN] = false;
		SelectItemPause();
	}
	else if (data::input.keys[SDLK_KP_ENTER])
	{
		data::input.keys[SDLK_KP_ENTER] = false;
		SelectItemPause();
	}

	if (data::input.mMoved || data::input.mPressed)
	{
		if (data::input.my > 165 && data::input.my < 385)
			selectedItem = (data::input.my - 175) / 70;
		else
			selectedItem = -1;
	}

	if (data::input.mPressed)
	{
		data::input.mPressed = false;
		SelectItemPause();
	}
}

void SelectItemPause()
{
	if (selectedItem == 0)
		data::currentState = GS_Play;
	else if (selectedItem == 1)
	{
		data::currentState = GS_Options;
	}
	else if (selectedItem == 2)
	{
		data::currentState = GS_Menu;
		data::isGamePlaying = false;
		data::timer = -1000;
		AudioPlayer::Stop();
	}

	data::input.mPressed = false;
	selectedItem = 0;
}