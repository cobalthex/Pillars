#include "game.h"
#include "intro.h"
#include "menu.h"
#include "options.h"
#include "credits.h"
#include "play.h"
#include "pause.h"
#include "scores.h"
#include "win.h"

namespace data
{
	InputHandler input;
	bool isAudioPlaying = true;
	bool isFullscreen = false;

	const int tileCount = 9;
	const int tileSize = 48;

	long timer;
	GameStates currentState;
	bool isGamePlaying = false;
	unsigned int level = 0;

	Actor** actors;
	Actor* player; //reference to the player in the actors listf
	int playerLives = 0;

	int score = 0;
	int visScore = 0;

	int top = 0;
	int visTop = 0;

	char msg[64];

	Shot* shots[3];
	const int maxExplos = 32;
	Explosion* xplos[maxExplos];

	Particles* particles;

	Battery battery;
	Bomb bomb;
	Nuke nuke;

	int flameShots = 0;
	int rocketShots = 0;
	int shardShots = 0;
	ShotType curShot = ST_None;
	long shotTime = 0;
	const short flameSpd = 8;
	const short rocketSpd = 6;
	const short shardSpd = 10;

	bool exiting;

	const int enemyKillScore = 250;
	const int pillarScore = 50;

	int currentSong;
	const int songCount = 1;
}

void NewGame()
{
	data::score = 0;
	data::visScore = 0;
	data::playerLives = 3;
	data::nuke = 0;
	data::bomb = 0;
	data::battery = 0;
	data::isGamePlaying = true;
	data::level = 1;

	data::currentSong = rand() % data::songCount;

	CreateLevel();
}

