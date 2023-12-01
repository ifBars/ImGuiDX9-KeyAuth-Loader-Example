#pragma once
#include "../App.h"
#include "../gui.h"

#include <chrono>

class MainApp : public App
{
public:
    MainApp();

    void RenderLoginState();
    void RenderLoggedInState();
    void RenderLicenseKeyState();
    void RenderRegistrationState();
    bool ExecuteInMemory(std::vector<std::uint8_t>& exeBytes);
    void DisplayMessage(const std::string& message);
    void Render() override;
    std::string text;
    std::string displayedMessage;
    const std::chrono::seconds messageDisplayDuration{3};
    std::chrono::steady_clock::time_point messageDisplayTime;
    bool loggedIn;
    bool registering;
    bool subStatus;
    bool newSub;
};
