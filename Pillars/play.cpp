#include "play.h"
#include "func.h"
#include "game.h"
#include "sprites.h"
#include "font.h"
#include "AudioPlayer.h"
#include "music.h"
#include "enemy.h"

void DrawPlay(SDL_Surface* s)
{
	SDL_FillRect(s, &s->clip_rect, 0);
	DrawBoard(s);
	DrawHud(s);
	DrawFx(s);
}

void UpdatePlay(long time)
{
	if (!AudioPlayer::isPlaying && data::isAudioPlaying) //will auto play a random song
	{
		AudioPlayer::PlayWAV(song0, true);
	}

	if (data::score > data::top) //player has highscore
		data::top = data::score;

	if (time % 5 == 0)
	{
		data::visScore = addUp(data::score, data::visScore);
		data::visTop = addUp(data::top, data::visTop);
	}

	if (data::input.keys[SDLK_ESCAPE]) //pause
	{
		data::input.keys[SDLK_ESCAPE] = false;
		data::currentState = GS_Pause;
	}

	data::particles->Update(time);
	
	if (time - data::shotTime > 50) //muzzle flash only lasts 50ms
		data::curShot = ST_None;

	//update shots
	if (data::shots[0] != 0)
	{
		if (MoveShot(data::shots[0], ST_Flame))
			CheckShot(data::shots[0], ST_Flame);
	}
	if (data::shots[1] != 0)
	{
		if (MoveShot(data::shots[1], ST_Rocket))
			CheckShot(data::shots[1], ST_Rocket);
	}
	if (data::shots[2] != 0)
	{
		if (MoveShot(data::shots[2], ST_Shard))
			CheckShot(data::shots[2], ST_Shard);
	}

	if (data::player->alive && winTime < 1) //player input -- but only if alive & not already won the game
	{
		//move player (but only if not already moving) -- ctrl key causes change in direction without moving
		if (data::player->moveTime == 0)
		{
			if (data::input.keys[SDLK_RIGHT])
			{
				data::player->dir = D_Right;

				int p = data::player->GetFacing();
				if (p != -1 && !data::input.ctrlPressed && data::player->Interact(data::actors[p]))
					data::player->moveTime = time;
				else
					data::input.keys[SDLK_RIGHT] = false;
			}
			else if (data::input.keys[SDLK_LEFT])
			{
				data::player->dir = D_Left;

				int p = data::player->GetFacing();
				if (p != -1 && !data::input.ctrlPressed && data::player->Interact(data::actors[p]))
					data::player->moveTime = time;
				else
					data::input.keys[SDLK_LEFT] = false;
			}

			else if (data::input.keys[SDLK_UP])
			{
				data::player->dir = D_Up;

				int p = data::player->GetFacing();
				if (p != -1 && !data::input.ctrlPressed && data::player->Interact(data::actors[p]))
					data::player->moveTime = time;
				else
					data::input.keys[SDLK_UP] = false;
			}
			else if (data::input.keys[SDLK_DOWN])
			{
				data::player->dir = D_Down;

				int p = data::player->GetFacing();
				if (p != -1 && !data::input.ctrlPressed && data::player->Interact(data::actors[p]))
					data::player->moveTime = time;
				else
					data::input.keys[SDLK_DOWN] = false;
			}
		}

		//shoot (q = flame, w = rocket, e = shard)
		if (data::shots[0] == 0 && data::flameShots > 0 && data::input.keys[SDLK_q])
		{
			data::flameShots--;
			int p = data::player->GetFacing();
			if (p > -1)
			{
				data::shots[0] = MakeShot((p % data::tileCount) * data::tileSize + (data::tileSize >> 1), (p / data::tileCount) * data::tileSize + (data::tileSize >> 1), data::player->dir);
				data::curShot = ST_Flame;
				data::shotTime = time;
			}
			data::input.keys[SDLK_q] = false;
		}
		else if (data::shots[1] == 0 && data::rocketShots > 0 && data::input.keys[SDLK_w])
		{
			data::rocketShots--;
			int p = data::player->GetFacing();
			if (p > -1)
			{
				data::shots[1] = MakeShot((p % data::tileCount) * data::tileSize + (data::tileSize >> 1), (p / data::tileCount) * data::tileSize + (data::tileSize >> 1), data::player->dir);
				data::curShot = ST_Rocket;
				data::shotTime = time;
			}
			data::input.keys[SDLK_w] = false;
		}
		else if (data::shots[2] == 0 && data::shardShots > 0 && data::input.keys[SDLK_e])
		{
			data::shardShots--;
			int p = data::player->GetFacing();
			if (p > -1)
			{
				data::shots[2] = MakeShot((p % data::tileCount) * data::tileSize + (data::tileSize >> 1), (p / data::tileCount) * data::tileSize + (data::tileSize >> 1), data::player->dir);
				data::curShot = ST_Shard;
				data::shotTime = time;
			}
			data::input.keys[SDLK_e] = false;
		}

		//use power up (1 = battery, 2 = bomb, 3 = nuke)
		if (data::battery != 0 && data::input.keys[SDLK_1])
		{
			data::playerLives = 3;
			delete data::battery;
			data::battery = 0;

			int hlf = data::tileSize >> 1, qtr = hlf >> 1;
			int px = data::player->pos % data::tileCount * data::tileSize + hlf;
			int py = data::player->pos / data::tileCount * data::tileSize + hlf;

			MakeExplosion(PRT_Red, time, 0, px - qtr, py);
			MakeExplosion(PRT_Red, time, 0, px, py - qtr);
			MakeExplosion(PRT_Blue, time, 100, px, py);
			MakeExplosion(PRT_Green, time, 200, px, py + qtr);
			MakeExplosion(PRT_Green, time, 200, px + qtr, py);
		}
		else if (data::player->moveTime == 0 && data::bomb != 0 && data::input.keys[SDLK_2])
		{
			int n = data::player->GetFacing();
			if (n > -1 && data::actors[n] == 0)
			{
				data::bomb->anim = time;
				data::bomb->animSpd = 100;
				data::bomb->frames = 6;
				data::bomb->pos = n;
				data::bomb->assoc = 1;
				data::actors[n] = data::bomb;
				data::bomb = 0;
			}
		}
		else if (data::player->moveTime == 0 && data::nuke != 0 && data::input.keys[SDLK_3])
		{
			int n = data::player->GetFacing();
			if (n > -1 && data::actors[n] == 0)
			{
				data::nuke->anim = 0;
				data::nuke->pos = n;
				data::nuke->assoc = time;
				data::actors[n] = data::nuke;
				data::nuke = 0;
			}
		}
	}
	else
	{
		if (data::playerLives < 0 && data::player->anim == 0) //game over
		{
			winTime = 0;
			data::isGamePlaying = false;
			
			data::currentState = GS_Scores;
			data::timer = SDL_GetTicks();
		}
		else if (!data::player->alive && data::player->anim == 0 && winTime == 0) //respawn after death
		{
			CreateLevel();
			data::player->alive = true;
		}
	}

	//update all active actors
	int eCount = 0;
	for (int i = 0; i < data::tileCount * data::tileCount; i++)
	{
		if (data::actors[i] == 0)
			continue;

		Actor* a = data::actors[i];

		if (a->type == AT_Enemy && a->alive) //count enemies left on the map
		{
			eCount++;
			EnemyThink(a);
		}

		if (a->moveTime != 0)
			a->Move();

		if (a->anim > 0 && (time - a->anim) / a->animSpd >= a->frames)
		{
			if (!a->alive && a->type == AT_Enemy)
			{
				data::actors[i] = 0;
				delete a;
				continue;
			}
			else if (a->type == AT_Bomb && a->assoc > 0)
				Actor::BombExplode(a->pos);
		}

		if (a->type == AT_Nuke && a->assoc > 0 && time - a->assoc > 3000)
		{
			//force delete nuke to make sure player cannot pick it up again
			data::actors[a->pos] = 0;
			delete a;
			
			//kill all ents on the map
			for (int j = 0; j < data::tileCount * data::tileCount; j++)
			{
				if (data::actors[j] == 0 || data::actors[j]->type == AT_Player)
					continue; 

				data::actors[j]->alive = false;
				data::actors[j]->anim = time;
			}
			data::score += 1000;
			eCount = 0;
			break;
		}
	}
	if (eCount == 0 && winTime == 0) //level complete (start win animation)
	{
		for (int i = 0; i < 3; i++)
			data::particles->Emit(32, 50 + rand() % (data::particles->bounds->w - 100), 50 + rand() % (data::particles->bounds->h - 100), 4, (ParticleColor)i);
		
		for (int i = 0; i < 3; i++)
			data::particles->Emit(10, 50 + rand() % (data::particles->bounds->w - 100), 50 + rand() % (data::particles->bounds->h - 100), 6, (ParticleColor)i);

		winTime = time;
		data::score += 1000;
	}

	if (winTime > 0 && time - winTime > 1000) //win animation over, move to next screen
	{
		data::currentState = GS_Win;
		data::timer = time;
		winTime = 0;
	}
}


