#ifndef _CLOCK_H
#define _CLOCK_H

#include "../include/SDL.h"

class Clock {
  public:
    Clock();
    ~Clock();

    void Initialize();
    void Update();
    float GetTimeSinceAppStart();
    float GetDeltaTime();
    float GetAverageFrameTime();
    float GetFPS();
    unsigned int GetLastTick();

  private:
    unsigned int appStartTime;

    unsigned int currentTick;
    unsigned int lastTick;
    unsigned int fpsTick;

    static const unsigned int FPS_UPDATE_TICKS = 1000;

    static const unsigned int FRAME_HISTORY_LENGTH = 100;

    unsigned int frameHistoryPointer;
    unsigned int frameHistory[FRAME_HISTORY_LENGTH];

    float deltaTime;
    float fps;
    float averageFrameTime;

    void UpdateFPS();
};

#endif