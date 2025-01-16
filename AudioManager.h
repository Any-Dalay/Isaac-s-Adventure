#ifndef AUDIOMANAGER_H_INCLUDED
#define AUDIOMANAGER_H_INCLUDED
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <stdio.h>
#pragma comment(lib, "winmm.lib")

class AudioManager {
private:
    std::string currentFile;
    bool isPlaying;

public:
    AudioManager() : isPlaying(false) {}

    bool playBackgroundMusic(const char* filename) {
        char command[256];

        // Si hay música reproduciéndose, la detenemos
        if (isPlaying) {
            stopBackgroundMusic();
        }

        // Abrimos el archivo de audio
        sprintf(command, "open \"%s\" type mpegvideo alias bgMusic", filename);
        if (mciSendStringA(command, NULL, 0, NULL) != 0) {
            return false;
        }

        // Reproducimos en loop
        if (mciSendStringA("play bgMusic repeat", NULL, 0, NULL) != 0) {
            return false;
        }

        currentFile = filename;
        isPlaying = true;
        return true;
    }

    void stopBackgroundMusic() {
        if (isPlaying) {
            mciSendStringA("stop bgMusic", NULL, 0, NULL);
            mciSendStringA("close bgMusic", NULL, 0, NULL);
            isPlaying = false;
        }
    }

    void pauseBackgroundMusic() {
        if (isPlaying) {
            mciSendStringA("pause bgMusic", NULL, 0, NULL);
        }
    }

    void resumeBackgroundMusic() {
        if (isPlaying) {
            mciSendStringA("resume bgMusic", NULL, 0, NULL);
        }
    }

    void setVolume(int volume) {  // volume debe estar entre 0 y 1000
        if (isPlaying) {
            char command[256];
            sprintf(command, "setaudio bgMusic volume to %d", volume);
            mciSendStringA(command, NULL, 0, NULL);
        }
    }

    ~AudioManager() {
        stopBackgroundMusic();
    }
};


#endif // AUDIOMANAGER_H_INCLUDED
