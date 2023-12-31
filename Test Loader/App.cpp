#include "App.h"

#include <thread>

#include "../MainApp/MainApp.h"
#include "Gui.h"
#include "Themes.h"
#include "../auth.hpp"
#include "../skStr.h"

using namespace std;
using namespace KeyAuth;

App::App(std::string _appName, int _width, int _height) : appName(std::move(_appName)), windowWidth(_width), windowHeight(_height)
{
    // Create gui
    Gui::CreateHWindow(appName.c_str(), windowWidth, windowHeight);
    Gui::CreateDevice();
    Gui::CreateImGui();

    Themes::DefaultDark();


}

App::~App()
{
    // Destroy gui
    Gui::DestroyImGui();
    Gui::DestroyDevice();
    Gui::DestroyHWindow();
}

void App::Update()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void App::BeginRender()
{
    Gui::BeginRender();
    Gui::BeginImGuiRender();
}

void App::EndRender()
{
    Gui::EndImGuiRender();
    Gui::EndRender();
}


