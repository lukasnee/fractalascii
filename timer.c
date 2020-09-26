#include "timer.h"

void timerDelayMs(uint32_t ms)
{
    uint64_t delta_ms = 0;
    Timer delay;
    clock_gettime(CLOCK_MONOTONIC_RAW, &delay.start);
    while(delta_ms < ms)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &delay.end);
        delta_ms = (delay.end.tv_sec - delay.start.tv_sec) * 1000 + 
            (delay.end.tv_nsec - delay.start.tv_nsec) / 1000000;
    }
}

uint32_t timerGetFPS(Timer * pTimer, uint32_t * fFPS)
{
    Timer frame;
    clock_gettime(CLOCK_MONOTONIC_RAW, &frame.end);
    long fps = 1000000000/(frame.end.tv_nsec - frame.start.tv_nsec);
    frame.start = frame.end;
    return (uint32_t)fps;
}