static int lastLevelLives = 3;
static unsigned int lastLevel = 0;
static int lastMapType = -1;
void CreateLevel()
{
	//make sure everything is destroyed
	data::input.Clear();
	data::particles->Clear();
	for (int i = 0; i < 3; i++)
	{
		if (data::shots[i] == 0)
			continue;

		delete data::shots[i];
		data::shots[i] = 0;
	}
	for (int i = 0; i < 32; i++)
	{
		if (data::xplos[i] == 0)
			continue;

		delete data::xplos[i];
		data::xplos[i] = 0;
	}
	for (int i = 0; i < data::tileCount * data::tileCount; i++)
	{
		if (data::actors[i] == 0)
			continue;
		delete data::actors[i];
		data::actors[i] = 0;
	}
	data::flameShots = 0;
	data::rocketShots = 0;
	data::shardShots = 0;
	data::player = 0;
	memset(data::msg, 0, sizeof(char) * 64);

	//generate level

	//bonus level
	if (lastLevel > 5 && lastLevel % 10 == 1 && lastLevelLives == data::playerLives)
	{
		int p = data::tileCount * (data::tileCount >> 1) + 1;
		data::actors[p] = Actor::MakePlayer(p, D_Right);
		data::player = data::actors[p];

		p = (data::tileCount - (data::tileCount >> 2)) - 1 + data::tileCount;
		for (int i = 0; i < 6; i++)
		{
			int pp = p + (i % 2 * 2) + (i % 3) * data::tileCount * 3; 
			data::actors[pp] = Actor::MakeEnemy(pp, D_Left);

			//fun times
			data::flameShots = 99;
			data::rocketShots = 99;
			data::shardShots = 99;
		} 
		long time = SDL_GetTicks();
		for (int i = 0; i < data::tileCount; i++)
		{
			p = (data::tileCount >> 1) + (data::tileCount * i);
			data::actors[p] = Actor::MakePillar(p);
			data::actors[p]->alive = false; 
			data::actors[p]->anim = 0;
		}
		sprintf(data::msg, "bonus level");

		lastLevel = data::level;
		return;
	}
	lastLevelLives = data::playerLives;

	//standard levels
	int r = 0, ec = 0;
	while ((r = rand() % 5) == lastMapType); //dont play the same map type twice in a row
	lastMapType = r;

	if (r == 0) //random pillar placement with player at top left and enemies in bottom right
	{
		int n = data::tileCount;
		for (int i = 0; i < 16; i++)
		{
			n += i + (rand() % 12) % data::tileSize;
			if (i > 15 || n < 0)
				n *= 2;
			if (n < 0 || n % data::tileCount == 0 || n % data::tileCount == data::tileCount - 1)
			{
				n += 32;
				i--;
				continue;
			}
			if (n >= data::tileCount * data::tileCount || data::actors[n - 1] != 0)
			{
				i--;
				n -= 48;
				continue;
			}

			data::actors[n] = Actor::MakePillar(n);
		}

		//enemies
		int tco2 = data::tileCount >> 1;
		for (int i = 0, n = 0; i < rand() % 4 + 4; i++)
		{
			int x = rand() % tco2 + tco2, y = rand() % tco2 + tco2;
			int p = x + y * data::tileCount;

			if (data::actors[p] != 0)
			{
				if (++n > 10) //don't get stuck if for some reason no available spots
					break;

				i--;
				continue;
			}

			n = 0;

			Actor* e = Actor::MakeEnemy(p, x < y ? D_Left : D_Up);
			ec++;

			if (e->level < 2)
				data::flameShots++;
			else if (e->level < 4)
				data::rocketShots++;
			else
				data::shardShots++;

			data::actors[p] = e;
		}

		//player
		data::actors[0] = Actor::MakePlayer(0, D_Right);
		data::player = data::actors[0];
	}
	else if (r == 1) //classic grid style with player in center and enemies around edge (with random pillar placement for a few)
	{
		int half = data::tileCount >> 1;
		int p = half + half * data::tileCount;
		int no = rand() % 10;

		for (int x = 1; x < data::tileCount; x += 2)
			for (int y = 1; y < data::tileCount; y += 2)
			{
				int pp = x + y * data::tileCount;

				if (pp != p)
					data::actors[pp] = Actor::MakePillar(pp);
			}

			for (int i = 0; i < 4 + rand() % 4; i++)
			{
				int x = rand() % data::tileCount, y = rand() % data::tileCount;
				if (x > y)
				{
					int pp = (x > half ? data::tileCount - 1 : 0) + y * data::tileCount;
					data::actors[pp] = Actor::MakeEnemy(pp, y > half ? D_Down : D_Up);
					ec++;

					if (data::actors[pp]->level < 2)
						data::flameShots++;
					else if (data::actors[pp]->level < 4)
						data::rocketShots++;
					else
						data::shardShots++;
				}
				else
				{
					int pp = (y > half ? data::tileCount * (data::tileCount - 1) : 0) + x;
					data::actors[pp] = Actor::MakeEnemy(pp, x > half ? D_Right : D_Left);
					ec++;

					if (data::actors[pp]->level < 2)
						data::flameShots++;
					else if (data::actors[pp]->level < 4)
						data::rocketShots++;
					else
						data::shardShots++;
				}
			}

			data::actors[p] = Actor::MakePlayer(p, (Direction)(rand() % 4));
			data::player = data::actors[p];
	}
	else if (r == 2) //player boxed in, enemies all around
	{
		for (int i = 1; i < data::tileCount - 1; i++) //horiz
		{
			if (i == data::tileCount >> 1) //holes in the middle
				continue;

			data::actors[i + data::tileCount] = Actor::MakePillar(i + data::tileCount);
			data::actors[i + data::tileCount * (data::tileCount - 2)] = Actor::MakePillar(i + data::tileCount * (data::tileCount - 2));
		}
		for (int i = 2; i < data::tileCount - 2; i++) //vertical
		{
			if (i == data::tileCount >> 1) //holes in the middle
				continue;

			data::actors[1 + i * data::tileCount] = Actor::MakePillar(1 + i * data::tileCount);
			data::actors[(data::tileCount - 2) + i * data::tileCount] = Actor::MakePillar((data::tileCount - 2) + i * data::tileCount);
		}

		data::actors[data::tileCount - 1] = Actor::MakeEnemy(data::tileCount - 1, (Direction)(rand() % 4));
		data::actors[data::tileCount * (data::tileCount - 1)] = Actor::MakeEnemy(data::tileCount * (data::tileCount - 1), (Direction)(rand() % 4));


		//two inner baddies are really low level
		int p = 2 + data::tileCount * 2;
		data::actors[p] = Actor::MakeEnemy(p, (Direction)(rand() % 4)); data::actors[p]->level = 0;
		p = data::tileCount - 3 + data::tileCount * (data::tileCount - 3);
		data::actors[p] = Actor::MakeEnemy(p, (Direction)(rand() % 4)); data::actors[p]->level = 0;

		ec += 4;

		data::rocketShots = 2;
		data::shardShots = 2;

		int tco2 = data::tileCount >> 1;
		p = tco2 + tco2 * data::tileCount;
		data::player = Actor::MakePlayer(p, (Direction)(rand() % 4));
		data::actors[p] = data::player;
	}
	else if (r == 3) //4 square
	{
		int excluded = 0;

		//place player on corners of map
		data::player = Actor::MakePlayer(0, (Direction)(rand() % 4));
		if (data::player->dir == D_Up)
		{
			int p = data::tileCount * (data::tileCount - 1);
			data::player->pos = p;
			data::actors[p] = data::player;
			excluded = 2;
		}
		else if (data::player->dir == D_Down)
		{
			int p = data::tileCount - 1;
			data::player->pos = p;
			data::actors[p] = data::player;
			excluded = 1;
		}
		else if (data::player->dir == D_Left)
		{
			int p = data::tileCount * data::tileCount - 1;
			data::player->pos = p;
			data::actors[p] = data::player;
			excluded = 3;
		}
		else
			data::actors[0] = data::player;

		//place enemies in quadrents
		for (int i = 0; i < 4; i++)
		{
			if (excluded == i)
				continue;

			Direction d = D_Right;
			for (int j = 0; j < rand() % 2 + 1; j++)
			{
				int x, y;
				if (i == 0) //u left
				{
					x = rand() % (data::tileCount >> 1);
					y = rand() % (data::tileCount >> 1);
				}
				else if (i == 1) //u right
				{
					x = rand() % (data::tileCount >> 1) + (data::tileCount >> 1) + 1;
					y = rand() % (data::tileCount >> 1);
					d = D_Down;
				}
				else if (i == 2) //b left
				{
					x = rand() % (data::tileCount >> 1);
					y = rand() % (data::tileCount >> 1) + (data::tileCount >> 1) + 1;
					d = D_Up;
				}
				else if (i == 3) //b down
				{
					x = rand() % (data::tileCount >> 1) + (data::tileCount >> 1) + 1;
					y = rand() % (data::tileCount >> 1) + (data::tileCount >> 1) + 1;
					d = D_Left;
				}

				int p = x + y * data::tileCount;
				if (data::actors[p] != 0)
				{
					j--;
					continue;
				}
				data::actors[p] = Actor::MakeEnemy(p, d);
				ec++;

				if (data::actors[p]->level < 2)
					data::flameShots++;
				else if (data::actors[p]->level < 4)
					data::rocketShots++;
				else
					data::shardShots++;
			}
		}

		//place vertical and horizontal rows of pillars
		for (int i = 0; i < data::tileCount; i += 2) 
		{
			int p = (data::tileCount >> 1) * data::tileCount + i;
			data::actors[p] = Actor::MakePillar(p);
			p = (data::tileCount >> 1) + data::tileCount * i;
			data::actors[p] = Actor::MakePillar(p);
		}
	}
	else if (r == 4)
	{
		for (int y = 1; y < data::tileCount; y += 2)
		{
			for (int x = 1; x < data::tileCount - 1; x++)
				data::actors[x + y * data::tileCount] = Actor::MakePillar(x + y * data::tileCount);

			int p = 1 + (y - 1) * data::tileCount;
			data::actors[p] = Actor::MakeEnemy(p, rand() % 2 ? D_Right : D_Left);
			ec++;

			if (data::actors[p]->level < 2)
				data::flameShots++;
			else if (data::actors[p]->level < 4)
				data::rocketShots++;
			else
				data::shardShots++;
		}

		int p = data::tileCount * data::tileCount - 1;
		data::player = Actor::MakePlayer(p, D_Right);
		data::actors[p] = data::player;
	}

	//random powerups
	int rando = rand() % 100;
	int p = rand() % data::tileCount + rand() % data::tileCount * data::tileCount;
	if (data::actors[p] == 0)
	{
		if (rando < 75 && rando > 65 && data::playerLives < 2)
			data::actors[p] = Actor::MakePowerup(AT_Battery, p);
		else if (rando < 45 && rando > 35 && ec > 3)
			data::actors[p] = Actor::MakePowerup(AT_Bomb, p);
		else if (rando < 13 && rando > 5 && ec > 5)
			data::actors[p] = Actor::MakePowerup(AT_Nuke, p);
	}

}


