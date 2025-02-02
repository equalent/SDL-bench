#include "pch.h"
#include "bench.h"
#include <SDL3/SDL_main.h>

#ifdef _WIN32

#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#endif

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
