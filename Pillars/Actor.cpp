#include "Actor.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include <cmath>

Actor::Actor()
{
	alive = false;
	type = AT_None;
	assoc = 0;
	level = 0;
	x = 0;
	y = 0;
	moveTime = 0;
	dir = D_Right;
	anim = 0;
	frames = 0;
	animSpd = 100;
	loop = false;
	pos = 0;
}

int Actor::GetFacing()
{
	int p = -1;

	if (dir == D_Left)
	{
		if (pos % data::tileCount != 0)
			p = pos - 1;
	}
	else if (dir == D_Right)
	{
		if (pos % data::tileCount != data::tileCount - 1)
			p = pos + 1;
	}
	else if (dir == D_Up)
	{
		if (pos / data::tileCount != 0)
			p = pos - data::tileCount;
	}
	else if (dir == D_Down)
	{
		if (pos / data::tileCount != data::tileCount - 1)
			p = pos + data::tileCount;
	}

	return p;
}

void Actor::Move()
{
	int p = (SDL_GetTicks() - moveTime) / 3;
	if (type == AT_Enemy) //enemies move at different speeds based on level
		p = (int)(p * (0.5f + (level * 0.1f)));

	bool iact = true;
	switch (dir)
	{
	case D_Right: 
		x = p;
		iact = Interact(data::actors[pos + 1]); //double check to make sure nothing has moved to tile since start of move
		if (x >= data::tileSize && iact)
		{
			x = 0;
			data::actors[pos] = 0;
			pos++;
			data::actors[pos] = this;
			moveTime = 0;
		}
		else if (!iact)
			moveTime = 0;
		break;

	case D_Up:
		y = -p;
		iact = Interact(data::actors[pos - data::tileCount]);
		if (y <= -data::tileSize && iact)
		{
			y = 0;
			data::actors[pos] = 0;
			pos -= data::tileCount;
			data::actors[pos] = this;
			moveTime = 0;
		}
		else if (!iact)
			moveTime = 0;
		break;

	case D_Left:
		x = -p;
		iact = Interact(data::actors[pos - 1]);
		if (x <= -data::tileSize && iact)
		{
			x = 0;
			data::actors[pos] = 0;
			pos--;
			data::actors[pos] = this;
			moveTime = 0;
		}
		else if (!iact)
			moveTime = 0;
		break;

	case D_Down: 
		y = p;
		iact = Interact(data::actors[pos + data::tileCount]);
		if (y >= data::tileSize && iact)
		{
			y = 0;
			data::actors[pos] = 0;
			pos += data::tileCount;
			data::actors[pos] = this;
			moveTime = 0;
		}
		else if (!iact)
			moveTime = 0;
		break;
	}
}

bool Actor::Interact(Actor* obj)
{
	bool cont = true;

	if (obj != 0 && type == AT_Player) //player actions
		cont = PlayerInteract(this, obj);
	else if (type == AT_Pillar) //pillar colliding with other objects
	{
		if (obj != 0 && obj->type == AT_Enemy) //crush an enemy
		{
			data::actors[obj->pos] = MakeOther(AT_Rubble, obj->pos, D_Right);
			delete obj;
			data::score += data::enemyKillScore << 2;
		}
		else
		{
			int p = GetFacing();
			if (p > -1)
				data::actors[p] = MakeOther(AT_Rubble, p, D_Right);
		}
	}
	else if (obj != 0 && type == AT_Enemy) //enemies
	{
		if (obj->type == AT_Player) //kill player if colliding
		{
			if (obj->alive)
				KillPlayer();
			cont = false;
			moveTime = 0;
		}
		else
			cont = EnemyInteract(this, obj);
	}

	return cont;
}

Actor* Actor::MakeEnemy(int pos, Direction direction)
{
	Actor* a = new Actor();

	a->alive = true;
	a->type = AT_Enemy;
	a->assoc = rand() % 4;
	a->level = (rand() % 12) >> 1;
	a->pos = pos;
	a->dir = direction;
	a->frames = 3;
	a->loop = false;

	return a;
}

Actor* Actor::MakePlayer(int pos, Direction direction)
{
	Actor* a = new Actor();

	a->alive = true;
	a->type = AT_Player;
	a->pos = pos;
	a->dir = direction;
	a->anim = 0;
	a->frames = 3;
	a->loop = false;

	return a;
}

Actor* Actor::MakePillar(int pos)
{
	Actor* a = new Actor();

	a->alive = true;
	a->type = AT_Pillar;
	a->pos = pos;
	a->frames = 3;

	return a;
}

Actor* Actor::MakePowerup(ActorType type, int pos)
{
	Actor* a = new Actor();

	a->alive = true;
	a->type = type;
	a->pos = pos;
	a->assoc = 0;

	if (type == AT_Battery)
	{
		a->frames = 30;
		a->anim = SDL_GetTicks();
		a->loop = true;
		a->animSpd = 60;
	}
	else if (type == AT_Bomb)
	{
		a->frames = 80;
		a->animSpd = 40;
		a->loop = true;
		a->anim = SDL_GetTicks();
	}
	else if (type == AT_Nuke)
	{
		a->frames = 80;
		a->animSpd = 30;
		a->loop = true;
		a->anim = SDL_GetTicks();
	}

	return a;
}

Actor* Actor::MakeOther(ActorType type, int pos, Direction direction)
{
	Actor* a = new Actor();

	a->alive = true;
	a->type = type;
	a->pos = pos;
	a->dir = direction;

	return a;
}

void Actor::BombExplode(int pos, bool killPlayer)
{
	int time = SDL_GetTicks();

	if (data::actors[pos]->type == AT_Bomb)
	{
		delete data::actors[pos];
		data::actors[pos] = 0;
	}

	for (int x = pos % data::tileCount - 1; x < pos % data::tileCount + 2; x++)
		for (int y = pos / data::tileCount - 1; y < pos / data::tileCount + 2; y++)
		{
			if (x < 0 || y < 0 || x >= data::tileCount || y >= data::tileCount || data::actors[y * data::tileCount + x] == 0)
				continue;
			
			Actor* a = data::actors[y * data::tileCount + x];
			
			a->anim = time;

			if (a->type == AT_Player)
			{
				if (killPlayer)
					KillPlayer();
				continue;
			}
			else if (a->type == AT_Enemy)
				data::score += data::enemyKillScore;
			else if (a->type == AT_Pillar)
			{
				if (a->alive)
					data::score += data::pillarScore;
				else
					a->anim = 0;
			}
			else if (a->type == AT_Rubble)
			{
				data::actors[a->pos] = 0;
				delete a;
				continue;
			}
			
			a->alive = false;
		}

	int x = pos % data::tileCount * data::tileSize + (data::tileSize >> 1), y = pos / data::tileCount * data::tileSize + (data::tileSize >> 1);	
	data::particles->Emit(32, x, y, 5, PRT_Green);
	data::particles->Emit(32, x, y, 4, PRT_Red);
	data::particles->Emit(32, x, y, 3, PRT_Blue);

	for (int i = 0; i < 30; i++)
		MakeExplosion((ParticleColor)(i / 10), time, (i / 10) * 150, (int)(x + (i / 10 + 1) * 20 * cos(0.62832f * (i % 10))), (int)(y + (i / 10 + 1) * 20 * sin(0.62832f * (i % 10))));
}