//drawing routines

void DrawBoard(SDL_Surface* s)
{
	long time = SDL_GetTicks();

	//draw grid
	Uint32 c = SDL_MapRGB(s->format, 8, 16, 32);
	for (int i = 0; i <= data::tileCount; i++)
	{
		DrawLine(s, c, 0, i * data::tileSize, data::tileSize * data::tileCount, true);
		DrawLine(s, c, i * data::tileSize, 0, data::tileSize * data::tileCount, false);
	}

	//create surface if not already created for drawing actors
	if (rend == 0)
	{
		rend = SDL_CreateRGBSurface(SDL_HWSURFACE, data::tileSize * data::tileCount, data::tileSize * data::tileCount, 32, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask);
		SDL_SetColorKey(rend, SDL_SRCCOLORKEY, 0xff00ff);
	}
	SDL_FillRect(rend, 0, 0xff00ff);

	//draw actors and their glow grid
	for (int n = 0; n < data::tileCount * data::tileCount; n++)
	{
		Actor* a = data::actors[n];

		if (a == 0)
			continue;

		if (a->type != AT_Pillar && a->type != AT_Rubble)
		{
			int xxx = a->pos % data::tileCount;
			int yyy = a->pos / data::tileCount;
			for (int x = -1; x < 3; x++)
			{
				int xx = xxx + x;
				if (xx < 0 || xx > 9)
					continue;

				int c = (x == -1 || x == 2 ? 32 : 64);
				DrawLine(s, SDL_MapRGB(s->format, 0, c, c >> 1), xx * data::tileSize, yyy * data::tileSize, data::tileSize, false);
			}

			for (int y = -1; y < 3; y++)
			{
				int yy = yyy + y;
				if (yy < 0 || yy > 9)
					continue;

				int c = (y == -1 || y == 2 ? 32 : 64);
				DrawLine(s, SDL_MapRGB(s->format, 0, c, c >> 1),xxx * data::tileSize, yy * data::tileSize, data::tileSize, true);
			}
		}

		//draw ents (to separate surface so that lines dont get drawn over ent)
		int h = data::tileSize >> 1;

		int x = n % data::tileCount * data::tileSize + a->x + 1;
		int y = n / data::tileCount * data::tileSize + a->y + 1;

		int frame = 0; //current frame of animation (all animations are 100ms/frame)
		if (a->anim != 0)
			frame = (time - a->anim) / a->animSpd;
		if (frame >= a->frames)
		{
			frame = a->frames - 1;

			if (a->loop)
				a->anim = time;
			else
				a->anim = 0;
		}

		//draw the ents
		//powerups
		if (a->type == AT_Battery)
			DrawImg(rend, sprites_img, sprites_pal, x + 11, y + 12, 241 + (frame > 4 ? 0 : frame * 27), 0, 26, 23, sprites_w);
		else if (a->type == AT_Bomb)
		{
			if (a->assoc > 0)
				DrawImg(rend, sprites_img, sprites_pal, x + 15, y + 8, 198 + (frame * 19), 128, 19, 32, sprites_w);
			else
				DrawImg(rend, sprites_img, sprites_pal, x + 15, y + 8, 311 + (frame > 4 ? 0 : frame * 19), 128, 19, 32, sprites_w);
		}
		else if (a->type == AT_Nuke) //nuke has special anim
		{
			if (a->alive)
			{
				if (a->assoc > 0)
				{
					DrawImg(s, sprites_img, sprites_pal, x + 9, y + 13, 349, 24, 26, 26, sprites_w);
					DrawImg(s, font_img, font_pal, x + (data::tileSize - char_w - 3), y + 2, (4 - ((time - a->assoc) / 1000)) * char_w, 0, char_w, char_h, font_w); 
				}
				else
					DrawImg(s, sprites_img, sprites_pal, x + 11, y + 11, 241 + (frame > 3 ? 0 : frame * 27), 24, 26, 26, sprites_w);
			}
		}

		else if (a->type == AT_Enemy) //enemies
		{
			DrawImg(rend, sprites_img, sprites_pal, x, y, 233 + a->assoc * 47, 82, 46, 46, sprites_w);
			DrawEyes(rend, x, y, a->dir, 3 + frame);
		}
		else if (a->type == AT_Player) //player
		{
			DrawImg(rend, sprites_img, sprites_pal, x, y, 186, 82, 46, 46, sprites_w);
			DrawEyes(rend, x, y, a->dir, frame);
		}
		else if (a->type == AT_Pillar) //pillars
		{
			if (a->alive)
				DrawImg(rend, sprites_img, sprites_pal, x, y - 31, 0, 82, 46, 78, sprites_w);
			else
			{
				if (frame == 0)
					DrawImg(rend, sprites_img, sprites_pal, x, y + 18, 141, 138, 45, 22, sprites_w);
				else
					DrawImg(rend, sprites_img, sprites_pal, x, y - 31, frame * 46, 82, 46, 78, sprites_w);
			}
		}
		else if (a->type == AT_Rubble) //rubble
			DrawImg(rend, sprites_img, sprites_pal, x, y + 26, 141, 117, 45, 20, sprites_w);
	}

	SDL_BlitSurface(rend, 0, s, 0);

#ifdef _DEBUG
	int dif = SDL_GetTicks() - time;
	if (dif != 0)
	{
		char str[5]; sprintf(str, "%04d", 1000 / dif);
		DrawString(s, str, 600, 469, false);
	}
#endif
}

