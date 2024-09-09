#include "../../includes/SoundManager.h"
#include <stdio.h>
#include <string.h>

// Sound variables
Mix_Music *music = NULL;
char soundDirectory[256] = "assets/sounds/";

int initSoundManager(const char *soundDir) {
    strncpy(soundDirectory, soundDir, sizeof(soundDirectory) - 1);
    soundDirectory[sizeof(soundDirectory) - 1] = '\0';

    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Failed to initialize SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    return 0;
}

void playSound(const char *filename) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", soundDirectory, filename);

    // Load the music file
    music = Mix_LoadMUS(path);
    if (music == NULL) {
        fprintf(stderr, "Failed to load sound: %s\n", Mix_GetError());
        return;
    }

    // Play the music
    if (Mix_PlayMusic(music, 1) == -1) {
        fprintf(stderr, "Failed to play sound: %s\n", Mix_GetError());
    }
}

void playShootSound() {
    playSound("shoot.mp3");
}

void playMainMenuSound() {
    playSound("main_menu.mp3");
}

void playInstructionSound() {
    playSound("instruction.mp3");
}

void playGameOverSound() {
    playSound("game_over.mp3");
}

void stopAllSounds() {
    Mix_HaltMusic();
}

void cleanupSoundManager() {
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
}