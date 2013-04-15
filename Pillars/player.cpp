#include "player.h"

void KillPlayer()
{
	data::playerLives--;
	data::player->alive = false;
	data::player->anim = SDL_GetTicks();

	delete data::bomb;
	data::bomb = 0;
	delete data::battery;
	data::battery = 0;
	delete data::nuke;
	data::nuke = 0;
}

bool PlayerInteract(Actor* player, Actor* obj)
{
	bool cont = true;

	if (obj->type == AT_Pillar && obj->alive) //knock a pillar down
	{
		obj->alive = false;
		obj->anim = SDL_GetTicks();
		data::score += data::pillarScore;

		obj->dir = player->dir;
		int p = obj->GetFacing();
		if (p != -1)
			obj->Interact(data::actors[p]);
		else
			data::actors[p] = Actor::MakeOther(AT_Rubble, p, obj->dir);	

		cont = false;
	}
	else if (obj->type == AT_Pillar && !obj->alive) //rubble/hole
	{
		if (obj->anim == 0 && player->alive)
			KillPlayer(); //hole
		else
			cont = false; //falling anim
	}
	else if (obj->type == AT_Rubble) //rubble (immovable)
		cont = false;
	else if (obj->type == AT_Enemy) //player runs into enemy, kill player
	{
		if (obj->alive && player->alive)
		{
			KillPlayer();
			cont = false;
		}
		else
		{
			if (!obj->alive)
			{
				data::actors[obj->pos] = 0;
				delete obj;
			}
			else
				cont = false;
		}
	}
	else if (obj->type == AT_Bomb) //player picks up bomb, active or inactive
	{
		if (data::bomb != 0)
			delete data::bomb;

		data::actors[obj->pos] = 0;
		obj->assoc = 0;
		obj->anim = 0;
		data::bomb = obj;
	}
	else if (obj->type == AT_Nuke) //player picks up bomb, inactive
	{
		if (obj->assoc != 0)
			cont = false;
		else
		{
			if (data::nuke != 0)
				delete data::nuke;

			data::actors[obj->pos] = 0;
			obj->assoc = 0;
			obj->anim = 0;
			data::nuke = obj;
		}
	}
	else if (obj->type == AT_Battery) //player picks up batter
	{
		if (data::battery != 0)
			delete data::battery;

		data::actors[obj->pos] = 0;
		obj->alive = false;
		obj->anim = 0;
		data::battery = obj;
	}

	return cont;
}