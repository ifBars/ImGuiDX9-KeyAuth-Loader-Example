#pragma once
#include <d3d9.h>
#include <chrono>
#include <DirectXTex.h>

#include "Themes.h"
#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"

namespace Gui
{
    struct Bubble {
        ImVec2 position;
        ImVec2 velocity;
        float radius;
        ImVec4 color;
    };

    inline Bubble bubbles[50];
    inline int bubbleCount = 50;
    inline float speedMultiplier = 0.3f;
    inline ImVec4 bubbleColor = ImVec4(.22f, .22f, .22f, .5f);

    void InitializeBubbles();
    void RenderBubbles();

    // Constant window size
    inline int WIDTH = 400;
    inline int HEIGHT = 300;
    inline int MAXWIDTH = 600;
    inline int MAXHEIGHT = 450;
    inline int CURRENTWIDTH = 300;
    inline int CURRENTHEIGHT = 200;

    // When this changes, exit threads
    // and closes app :)
    inline bool isRunning = true;

    // fonts
    inline ImFont* regularFont;
    inline ImFont* titleFont;

    // images
    inline IDirect3DTexture9* LoaderIcon;
    inline IDirect3DTexture9* CS2Image;

    // winapi window vars
    inline HWND window = nullptr;
    inline WNDCLASSEX windowClass = { };

    // Points for window movement
    inline POINTS position = { };

    // DirectX state vars
    inline PDIRECT3D9 d3d = nullptr;
    inline LPDIRECT3DDEVICE9 device = nullptr;
    inline D3DPRESENT_PARAMETERS presentParameters = { };


    inline const char* windowName;

    inline static ImVec4 rainbowColors[] = {
    ImVec4(1.0f, 0.0f, 0.0f, 1.0f), // Red
    ImVec4(1.0f, 0.5f, 0.0f, 1.0f), // Orange
    ImVec4(1.0f, 1.0f, 0.0f, 1.0f), // Yellow
    ImVec4(0.0f, 1.0f, 0.0f, 1.0f), // Green
    ImVec4(0.0f, 0.0f, 1.0f, 1.0f), // Blue
    ImVec4(0.5f, 0.0f, 1.0f, 1.0f), // Purple
    };

    inline static int currentColorIndex = 0;

    IDirect3DTexture9* LoadImageToTexture(const unsigned char* image_data, size_t image_size, bool use_alpha = false) noexcept;

    // Handle window creation & destruction
    void CreateHWindow(const char* inWindowName, int inWidth = WIDTH, int inHeight = HEIGHT) noexcept;
    void DestroyHWindow() noexcept;

    // Handle device creation & destruction
    bool CreateDevice() noexcept;
    void ResetDevice() noexcept;
    void DestroyDevice() noexcept;

    // Handle ImGui creation & destruction
    void CreateImGui() noexcept;
    void DestroyImGui() noexcept;

    // Rendering
    void BeginRender() noexcept;
    void EndRender() noexcept;
    void BeginImGuiRender() noexcept;
    void EndImGuiRender() noexcept;

    void DrawRainbowBar(float width, float height) noexcept;
}
