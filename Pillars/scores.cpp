#include "scores.h"
#include "func.h"
#include "game.h"
#include "sprites.h"
#include "AudioPlayer.h"
#include <cmath>

void DrawScores(SDL_Surface* s)
{
	SDL_FillRect(s, &s->clip_rect, 0);
	int tdif = SDL_GetTicks() - data::timer;

	int x1 = (s->w - 312) >> 1;

	int xx = (s->w - 207) >> 1;
	DrawString(s, "Game Over", xx, 70, true);
	DrawString(s, "Your Score", (s->w - 230) >> 1, 180, true);
	DrawString(s, "High Score", (s->w - 230) >> 1, 300, true);

	//change the color of the game over logo
	for (int x = 0; x < 207; x++)
		for (int y = 0; y < 24; y++)
		{
			int n = (y + 70) * s->pitch / szInt + (x + xx);
			if (((unsigned int*)s->pixels)[n] != 0)
				((unsigned int*)s->pixels)[n] = 0xff4400 & ((unsigned int*)s->pixels)[n];
		}

	DrawNums(s, data::score, x1, 220);
	DrawNums(s, data::top, x1, 340);

	if (data::score > 0 && data::score >= data::top && tdif % 400 < 200)
		DrawString(s, "You got the high score!", (s->w - 529) >> 1, 400, true);

	DrawString(s, "Press any key to continue", (s->w - 225) >> 1, 470, false);

	//draw pulses
	int start = (tdif / 10) % s->w;
	for (int i = 0; i < 240; i++)
	{
		int c = (int)(sin((float)i / 80.f) * 200);
		int n = (i + start) % s->w;
		DrawDot(s, SDL_MapRGB(s->format, c, 0, 0), n, 100);
		DrawDot(s, SDL_MapRGB(s->format, c, 0, 0), n + 1, 101);
		DrawDot(s, SDL_MapRGB(s->format, c, 0, 0), n, 100);

		n = s->w - n;
		DrawDot(s, SDL_MapRGB(s->format, c, 0, 0), n, 105);
		DrawDot(s, SDL_MapRGB(s->format, c, 0, 0), n - 1, 106);
		DrawDot(s, SDL_MapRGB(s->format, c, 0, 0), n, 105);
	}
	
	//fade in animation
	if (tdif < 900)
	{
		int anim = tdif / 100;

		//score
		DrawImg(s, sprites_img, sprites_pal, x1 + anim * 39, 220, 405, 52, 37, 29, sprites_w);
		DrawImg(s, sprites_img, sprites_pal, x1 + (anim + 1) * 39, 220, 369, 52, 37, 29, sprites_w);
		SDL_FillRect(s, &Rect(x1 + (anim + 1) * 38, 220, (8 - anim) * 39, 29), 0);

		//top
		DrawImg(s, sprites_img, sprites_pal, x1 + (9 - anim) * 39, 340, 405, 52, 37, 29, sprites_w);
		DrawImg(s, sprites_img, sprites_pal, x1 + (8 - anim) * 39, 340, 369, 52, 37, 29, sprites_w);
		SDL_FillRect(s, &Rect(x1 - 39, 340, (8 - anim) * 39, 29), 0);
	}
}

void UpdateScores(long time)
{
	if (time - data::timer > 800 && (data::input.IsKeyPressed() || data::input.mPressed)) //any key or mouse
	{
		data::input.Clear();
		data::currentState = GS_Menu;
		data::isGamePlaying = false;
		data::timer = -1000; //skip intro
		
		AudioPlayer::Stop();
	}
}