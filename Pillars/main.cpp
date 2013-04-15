#include "game.h"
#include "func.h"
#include "sprites.h"

int main(int argc, char** argv)
{
	static SDL_Surface* screen;

	SDL_putenv("SDL_VIDEO_CENTERED=1");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		Error(SDL_GetError(), true);

	SDL_WM_SetCaption("Pillars", "Pillars");

	//set the icon
	SDL_Surface* icon = SDL_CreateRGBSurface(0, 32, 32, 32, 0x0000ff, 0x00ff00, 0xff0000, 0x000000);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, 0xff00ff);
	DrawImg(icon, sprites_img, sprites_pal, 0, 0, 208, 12, 32, 32, sprites_w);

	SDL_WM_SetIcon(icon, 0);
	SDL_ShowCursor(0);

	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF);
	
	/*SDL_Surface* rend = SDL_CreateRGBSurface(SDL_HWPALETTE | SDL_HWPALETTE, 640, 480, 32, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	SDL_FillRect(screen, &screen->clip_rect, 0);
	SDL_Rect blitRect = Rect((800 - 640) >> 1, 0, 640, 480);*/

	init(screen);

	while (1)
	{
		if (!update())
			break;

		draw(screen);
		//SDL_BlitSurface(rend, 0, screen, &blitRect);
		SDL_Flip(screen);
		SDL_Delay(5);
	}

	//save scores
	FILE* scores = fopen(".dat", "w");
	if (scores != 0)
	{
		fprintf(scores, "%c%c%d", data::isAudioPlaying, data::isFullscreen, data::top);
		fclose(scores);
	}

	SDL_CloseAudio();
	SDL_Quit();
	return 0;
}