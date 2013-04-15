#pragma once
#include "Actor.h"
#include "Particles.h"

typedef struct
{
	ParticleColor color;
	long time;
	long delay;
	int x, y;
} Explosion;

Explosion* MakeExplosion(ParticleColor color, long time, long delay, int x, int y);

enum ShotType : char
{
	ST_None,
	ST_Flame,
	ST_Rocket,
	ST_Shard
};

typedef struct
{
	//ShotType type; <----- hard coded into shots list
	int x, y;
	int ox, oy; //origin
	long time; //creation time <--- dictates position
	Direction dir;
} Shot;

Shot* MakeShot(int x, int y, Direction dir);
bool MoveShot(Shot* shot, ShotType type);
bool CheckShot(Shot* shot, ShotType type);

typedef Actor* Bomb;
typedef Actor* Nuke;
typedef Actor* Battery;