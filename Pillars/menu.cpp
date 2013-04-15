#include "menu.h"
#include "func.h"
#include "game.h"
#include "logo.h"
#include <cmath>

static int selectedItem = 0;

void DrawMenu(SDL_Surface* s)
{
	SDL_FillRect(s, &s->clip_rect, 0);

	long time = SDL_GetTicks();
	long tdif = time - data::timer;

	int max = (long)(((s->h - logo_h) >> 1) - 10) * 5;
	if (tdif < max)
	{
		DrawString(s, "Dejitaru Forge", (s->w - 325) >> 1, 80 - (tdif >> 2), true);
		int xx = (s->w - 200) >> 1;
		DrawString(s, "Presents", xx, 120 - (tdif >> 2), true);

		for (int x = 0; x < 200; x++)
			for (int y = 0; y < 24; y++)
			{
				int n = (y + 120 - (tdif >> 2)) * s->pitch / szInt + (x + xx);
				if (n > 0 && ((unsigned int*)s->pixels)[n] != 0)
					((unsigned int*)s->pixels)[n] = 0xffffaa - ((unsigned int*)s->pixels)[n];
			}

		DrawImg(s, logo_img, logo_pal, (s->w - logo_w) >> 1, ((s->h - logo_h) >> 1) - tdif / 5, 0, 0, logo_w, logo_h, logo_w);

		int n = max - tdif;
		if (tdif > n >> 2)
			DrawString(s, "credits", (s->w - 161) >> 1, 340, true);
		if (tdif > n >> 1)
			DrawString(s, "Options", (s->w - 161) >> 1, 270, true);
		if (tdif > n)
			DrawString(s, "Play", (s->w - 92) >> 1, 200, true);
		DrawString(s, "Exit", (s->w - 92) >> 1, 410, true);
	}
	else
	{
		DrawImg(s, logo_img, logo_pal, (s->w - logo_w) >> 1, 10, 0, 0, logo_w, logo_h, logo_w);

		//draw pulses
		int start = (time / 10) % s->w;
		for (int i = 0; i < 240; i++)
		{
			int c = (int)(sin((float)i / 80.f) * 255);
			int n = (i + start) % s->w;
			DrawDot(s, SDL_MapRGB(s->format, 0, c >> 3, c >> 1), n, 19 + logo_h);
			DrawDot(s, SDL_MapRGB(s->format, 0, c >> 2, c), n + 1, 20 + logo_h);
			DrawDot(s, SDL_MapRGB(s->format, 0, c >> 3, c >> 1), n, 21 + logo_h);

			n = s->w - n;
			DrawDot(s, SDL_MapRGB(s->format, 0, c >> 3, c >> 1), n, 25 + logo_h);
			DrawDot(s, SDL_MapRGB(s->format, 0, c >> 2, c), n - 1, 26 + logo_h);
			DrawDot(s, SDL_MapRGB(s->format, 0, c >> 3, c >> 1), n, 27 + logo_h);
		}

		//draw menu options
		DrawString(s, "Play", (s->w - 92) >> 1, 200, true);
		DrawString(s, "Options", (s->w - 161) >> 1, 270, true);
		DrawString(s, "credits", (s->w - 161) >> 1, 340, true);
		DrawString(s, "Exit", (s->w - 92) >> 1, 410, true);

		if (selectedItem > -1) //draw boxes around the selected item
			DrawString(s, "#          #", (s->w - 276) >> 1, 200 + selectedItem * 70, true);
	}
}

void UpdateMenu(long time)
{
	if (data::input.keys[SDLK_ESCAPE])
		selectedItem = 3;

	else if (data::input.keys[SDLK_DOWN])
	{
		data::input.keys[SDLK_DOWN] = false;
		++selectedItem %= 4;
	}
	else if (data::input.keys[SDLK_UP])
	{
		data::input.keys[SDLK_UP] = false;
		selectedItem--;
		if (selectedItem < 0)
			selectedItem = 3;
	}
	else if (data::input.keys[SDLK_RETURN])
	{
		data::input.keys[SDLK_RETURN] = false;
		SelectItem();
	}
	else if (data::input.keys[SDLK_KP_ENTER])
	{
		data::input.keys[SDLK_KP_ENTER] = false;
		SelectItem();
	}

	if (data::input.mMoved || data::input.mPressed)
	{
		if (data::input.my > 165 && data::input.my < 455)
			selectedItem = (data::input.my - 175) / 70;
		else
			selectedItem = -1;
	}

	if (data::input.mPressed)
		SelectItem();
}

void SelectItem()
{
	if (selectedItem == 0)
	{
		data::currentState = GS_Play;
		data::timer = SDL_GetTicks();

		NewGame();
	}
	else if (selectedItem == 1)
	{
		data::currentState = GS_Options;
		data::timer = SDL_GetTicks();
	}
	else if (selectedItem == 2)
	{
		data::currentState = GS_Credits;
		data::timer = SDL_GetTicks();
	}
	else if (selectedItem == 3)
		data::exiting = true;

	data::input.mPressed = false;
	selectedItem = 0;
}