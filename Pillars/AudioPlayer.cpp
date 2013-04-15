#include "AudioPlayer.h"

SDL_AudioSpec AudioPlayer::spec;
Uint8* AudioPlayer::sound;
Uint32 AudioPlayer::soundLen;
int AudioPlayer::soundPos;
bool AudioPlayer::isLooping;
bool AudioPlayer::isPlaying;

void AudioPlayer::PlayWAV(const unsigned char* Audio, bool Loop)
{	
	soundPos = 0;
	isLooping = Loop;
	isPlaying = true;

	spec.samples = 4096;
	spec.callback = MixWAV;

	//get song info from wav header
	memcpy(&soundLen, Audio + 4, 4);
	memcpy(&spec.channels, Audio + 22, 1);
	memcpy(&spec.freq, Audio + 24, 4);

	sound = (Uint8*)Audio + 44;

	if (SDL_OpenAudio(&spec, 0) == 0)
		SDL_PauseAudio(0);
}

void AudioPlayer::Stop()
{
	isPlaying = false;
	SDL_CloseAudio();
	soundPos = 0;
	soundLen = 0;
	sound = 0;
	memset(&spec, 0, sizeof(SDL_AudioSpec));
}

void AudioPlayer::MixWAV(void *udata, Uint8 *stream, int len) //original from SDL example
{
	Uint8 *waveptr;
    int waveleft;

    waveptr = sound + soundPos; //44 bytes for wav header
    waveleft = soundLen - soundPos;

    while (waveleft <= len)
	{
        SDL_memcpy(stream, waveptr, waveleft);
        stream += waveleft;
        len -= waveleft;
        waveptr = sound;
        waveleft = soundLen;
        soundPos = 0;

		if (!isLooping)
			Stop();
    }

    SDL_memcpy(stream, waveptr, len);
    soundPos += len;
}