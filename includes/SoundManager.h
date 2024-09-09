#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// Function declarations
int initSoundManager(const char *soundDir);
void playShootSound();
void playMainMenuSound();
void playInstructionSound();
void playGameOverSound();
void stopAllSounds();
void cleanupSoundManager();

#endif // SOUNDMANAGER_H