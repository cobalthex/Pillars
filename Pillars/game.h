#pragma once

#include <ctime>
#include <SDL/SDL.h>
#include "Actor.h"
#include "types.h"
#include "Particles.h"
#include "InputHandler.h"
#include "music.h"

//All of the states of this game
enum GameStates
{
	GS_None = 0,
	GS_Intro,
	GS_Menu,
	GS_Options,
	GS_Credits,
	GS_Play,
	GS_Pause,
	GS_Win,
	GS_Scores
};

//all of the global data in the game
namespace data
{
	extern InputHandler input;
	extern bool isFullscreen;
	extern bool isAudioPlaying;
	extern unsigned int level;

	extern const int tileCount;
	extern const int tileSize;

	extern long timer;
	extern GameStates currentState;
	extern bool isGamePlaying;

	extern Actor** actors;
	extern Actor* player;
	extern int playerLives;

	extern int score;
	extern int visScore;

	extern int top;
	extern int visTop;

	extern char msg[];

	extern Shot* shots[3];
	extern const int maxExplos;
	extern Explosion* xplos[];

	extern Particles* particles;

	extern Battery battery;
	extern Bomb bomb;
	extern Nuke nuke;

	extern int flameShots;
	extern int rocketShots;
	extern int shardShots;
	extern ShotType curShot;
	extern long shotTime;
	extern const short flameSpd;
	extern const short rocketSpd;
	extern const short shardSpd;

	extern bool exiting;

	extern const int enemyKillScore;
	extern const int pillarScore;

	extern int currentSong;
	extern const int songCount;
};

void NewGame();
void CreateLevel();

bool update();
void init(SDL_Surface* screen);
void draw(SDL_Surface* screen);
void end();