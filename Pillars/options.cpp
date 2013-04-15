#include "options.h"
#include "func.h"
#include "game.h"
#include "AudioPlayer.h"

static int selectedItem = 0;
static SDL_Surface* screen;

void DrawOptions(SDL_Surface* s)
{
	screen = s;
	SDL_FillRect(s, &s->clip_rect, 0);

	int xx = (s->w - 161) >> 1;
	DrawString(s, "Options", xx, 80, true);

	//change the color of the options logo
	for (int x = 0; x < 161; x++)
		for (int y = 0; y < 24; y++)
		{
			int n = (y + 80) * s->pitch / szInt + (x + xx);
			if (((unsigned int*)s->pixels)[n] != 0)
				((unsigned int*)s->pixels)[n] = 0xffffff - ((unsigned int*)s->pixels)[n];
		}
	
	if (data::isAudioPlaying)
		DrawString(s, "Playing audio", (s->w - 299) >> 1, 200, true);
	else
		DrawString(s, "Muted audio", (s->w - 253) >> 1, 200, true);;

	if (data::isFullscreen)
		DrawString(s, "fullscreen", (s->w - 230) >> 1, 270, true);
	else
		DrawString(s, "windowed", (s->w - 184) >> 1, 270, true);

	DrawString(s, "Back", (s->w - 92) >> 1, 340, true);

	if (selectedItem > -1) //draw boxes around the selected item
		DrawString(s, "#              #", (s->w - 368) >> 1, 200 + selectedItem * 70, true);
}

void UpdateOptions(long time)
{
	if (data::input.keys[SDLK_ESCAPE])
	{
		data::input.keys[SDLK_ESCAPE] = false;
		selectedItem = 2;
		SelectItemOptions();
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
		SelectItemOptions();
	}
	else if (data::input.keys[SDLK_KP_ENTER])
	{
		data::input.keys[SDLK_KP_ENTER] = false;
		SelectItemOptions();
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
		SelectItemOptions();
	}
}

void SelectItemOptions()
{
	if (selectedItem == 0)
	{
		data::isAudioPlaying = !data::isAudioPlaying;
		AudioPlayer::Stop();
	}
	else if (selectedItem == 1)
	{
		data::isFullscreen = !data::isFullscreen;
		screen = SDL_SetVideoMode(0, 0, 0, screen->flags ^ SDL_FULLSCREEN);
	}
	else if (selectedItem == 2)
	{
		if (data::isGamePlaying)
			data::currentState = GS_Pause;
		else
		{
			data::currentState = GS_Menu;
			data::timer = -1000;
		}
		selectedItem = 0;
	}

	data::input.mPressed = false;
}