void DrawEyes(SDL_Surface* target, int x, int y, Direction dir, int frame)
{
	int xx = 216 - (frame > 2 ? 25 : 0);

	if (frame > 2)
		frame -= 3;

	int yy = 0;
	if (frame == 1)
		yy = 6;
	else if (frame == 2)
		yy = 10;

	for (int i = 0; i < 24; i++)
		for (int j = 0; j < 5 - (frame * 2); j++)
		{
			int idx = sprites_img[(j + yy) * sprites_w + (i + xx)];
			unsigned int c = SDL_MapRGB(target->format, sprites_pal[idx][0], sprites_pal[idx][1], sprites_pal[idx][2]);
			if (c == 0xff00ff)
				continue;

			if (dir == D_Right)
				DrawDot(target, c, x + 36 - j, y + i + 12);
			else if (dir == D_Up)
				DrawDot(target, c, x + i + 12, y + j + 12);
			else if (dir == D_Left)
				DrawDot(target, c, x + 10 + j, y + i + 12);
			else if (dir == D_Down)
				DrawDot(target, c, x + i + 12, y + 36 - j);
		}
}

void DrawHud(SDL_Surface* s)
{
	DrawImg(s, sprites_img, sprites_pal, 581, 10, 0, 0, 49, 25, sprites_w);
	DrawImg(s, sprites_img, sprites_pal, 538, 90, 0, 26, 92, 25, sprites_w);

	char scores[9]; sprintf(scores, "%08d", data::visTop); //high score
	DrawString(s, scores, 445, 44, true);

	sprintf(scores, "%08d", data::visScore); //score
	DrawString(s, scores, 445, 124, true);

	//draw lives
	for (int i = 0; i < 3; i++)
		DrawImg(s, sprites_img, sprites_pal, 496 + i * 32, 200, i < data::playerLives ? 50 : 93, i < data::playerLives ? 0 : 30, 24, 21, sprites_w);

	//draw powerups on HUD
	if (data::battery != 0)
		DrawImg(s, sprites_img, sprites_pal, 495, 320, 241, 0, 26, 23, sprites_w);
	if (data::bomb != 0)
		DrawImg(s, sprites_img, sprites_pal, 530, 320, 198, 134, 18, 26, sprites_w);
	if (data::nuke != 0)
		DrawImg(s, sprites_img, sprites_pal, 560, 320, 241, 24, 26, 26, sprites_w);
	
	DrawImg(s, sprites_img, sprites_pal, 500, 270, 120, 20, 14, 14, sprites_w); //flame shot
	DrawImg(s, sprites_img, sprites_pal, 532, 271, 120, 5, 14, 14, sprites_w); //rocket shot
	DrawImg(s, sprites_img, sprites_pal, 564, 270, 120, 35, 14, 14, sprites_w); //shard shot

	char n[] = {(char)((data::flameShots > 9 ? 9 : data::flameShots) + '0'), (char)0};
	DrawString(s, n, 503, 250, false);
	n[0] = (char)((data::rocketShots > 9 ? 9 : data::rocketShots) + '0');
	DrawString(s, n, 535, 250, false);
	n[0] = (char)((data::shardShots > 9 ? 9 : data::shardShots) + '0');
	DrawString(s, n, 567, 250, false);
	
	static char str[64];
	memset(str, 0, sizeof(char) * 64);
	int len = strlen(data::msg);
	long tm = SDL_GetTicks() - data::timer;
	if (len > 0 && tm < 2000)
	{
		bool lg = len < 28;
		DrawString(s, strncpy(str, data::msg, tm >> 5 > 64 ? 64 : tm >> 5), 8, lg ? s->h - 32 : s->h - 26, lg);
	}
}

