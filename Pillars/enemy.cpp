#include "enemy.h"

void EnemyThink(Actor* enemy)
{
	long time = SDL_GetTicks();

	int px = data::player->pos % data::tileCount, py = data::player->pos / data::tileCount;
	int ex = enemy->pos % data::tileCount, ey = enemy->pos / data::tileCount;

	//check if in line with player
	if (px == ex)
	{
		if (py < ey && enemy->moveTime == 0 && CanSee(enemy, D_Up))
		{
			enemy->dir = D_Up;
			int act = enemy->GetFacing();
			if (act > -1 && enemy->Interact(data::actors[act]))
				enemy->moveTime = time;
		}
		else if (py > ey && enemy->moveTime == 0 && CanSee(enemy, D_Down))
		{
			enemy->dir = D_Down;
			int act = enemy->GetFacing();
			if (act > -1 && enemy->Interact(data::actors[act]))
				enemy->moveTime = time;
		}
	}
	if (py == ey)
	{
		if (px < ex && enemy->moveTime == 0 && CanSee(enemy, D_Left))
		{
			enemy->dir = D_Left;
			int act = enemy->GetFacing();
			if (act > -1 && enemy->Interact(data::actors[act]))
				enemy->moveTime = time;
		}
		else if (px > ex && enemy->moveTime == 0 && CanSee(enemy, D_Right))
		{
			enemy->dir = D_Right;
			int act = enemy->GetFacing();
			if (act > -1 && enemy->Interact(data::actors[act]))
				enemy->moveTime = time;
		}
	}

	//can't directly see
	int act = enemy->GetFacing();
	if (enemy->moveTime == 0)
	{
		if (act > -1 && enemy->level == 5 && data::actors[act] != 0) //super enemy can plow through rubble and will blow up if near player
		{
			if (data::actors[act]->type == AT_Rubble)
			{
				enemy->moveTime = time;
				delete data::actors[act];
				data::actors[act] = 0;
			}

			int dx = px - ex;
			int dy = py - ey;
			if ((dx * dx) + (dy * dy) < 10 && enemy->alive)
				Actor::BombExplode(enemy->pos);
		}

		if (rand() % 20 == 0)
			ChangeDir(enemy);
		else if (act > -1 && enemy->Interact(data::actors[act]))
			enemy->moveTime = time;
		else if (act > -1 && data::actors[act]->type == AT_Pillar)
		{
			if (!data::actors[act]->alive && rand() % 16 == 1 && enemy->level < 4) //1/16th chance of falling into hole (if lower level enemy)
			{
				enemy->alive = false;
				enemy->anim = SDL_GetTicks();
			}
			else
				ChangeDir(enemy);
		}
		else if (act == -1)
			ChangeDir(enemy);
	}
}

void ChangeDir(Actor* a)
{
	//random direction if hit wall
	if (a->level == 0)
		a->dir = (Direction)(rand() % 4);
	else if (a->level == 1)
		a->dir = (Direction)((a->dir + 1) % 4);
	else if (a->level == 2)
		a->dir = (Direction)((a->dir - 1) < 0 ? 3 : a->dir - 1);
	//if skilled, move in direction of player
	else
	{
		int dx = (data::player->pos % data::tileCount) - (a->pos % data::tileCount);
		int dy = (data::player->pos / data::tileCount) - (a->pos / data::tileCount);

		if (abs(dx) > abs(dy))
		{
			if (dx > 0)
				a->dir = D_Right;
			else 
				a->dir = D_Left;
		}
		else
		{
			if (dy > 0)
				a->dir = D_Down;
			else 
				a->dir = D_Up;
		}
	}
}

bool CanSee(Actor* a, Direction d)
{
	if (d == D_Right)
	{
		if (a->pos % data::tileCount == data::tileCount - 1)
			return false;
		for (int i = a->pos + 1; i < a->pos / data::tileCount * data::tileCount + (data::tileCount - 1); i++)
			if (data::actors[i] != 0 && data::actors[i] != data::player)
				return false;
	}
	else if (d == D_Left)
	{
		if (a->pos % data::tileCount == 0)
			return false;
		for (int i = a->pos - 1; i >= a->pos / data::tileCount * data::tileCount && i % data::tileCount < a->pos % data::tileCount; i--)
			if (data::actors[i] != 0 && data::actors[i] != data::player)
				return false;
	}
	else if (d == D_Up)
	{
		if (a->pos / data::tileCount == 0)
			return false;
		for (int i = a->pos - data::tileCount; i / data::tileCount >= 0 && i >= 0; i -= data::tileCount)
			if (data::actors[i] != 0 && data::actors[i] != data::player)
				return false;
	}
	else if (d == D_Down)
	{
		if (a->pos / data::tileCount == data::tileCount - 1)
			return false;
		for (int i = a->pos + data::tileCount; i / data::tileCount < data::tileCount - 1; i += data::tileCount)
			if (data::actors[i] != 0 && data::actors[i] != data::player)
				return false;
	}

	return true;
}

bool EnemyInteract(Actor* enemy, Actor* obj)
{
	bool cont = true;

	if (obj->type == AT_Pillar)
	{
		if (enemy->level == 5)
		{
			/*
			int act = obj->GetFacing();
			if (act > -1)
				obj->Interact(data::actors[act]);
			*/
			cont = false;
		}
		else
			cont = false;
	}
	else if (obj->type == AT_Bomb)
	{
		if (obj->assoc > 0)
		{
			Actor::BombExplode(obj->pos);
			cont = false;
		}
		else
		{
			data::actors[obj->pos] = 0;
			delete obj;
		}
	}
	else if (obj->type == AT_Nuke || obj->type == AT_Battery)
	{
		data::actors[obj->pos] = 0;
		delete obj;
	}
	else if (obj->type == AT_Enemy || obj->type == AT_Rubble)
	{
		//switch directions
		if (enemy->dir == D_Left)
			enemy->dir = D_Right;
		else if (enemy->dir == D_Right)
			enemy->dir = D_Left;
		else if (enemy->dir == D_Up)
			enemy->dir = D_Down;
		else if (enemy->dir == D_Down)
			enemy->dir = D_Up;

		cont = false;
	}

	return cont;
}