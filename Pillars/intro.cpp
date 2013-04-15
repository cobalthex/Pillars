#include "intro.h"
#include "func.h"
#include "game.h"
#include "logo.h"

void DrawIntro(SDL_Surface* s)
{
	SDL_FillRect(s, &s->clip_rect, 0);

	DrawString(s, "Dejitaru Forge", (s->w - 325) >> 1, 80, true);
	int xx = (s->w - 200) >> 1;
	DrawString(s, "Presents", xx, 120, true);

	for (int x = 0; x < 200; x++)
			for (int y = 0; y < 24; y++)
			{
				int n = (y + 120) * s->pitch / szInt + (x + xx);
				if (n > 0 && ((unsigned int*)s->pixels)[n] != 0)
					((unsigned int*)s->pixels)[n] = 0xffffaa - ((unsigned int*)s->pixels)[n];
			}

	DrawImg(s, logo_img, logo_pal, (s->w - logo_w) >> 1, (s->h - logo_h) >> 1, 0, 0, logo_w, logo_h, logo_w);
}

void UpdateIntro(long time)
{
	if (time - data::timer > 1000 || data::input.IsKeyPressed() || data::input.mPressed)
	{
		data::input.Clear();
		data::currentState = GS_Menu;
		data::timer = time;
	}
}