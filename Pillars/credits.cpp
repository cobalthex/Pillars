#include "credits.h"
#include "func.h"
#include "game.h"
#include "logo.h"
#include "lgfont.h"
#include "AudioPlayer.h"
#include <cmath>

static const int numCreds = 2;
static const char* credTitle[] = {
	"Created by",
	"Graphics by",
};
static const char* credValue[] = {
	"Matt Hines",
	"Matt Hines",
};

void DrawCredits(SDL_Surface* s)
{
	long time = (SDL_GetTicks() - data::timer);
	SDL_FillRect(s, &s->clip_rect, 0);

	int itro = (long)(s->h + logo_h) * 4;
	if (time < itro) //draw menu fading out
	{
		DrawImg(s, logo_img, logo_pal, (s->w - logo_w) >> 1, 10 + time / 4, 0, 0, logo_w, logo_h, logo_w);

		if (time < 1050)
			DrawString(s, "Exit", (s->w - 92) >> 1, 410, true);
		if (time < 700)
			DrawString(s, "credits", (s->w - 161) >> 1, 340, true);
		if (time < 350)
			DrawString(s, "Options", (s->w - 161) >> 1, 270, true);
	}
	else if (time < itro + 500)
	{
		int xx = (s->w - logo_w) >> 1;
		DrawImg(s, logo_img, logo_pal, xx, 50, 0, 0, logo_w, logo_h, logo_w);
		
		int freq = (time - itro) / 100 + 1;
		for (unsigned int i = 0; i < logo_w; i += freq)
			for (unsigned int j = 0; j < logo_h; j += freq)
				DrawDot(s, 0, i + xx, j + 50);
	}
	else
	{
		DrawImg(s, logo_img, logo_pal, (s->w - logo_w) >> 1, 50, 0, 0, logo_w, logo_h, logo_w);

		int i = ((time - 20) / 2000 - 1) % numCreds;
		int sz = strlen(credValue[i]) * (lgchar_w + 1);
		int y = time / 5 % 400;
		//left text
		if (y < 350)
			DrawString(s, (char*)credTitle[i], 20, 490 - y, true);
		else
			DrawString(s, (char*)credTitle[i], 20 - (y - 350) * 8, 140, true);
		
		//right text
		if (y < 50)		
			DrawString(s, (char*)credValue[i], (s->w + 20 - sz) + (50 - y) * 8, 140, true);
		else
			DrawString(s, (char*)credValue[i], s->w + 20 - sz, 90 + y, true);
	}
}

void UpdateCredits(long time)
{
	if (data::input.keys[SDLK_ESCAPE] || data::input.mPressed)
	{
		data::currentState = GS_Menu;
		data::timer = -1000;
		data::input.keys[SDLK_ESCAPE] = false;
		data::input.mPressed = false;
		//AudioPlayer::Stop();
	}

	//else if (!AudioPlayer::isPlaying)
	//	AudioPlayer::Play((Uint8*)scredits_data, scredits_len, true);
}