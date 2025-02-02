#include "pch.h"
#include "bench.h"

#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlgpu3.h"

BnContext* bnCreateContext(int width, int height)
{
    BnContext* ctx = new BnContext();
    ctx->width = width;
    ctx->height = height;

    ctx->window = SDL_CreateWindow("SDL Bench", width, height, SDL_WINDOW_RESIZABLE);
    if (!ctx->window)
    {
        bnLogError("SDL_CreateWindow failed: %s", SDL_GetError());
        bnDestroyContext(ctx);
        return nullptr;
    }

    ctx->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, false, "direct3d12");
    if (!ctx->device)
    {
        bnLogError("SDL_CreateGPUDevice failed: %s", SDL_GetError());
        bnDestroyContext(ctx);
        return nullptr;
    }

    ctx->windowClaimed = SDL_ClaimWindowForGPUDevice(ctx->device, ctx->window);
    if (!ctx->windowClaimed)
    {
        bnLogError("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
        bnDestroyContext(ctx);
        return nullptr;
    }

    SDL_SetGPUSwapchainParameters(ctx->device, ctx->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

    ctx->imguiContext = ImGui::CreateContext();
    if (!ctx->imguiContext)
    {
        bnLogError("ImGui::CreateContext failed");
        bnDestroyContext(ctx);
        return nullptr;
    }

    ImGui_ImplSDLGPU3_InitInfo info = {};
    info.Device = ctx->device;
    info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(ctx->device, ctx->window);
    info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;

    ImGui_ImplSDLGPU3_Init(&info);
    ImGui_ImplSDL3_InitForSDLGPU(ctx->window);

    return ctx;
}

void bnDestroyContext(BnContext* ctx)
{
    if (!ctx)
    {
        return;
    }

    // wait device idle
    if (ctx->device)
    {
        SDL_WaitForGPUIdle(ctx->device);
    }

    if (ctx->imguiContext)
    {
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext(ctx->imguiContext);
    }

    if (ctx->windowClaimed)
    {
        bnLogInfo("Releasing swap chain...");
        SDL_ReleaseWindowFromGPUDevice(ctx->device, ctx->window);
    }

    if (ctx->device)
    {
        bnLogInfo("Destroying GPU device...");
        SDL_DestroyGPUDevice(ctx->device);
    }

    if (ctx->window)
    {
        bnLogInfo("Destroying window...");
        SDL_DestroyWindow(ctx->window);
    }

    delete ctx;
}

static void bnHandleResize(BnContext* ctx, int width, int height)
{
    ctx->width = width;
    ctx->height = height;
}

static void bnHandleEvent(BnContext* ctx, const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);

    switch (event.type)
    {
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        ctx->running = false;
        return;
    case SDL_EVENT_WINDOW_RESIZED:
        bnHandleResize(ctx, event.window.data1, event.window.data2);
        return;
    default:
        return;
    }
}

int bnRun(BnContext* ctx)
{
    if (!ctx)
    {
        bnLogError("Invalid context");
        return -1;
    }

    while (ctx->running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            bnHandleEvent(ctx, event);
        }

        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(nullptr);

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();

        {
            SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(ctx->device);

            SDL_GPUTexture* backBuffer;
            SDL_AcquireGPUSwapchainTexture(cmdBuf, ctx->window, &backBuffer, nullptr, nullptr);

            if (backBuffer)
            {
                Imgui_ImplSDLGPU3_PrepareDrawData(drawData, cmdBuf);

                SDL_GPUColorTargetInfo tgtInfo = {};
                tgtInfo.texture = backBuffer;
                tgtInfo.clear_color = SDL_FColor{ 0.4f, 0.4f, 0.4f, 1.0f };
                tgtInfo.load_op = SDL_GPU_LOADOP_CLEAR;
                tgtInfo.store_op = SDL_GPU_STOREOP_STORE;
                tgtInfo.mip_level = 0;
                tgtInfo.layer_or_depth_plane = 0;
                tgtInfo.cycle = false;

                SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuf, &tgtInfo, 1, nullptr);
                ImGui_ImplSDLGPU3_RenderDrawData(drawData, cmdBuf, renderPass);
                SDL_EndGPURenderPass(renderPass);
            }

            SDL_SubmitGPUCommandBuffer(cmdBuf);
        }
    }

    return 0;
}