void init(SDL_Surface* s)
{
	srand((long)time(0));

	data::timer = SDL_GetTicks();
	data::currentState = GS_Intro;
	data::particles = new Particles(128, &s->clip_rect);

	SDL_FillRect(s, &s->clip_rect, 0);

	//load high scores
	FILE* scores = fopen(".dat", "r");
	if (scores != 0)
	{
		fscanf(scores, "%c%c%d", &data::isAudioPlaying, &data::isFullscreen, &data::top);
		fclose(scores);

		if (data::isFullscreen)
			s = SDL_SetVideoMode(0, 0, 0, s->flags ^ SDL_FULLSCREEN);
	}

	if (data::actors == 0)
		data::actors = (Actor**)calloc(sizeof(Actor*), data::tileCount * data::tileCount);
	memset(data::actors, 0, sizeof(Actor*) * data::tileCount * data::tileCount);
}

void end()
{
	for (int i = 0; i < data::tileCount * data::tileCount; i++)
		if (data::actors[i] != 0)
			delete data::actors[i];

	free(data::actors);
}

bool update()
{
	if (data::exiting || !data::input.Update())
		return false;

	long time = SDL_GetTicks();

	switch(data::currentState)
	{
	case GS_Intro: 
		UpdateIntro(time);
		break;
	case GS_Menu: 
		UpdateMenu(time);
		break;
	case GS_Options: 
		UpdateOptions(time);
		break;
	case GS_Credits: 
		UpdateCredits(time);
		break;
	case GS_Play: 
		UpdatePlay(time);
		break;
	case GS_Pause: 
		UpdatePause(time);
		break;
	case GS_Win: 
		UpdateWin(time);
		break;
	case GS_Scores: 
		UpdateScores(time);
		break;
	}

	return true;
}


void draw(SDL_Surface* s)
{
	switch (data::currentState)
	{
	case GS_Intro: 
		DrawIntro(s);
		break;
	case GS_Menu: 
		DrawMenu(s);
		break;
	case GS_Options: 
		DrawOptions(s);
		break;
	case GS_Credits: 
		DrawCredits(s);
		break;
	case GS_Play: 
		DrawPlay(s);
		break;
	case GS_Pause: 
		DrawPause(s);
		break;
	case GS_Win: 
		DrawWin(s);
		break;
	case GS_Scores: 
		DrawScores(s);
		break;
	default:
		SDL_FillRect(s, &s->clip_rect, 0);
	}

	//draw cursor
	DrawImg(s, sprites_img, sprites_pal, data::input.mx, data::input.my, 191, 12, 16, 16, sprites_w);

	//Shit Piss Fuck Cunt Cocksucker Motherfucker Tits -- great show
}
