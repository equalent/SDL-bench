#include "pch.h"
#include "bench.h"
#include <SDL3/SDL_main.h>

int main(int argc, char** argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        bnLogCritical("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    BnContext* context = bnCreateContext(800, 600);
    if (!context)
    {
        bnLogCritical("bnCreateContext failed");
        return -1;
    }

    int returnCode = bnRun(context); 

    bnDestroyContext(context);

    SDL_Quit();
    return returnCode;
}
