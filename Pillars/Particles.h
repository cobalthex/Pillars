#pragma once

#include "func.h"
#include "sprites.h"

enum ParticleColor : char
{
	PRT_Red,
	PRT_Blue,
	PRT_Green
};

typedef struct 
{
	short x, y; 
	ParticleColor color; 
	float vx, vy; 
	bool alive; 
} particle;

class Particles
{
public:

	Particles();
	Particles(int maxParticles, SDL_Rect* screenBounds);
	~Particles();

	void Emit(int count, short x, short y, float v, ParticleColor color);
	void Clear();

	void Update(long time);
	void Draw(SDL_Surface* s);

	SDL_Rect* bounds;
	float decay;
	int freq;

private:
	int pCount;
	particle* particles; 
	short counter;
};