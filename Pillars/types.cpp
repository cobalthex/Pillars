#include "types.h"
#include "game.h"

Explosion* MakeExplosion(ParticleColor color, long time, long delay, int x, int y)
{
	Explosion* e = new Explosion();
	e->color = color;
	e->time = time;
	e->delay = delay;
	e->x = x;
	e->y = y;

	for (int i = 0; i < 32; i++)
	{
		if (data::xplos[i] == 0)
		{
			data::xplos[i] = e;
			return data::xplos[i];
		}
	}

	return 0;
}

Shot* MakeShot(int x, int y, Direction dir)
{
	Shot* s = new Shot();
	s->ox = x;
	s->oy = y;
	s->x = x;
	s->y = y;
	s->dir = dir;
	s->time = SDL_GetTicks();

	return s;
}

bool MoveShot(Shot* s, ShotType st)
{
	int spd = data::flameSpd, idx = 0;
	if (st == ST_Rocket)
	{
		spd = data::rocketSpd;
		idx = 1;
	}
	else if (st == ST_Shard)
	{
		spd = data::shardSpd;
		idx = 2;
	}
	int dist = (SDL_GetTicks() - s->time) / 10 * spd;

	if (s->dir == D_Right)
		s->x = s->ox + dist;
	else if (s->dir == D_Left)
		s->x = s->ox - dist;
	else if (s->dir == D_Up)
		s->y = s->oy - dist;
	else if (s->dir == D_Down)
		s->y = s->oy + dist;

	if (s->x <= 0)
	{
		MakeExplosion((ParticleColor)idx, SDL_GetTicks(), 0, 4, s->y);
		delete s;
		data::shots[idx] = 0;
		return false;
	}
	else if (s->x >= data::tileCount * data::tileSize)
	{
		MakeExplosion((ParticleColor)idx, SDL_GetTicks(), 0, data::tileCount * data::tileSize - 4, s->y);
		delete s;
		data::shots[idx] = 0;
		return false;
	}
	else if (s->y <= 0)
	{
		MakeExplosion((ParticleColor)idx, SDL_GetTicks(), 0, s->x, 4);
		delete s;
		data::shots[idx] = 0;
		return false;
	}
	else if (s->y >= data::tileCount * data::tileSize)
	{
		MakeExplosion((ParticleColor)idx, SDL_GetTicks(), 0, s->x, data::tileCount * data::tileSize - 4);
		delete s;
		data::shots[idx] = 0;
		return false;
	}
	return true;
}

bool CheckShot(Shot* s, ShotType st)
{

	int x = 0, y = 0;
	Actor* a;

	//starting coords for checking
	int sx = s->x / data::tileSize - 1;
	int sy = s->y / data::tileSize - 1;

	//check the 9 spots near the shot to see if any are within (using exact points)
	for (int i = sx < 0 ? 0 : sx; i < sx + 3 && i < data::tileCount; i++)
		for (int j = sy < 0 ? 0 : sy; j < sy + 3 && j < data::tileCount; j++)
		{
			a = data::actors[i + j * data::tileCount];

			if (a != 0) //actor exists at spot
			{
				x = i * data::tileSize + a->x;
				y = j * data::tileSize + a->y;

				if (s->x < x|| s->y < y || s->x >= x + data::tileSize || s->y >= y + data::tileSize)
					continue; //outside object's bounds, ignore

				if (a->type == AT_Enemy && a->alive) //kill enemy
				{
					data::score += data::enemyKillScore;
					a->alive = false;
					a->anim = SDL_GetTicks();

					data::particles->Emit(16, s->x, s->y, 8, (ParticleColor)(st - 1));
					MakeExplosion((ParticleColor)(st - 1), SDL_GetTicks(), 0, s->x, s->y);
					data::shots[(int)st - 1] = 0;
					delete s;
					return false;
				}
				else if (a->type == AT_Pillar && a->alive) //rockets destroy pillars, shards go through
				{
					if (st == ST_Flame || st == ST_Rocket)
					{
						if (st == ST_Rocket)
						{
							a->alive = false;
							a->anim = SDL_GetTicks();
						}

						data::particles->Emit(16, s->x, s->y, 8, (ParticleColor)(st - 1));
						MakeExplosion((ParticleColor)(st - 1), SDL_GetTicks(), 0, s->x, s->y);
						data::shots[(int)st - 1] = 0;
						delete s;

						return false;
					}
				}
				else if (a->type == AT_Rubble && st == ST_Rocket) //rockets destroy rubble
				{
					data::actors[a->pos] = 0;
					delete a;
				}
			}
		}

		return true; //no collision
}