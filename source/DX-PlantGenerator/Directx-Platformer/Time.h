#pragma once
#include <chrono>
#include <vector>
class Application;

class Time
{
public:
    friend class Application;
    float DeltaTime();
    int FPS();
private:
    Time();
    void Update();

    std::chrono::steady_clock::time_point timeOfLastFrame = std::chrono::steady_clock::now();
    float timeSinceLastFrame = 0;
    std::vector<float> fpsMoveAvr;
    int avrIndex = 0;
    
};