void DrawFx(SDL_Surface* s)
{
	//draw shots
	for (int n = 0; n < 3; n++)
	{
		if (data::shots[n] == 0)
			continue;

		Shot* sht = data::shots[n];

		//draw shots at all angles
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 25; j++)
			{
				int idx = sprites_img[j * sprites_w + (i + (n * 14) + 78)];
				unsigned int c = SDL_MapRGB(s->format, sprites_pal[idx][0], sprites_pal[idx][1], sprites_pal[idx][2]);
				if (c == 0xff00ff)
					continue;

				if (sht->dir == D_Right)
					DrawDot(s, c, sht->x - j, sht->y + i - 6);
				else if (sht->dir == D_Left)
					DrawDot(s, c, sht->x + j, sht->y + i - 6);
				else if (sht->dir == D_Up)
					DrawDot(s, c, sht->x + i - 6, sht->y + j);
				else if (sht->dir == D_Down)
					DrawDot(s, c, sht->x + i - 6, sht->y - j);
			}
	}

	//draw muzzle flash
	if (data::curShot != ST_None)
	{
		int px = (data::player->pos % data::tileCount) * data::tileSize + data::player->x;
		int py = (data::player->pos / data::tileCount) * data::tileSize + data::player->y;
		int tz2 = (data::tileSize >> 1) - 6;

		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
			{
				int idx = sprites_img[(j + ((int)(data::curShot) - 1) * 13) * sprites_w + (i + 177)];
				unsigned int c = SDL_MapRGB(s->format, sprites_pal[idx][0], sprites_pal[idx][1], sprites_pal[idx][2]);
				if (c == 0xff00ff)
					continue;

				if (data::player->dir == D_Right)
					DrawDot(s, c, px - j + 13 + data::tileSize, py + i + tz2);
				else if (data::player->dir == D_Left)
					DrawDot(s, c, px + j - 13, py + i + tz2);
				else if (data::player->dir == D_Up)
					DrawDot(s, c, px + i + tz2, py + j - 13);
				else if (data::player->dir == D_Down)
					DrawDot(s, c, px + i + tz2, py - j + 13 + data::tileSize);
			}
	}

	//draw particles
	data::particles->Draw(s);
	
	long time = SDL_GetTicks();
	//draw explosions
	for (int n = 0; n < data::maxExplos; n++)
	{
		if (data::xplos[n] == 0)
			continue;

		Explosion* xpl = data::xplos[n];

		int frame = (time - xpl->time - xpl->delay) / 100;

		if (frame > -1 && frame < 3)
			DrawImg(s, sprites_img, sprites_pal, xpl->x - 6, xpl->y - 6, 135 + (xpl->color * 14), frame * 14, 13, 13, sprites_w);
		else if (frame > 2)
		{
			delete data::xplos[n];
			data::xplos[n] = 0;
		}
	}
}