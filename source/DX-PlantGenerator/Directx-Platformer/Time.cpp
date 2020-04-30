#include "Time.h"

Time::Time()
{
    for(int i = 0; i <10; i++)
        fpsMoveAvr.push_back(60);
}

void Time::Update()
{
    using clock = std::chrono::steady_clock;

    float timeSinceLastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - timeOfLastFrame).count();
    timeOfLastFrame = clock::now();

    fpsMoveAvr[avrIndex] = (1000. / timeSinceLastFrame);
    avrIndex++;
    if (avrIndex == 10)
        avrIndex = 0;
}

float Time::DeltaTime()
{
    return timeSinceLastFrame;
}

int Time::FPS()
{
    float total = 0;
    for (float val : fpsMoveAvr)
    {
        total += val;
    }
    return total / 10.;
}