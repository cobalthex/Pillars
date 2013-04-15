#include "Particles.h"
#include <cmath>
#define TWO_PI 6.283185f

Particles::Particles() : pCount(0), counter(0), bounds(0), particles(0), decay(0), freq(1) { }

Particles::Particles(int maxParticles, SDL_Rect* screenBounds) : pCount(maxParticles), counter(0), bounds(screenBounds)
{
	particles = (particle*)calloc(maxParticles, sizeof(particle));
	decay = 0.2f;
	freq = 60;
}

Particles::~Particles()
{
	counter = 0;
	pCount = 0;
	free(particles);
}

void Particles::Emit(int n, short x, short y, float v, ParticleColor color)
{
	float a = TWO_PI / n;
	for (int i = 0; i < n; i++)
	{
		++counter %= pCount;
		particles[counter].alive = true;
		particles[counter].color = color;
		particles[counter].x = x;
		particles[counter].y = y;
		particles[counter].vx = v * cos(a * i);
		particles[counter].vy = v * sin(a * i);
	}
}

void Particles::Clear()
{
	memset(particles, 0, sizeof(particle) * pCount);
	counter = 0;
}

void Particles::Update(long time)
{
	if (time % 4 > 0)
		return;

	for (int i = 0; i < pCount; i++)
	{
		particle* p = &particles[i];

		if (!p->alive)
			continue;

		//move by velocity
		p->x += (short)p->vx;
		p->y += (short)p->vy;

		//decrease velocity every call
		if (p->vx < 0)
			p->vx += decay;
		else if (p->vx > 0)
			p->vx -= decay;

		if (p->vy < 0)
			p->vy += decay;
		else if (p->vy > 0)
			p->vy -= decay;

		if (p->x < bounds->x || p->x > bounds->w || p->y < bounds->y || p->y > bounds->h || (abs(p->vx) <= decay && abs(p->vy) <= decay))
			p->alive = false;
	}
}

void Particles::Draw(SDL_Surface* s)
{
	for (int i = 0; i < pCount; i++)
	{
		particle* p = &particles[i];

		if (!p->alive)
			continue;

		DrawImg(s, sprites_img, sprites_pal, p->x, p->y, 120 + p->color * 5, 0, 4, 4, sprites_w);
	}
}
