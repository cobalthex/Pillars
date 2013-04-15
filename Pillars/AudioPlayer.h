#pragma once

#include <SDL/SDL.h>

class AudioPlayer
{
private:
	static SDL_AudioSpec spec;
    static Uint8* sound;
    static Uint32 soundLen;
    static int soundPos;
	static bool isLooping; 
public:

	static bool isPlaying;

	static void PlayWAV(const unsigned char* audio, bool loop);
	static void PlayMP3(const unsigned char* audio, const unsigned int length, bool loop);
	static void Stop();
	static void MixWAV(void *udata, Uint8 *stream, int len);
};