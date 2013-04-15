#include "func.h"
#include "font.h"
#include "lgfont.h"
#include "sprites.h"

void DrawLine(SDL_Surface* s, unsigned int color, int x, int y, int length, bool hz)
{
	if (hz)
		for (int i = x; i < x + length; i++)
			DrawDot(s, color, i, y);
	else
		for (int i = y; i < y + length; i++)
			DrawDot(s, color, x, i);
}

void DrawImg(SDL_Surface* s, unsigned char* img, unsigned char pal[][3], unsigned int x, unsigned int y, unsigned int px, unsigned int py, unsigned int pw, unsigned int ph, unsigned int iw)
{
	for (unsigned int i = px; i < px + pw; i++)
		for (unsigned int j = py; j < py + ph; j++)
		{
			int idx = img[j * iw + i];
			unsigned int c = SDL_MapRGB(s->format, pal[idx][0], pal[idx][1], pal[idx][2]);
			if (c != 0xff00ff) //ignore magenta
				DrawDot(s, c, x + i - px, y + j - py);
		}
}

static int spacing = 1;
void DrawString(SDL_Surface* s, char* string, int x, int y, bool useLargeFont)
{
	int p = 0;
	for (unsigned int n = 0; n < strlen(string); n++)
	{
		int ch = 0;
		
		if (string[n] == '#')
			ch = 40;
		else if (string[n] == '.')
			ch = 39;
		else if (string[n] == '?')
			ch = 38;
		else if (string[n] == '!')
			ch = 37;
		else if (string[n] >= 'a' && string[n] <= 'z')
			ch = string[n] - 'a' + 11;
		else if (string[n] >= 'A' && string[n] <= 'Z')
			ch = string[n] - 'A' + 11;
		else if (string[n] >= '0' && string[n] <= '9')
			ch = string[n] - '0' + 1;

		if (useLargeFont)
		{

			for (unsigned int i = 0; i < lgchar_w; i++)
				for (unsigned int j = 0; j < lgchar_h; j++)
				{
					int idx = lgfont_img[j * lgfont_w + (ch * lgchar_w) + i];
					unsigned int c = SDL_MapRGB(s->format, lgfont_pal[idx][0], lgfont_pal[idx][1], lgfont_pal[idx][2]);

					if (c != 0xff00ff) //ignore magenta
						DrawDot(s, c, i + x + p, y + j);
				}

			p += lgchar_w - lgchar_p + spacing;
		}
		else
		{
			for (unsigned int i = 0; i < char_w; i++)
				for (unsigned int j = 0; j < char_h; j++)
				{
					int idx = font_img[j * font_w + (ch * char_w) + i];
					unsigned int c = SDL_MapRGB(s->format, font_pal[idx][0], font_pal[idx][1], font_pal[idx][2]);

					if (c != 0xff00ff) //ignore magenta
						DrawDot(s, c, i + x + p, y + j);
				}

			p += char_w + spacing;
		}
	}
}

void DrawNums(SDL_Surface* s, unsigned int n, int x, int y)
{
	char nums[9];
	sprintf(nums, "%08d", n);
	for (unsigned int i = 0; i < strlen(nums); i++)
		DrawImg(s, sprites_img, sprites_pal, x + i * 39, y, (nums[i] - 48) * 37, 52, 37, 29, sprites_w);
}