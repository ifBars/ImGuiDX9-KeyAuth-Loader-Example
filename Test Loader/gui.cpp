#include "gui.h"
#include "Font.h"
#pragma comment(lib, "d3d9.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (Gui::device && wideParameter != SIZE_MINIMIZED)
		{
			Gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			Gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			Gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		Gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(Gui::window, &rect);

			rect.left += points.x - Gui::position.x;
			rect.top += points.y - Gui::position.y;

			if (Gui::position.x >= 0 &&
				Gui::position.x <= Gui::WIDTH &&
				Gui::position.y >= 0 && Gui::position.y <= 19)
				SetWindowPos(
					Gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return long(DefWindowProc(window, message, wideParameter, longParameter));
}

void Gui::CreateHWindow(const char* inWindowName, int inWidth, int inHeight) noexcept
{
	WIDTH = inWidth;
	HEIGHT = inHeight;

	Gui::windowName = inWindowName;
	wchar_t* wString = new wchar_t[strlen(windowName)];
	MultiByteToWideChar(CP_ACP, 0, windowName, -1, wString, 4096);

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WNDPROC(WindowProcess);
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = wString;
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		WS_EX_LAYERED,
		wString,
		wString,
		WS_POPUP | WS_VISIBLE,
		100,
		100,
		MAXWIDTH,
		MAXHEIGHT,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);
	SetLayeredWindowAttributes(window, 0, 0, LWA_ALPHA);
	SetLayeredWindowAttributes(window, 0, RGB(0, 0, 0), LWA_COLORKEY);

	ShowWindow(window, SW_SHOWNORMAL);
	UpdateWindow(window);
}

void Gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

// Vertex Shader
const char* vsCode = R"(
    float4 mainVS(float4 position : POSITION, float2 texCoord : TEXCOORD0) : POSITION0
    {
        return mul(position, matWorldViewProj);
    }
)";

// Pixel Shader
const char* psCode = R"(
    float4 mainPS(float2 texCoord : TEXCOORD0) : COLOR0
    {
        color.rgb = pow(color.rgb, 2.2); // Gamma correction
		return color;
    }
)";

void Gui::RenderBubbles() {
	for (int i = 0; i < bubbleCount; ++i) {

		bubbles[i].position.x += bubbles[i].velocity.x * speedMultiplier;
		bubbles[i].position.y += bubbles[i].velocity.y * speedMultiplier;

		// Check boundaries and bounce back if needed
		if (bubbles[i].position.x < 0 || bubbles[i].position.x > CURRENTWIDTH) {
			bubbles[i].velocity.x = -bubbles[i].velocity.x;
		}
		if (bubbles[i].position.y < 0 || bubbles[i].position.y > CURRENTHEIGHT) {
			bubbles[i].velocity.y = -bubbles[i].velocity.y;
		}

		ImGui::GetWindowDrawList()->AddCircleFilled(bubbles[i].position, bubbles[i].radius,
			IM_COL32(
				int(bubbles[i].color.x * 255),
				int(bubbles[i].color.y * 255),
				int(bubbles[i].color.z * 255),
				int(bubbles[i].color.w * 255)
			));
	}
}

void Gui::InitializeBubbles() {
	for (int i = 0; i < bubbleCount; ++i) {
		bubbles[i].position = ImVec2(rand() % CURRENTWIDTH, rand() % CURRENTHEIGHT);
		bubbles[i].velocity = ImVec2((rand() % 13 - 8),
			(rand() % 13 - 8));
		bubbles[i].radius = static_cast<float>(rand() % 15 + 10);
		bubbles[i].color = ImColor(bubbleColor);
	}
}

