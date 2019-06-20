#include "Clock.h"

Clock::Clock() {
    frameHistoryPointer = 0;
    appStartTime = 0;
    currentTick = 0;
    lastTick = 0;
    for (unsigned int i = 0; i < FRAME_HISTORY_LENGTH; i++) {
        frameHistory[i] = 0;
    }
    deltaTime = 0.0f;
}

Clock::~Clock() {}

void Clock::Initialize() {
    appStartTime = lastTick = currentTick = SDL_GetTicks();
}

void Clock::Update() {
    lastTick = currentTick;
    currentTick = SDL_GetTicks();
    unsigned int deltaTicks = currentTick - lastTick;
    fpsTick += deltaTicks;

    deltaTime = (float)(deltaTicks) / 1000.0f;

    frameHistory[frameHistoryPointer++] = deltaTicks;
    if (frameHistoryPointer >= FRAME_HISTORY_LENGTH) {
        frameHistoryPointer = 0;
    }

    if (fpsTick >= FPS_UPDATE_TICKS) {
        fpsTick -= FPS_UPDATE_TICKS;
        UpdateFPS();
    }
}

float Clock::GetTimeSinceAppStart() {
    return (float)(currentTick - appStartTime) / 1000.0f;
}

float Clock::GetDeltaTime() {
    return deltaTime;
}

float Clock::GetAverageFrameTime() {
    return averageFrameTime;
}

float Clock::GetFPS() {
    return fps;
}

unsigned int Clock::GetLastTick() {
    return frameHistory[frameHistoryPointer];
}

void Clock::UpdateFPS() {
    unsigned int sumTimes = 0;
    unsigned int timeCounts = 0;
    for (unsigned int i = 0; i < FRAME_HISTORY_LENGTH; i++) {
        if (frameHistory[i] == 0)
            continue;
        sumTimes += frameHistory[i];
        timeCounts++;
    }
    averageFrameTime = (float)sumTimes / (float)timeCounts;
    fps = 1000.0f / averageFrameTime;
}