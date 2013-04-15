#pragma once

enum ActorType
{
	AT_None,
	AT_Player,
	AT_Pillar,
	AT_Rubble,
	AT_Enemy,
	AT_Battery,
	AT_Bomb,
	AT_Nuke
};

enum Direction : char
{
	D_Right,
	D_Up,
	D_Left,
	D_Down
};

class Actor
{
public:
	bool alive; //is the actor alive
	ActorType type;
	long assoc; //specific to this actor (color for enemies, etc)
	char level; //skill level
	int pos; //position (cooresponds to the actor array index)
	int x, y; //x & y position (in pixels offset from the position)
	long moveTime; //time when actor started moving, dictates position
	Direction dir; //direction of object (right being 0, up being 1, left being 2, down being 3)
	long anim; //animation start time
	int frames; //number of fraemes in the animation
	int animSpd; //speed of animation in ms (defaults to 100ms)
	bool loop; //loop the animation

	Actor();

	void Move();
	bool Interact(Actor* object);
	int GetFacing(); //get the position of the actor that this actor is facing, -1 if none
	
	static Actor* MakeEnemy(int position, Direction direction);
	static Actor* MakePlayer(int position, Direction direction);
	static Actor* MakePillar(int position);
	static Actor* MakePowerup(ActorType type, int position);
	static Actor* MakeOther(ActorType type, int position, Direction direction);
	static void BombExplode(int pos, bool killPlayer = false);
};