bool Gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void Gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void Gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void Gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Load fonts
	ImGuiIO& io = ::ImGui::GetIO();
	ImFontConfig config;
	config.FontDataOwnedByAtlas = false;  // Important if you free the memory yourself
	regularFont = io.Fonts->AddFontFromMemoryCompressedTTF(Font::FontData_compressed_data, Font::FontData_compressed_size, 16.f, &config);
	titleFont = io.Fonts->AddFontFromMemoryCompressedTTF(Font::FontData_compressed_data, Font::FontData_compressed_size, 26.f, &config);

	LoaderIcon = LoadImageToTexture(Font::LoaderIcon, sizeof(Font::LoaderIcon), true);
	CS2Image = LoadImageToTexture(Font::CS2ImageData, sizeof(Font::CS2ImageData), true);

	// Set texture stage states for sRGB sampling
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, TRUE);

	LPDIRECT3DVERTEXSHADER9 pVertexShader = nullptr;
	device->CreateVertexShader(reinterpret_cast<const DWORD*>(vsCode), &pVertexShader);

	LPDIRECT3DPIXELSHADER9 pPixelShader = nullptr;
	device->CreatePixelShader(reinterpret_cast<const DWORD*>(psCode), &pPixelShader);

	// Set shaders
	device->SetVertexShader(pVertexShader);
	device->SetPixelShader(pPixelShader);

	io.IniFilename = NULL;

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

IDirect3DTexture9* Gui::LoadImageToTexture(const unsigned char* image_data, size_t image_size, bool use_alpha) noexcept {
	IDirect3DTexture9* g_Texture = nullptr;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;
	HRESULT hr = DirectX::LoadFromWICMemory(
		image_data,             // Pointer to your raw array
		image_size,             // Size of the raw array
		DirectX::WIC_FLAGS_NONE,// WIC flags
		&metadata,              // Output metadata
		scratchImage            // Output image
	);

	D3DFORMAT textureFormat = use_alpha ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;

	HRESULT hr2 = Gui::device->CreateTexture(
		metadata.width,
		metadata.height,
		1,
		D3DUSAGE_AUTOGENMIPMAP,
		textureFormat,
		D3DPOOL_MANAGED,
		&g_Texture,
		nullptr
	);

	if (SUCCEEDED(hr2)) {
		// Update texture data
		D3DLOCKED_RECT lockedRect;
		g_Texture->LockRect(0, &lockedRect, nullptr, 0);
		memcpy(lockedRect.pBits, scratchImage.GetPixels(), scratchImage.GetPixelsSize());
		g_Texture->UnlockRect(0);
	}

	return g_Texture;
}

void Gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

static std::chrono::high_resolution_clock::time_point lastColorChangeTime;

void Gui::DrawRainbowBar(float width, float height) noexcept
{
	float speedMultiplier = .5f;
	ImVec2 barSize(width, height);

	// Calculate elapsed time since the last color change
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto elapsedSeconds = std::chrono::duration<float>(currentTime - lastColorChangeTime).count();

	// Update color index if enough time has passed
	if (elapsedSeconds > speedMultiplier) // Adjust the duration based on your preference
	{
		currentColorIndex = (currentColorIndex + 1) % IM_ARRAYSIZE(rainbowColors);
		lastColorChangeTime = currentTime;
	}

	// Interpolate between current and next color based on elapsed time
	ImVec4 currentColor = rainbowColors[currentColorIndex];
	ImVec4 nextColor = rainbowColors[(currentColorIndex + 1) % IM_ARRAYSIZE(rainbowColors)];
	ImVec4 interpolatedColor;

	float t = fmod(elapsedSeconds / speedMultiplier, 1.0f); // Adjust the duration here as well
	interpolatedColor.x = currentColor.x + t * (nextColor.x - currentColor.x);
	interpolatedColor.y = currentColor.y + t * (nextColor.y - currentColor.y);
	interpolatedColor.z = currentColor.z + t * (nextColor.z - currentColor.z);
	interpolatedColor.w = currentColor.w + t * (nextColor.w - currentColor.w);

	// Draw the rainbow bar with the interpolated color
	ImGui::PushStyleColor(ImGuiCol_Button, interpolatedColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, interpolatedColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, interpolatedColor);
	ImGui::Button(" ", barSize);
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

void Gui::BeginImGuiRender() noexcept
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(CURRENTWIDTH), static_cast<float>(CURRENTHEIGHT)));
	ImGui::Begin(
		Gui::windowName,
		&isRunning,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
	);
}

void Gui::EndImGuiRender() noexcept
{
	ImGui::End();
}
