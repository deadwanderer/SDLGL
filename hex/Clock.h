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
    
    private:
    unsigned int appStartTime;
    
    unsigned int currentTick;
    unsigned int lastTick;
    unsigned int fpsTick;
    
    unsigned int FPS_UPDATE_TICKS = 1000;
    
    unsigned int FRAME_HISTORY_LENGTH = 100;
    
    unsigned int frameHistoryPointer;
    unsigned int frameHistory[FRAME_HISTORY_LENGTH];
    
    float deltaTime;
    float fps;
    float averageFrameTime;
    
    void UpdateFPS();
};