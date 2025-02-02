#pragma once

struct BnContext
{
    SDL_Window* window = nullptr;
    bool running = true;
    int width = 0;
    int height = 0;

    SDL_GPUDevice* device = nullptr;
    bool windowClaimed = false;

    ImGuiContext* imguiContext = nullptr;
};

BnContext* bnCreateContext(int width, int height);
void bnDestroyContext(BnContext* context);
int bnRun(BnContext* context);


