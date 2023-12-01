#include "MainApp.h"
#include "../Gui.h"

#include "../Font.h"
#include "../GuiHelper.h"
#include "../globals.h"
#include "../auth.hpp"
#include "../App.h"
#include "../skStr.h"
std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);

using namespace KeyAuth;

auto name = skCrypt("Test Loader");
auto ownerid = skCrypt("EdmsTKiuld");
auto secret = skCrypt("8416cd0426cc8888a49ac0d3ca99095747a9ccd345ecc99146472425be5b6a5f");
auto version = skCrypt("1.0");
auto url = skCrypt("https://keyauth.win/api/1.2/");
auto username = skCrypt("Username");
auto password = skCrypt("Password");
auto login = skCrypt("Login");
auto licensekey = skCrypt("License Key");
auto fid = skCrypt("747632");

std::string Cs2Status = "";

api KeyAuthApp(name.decrypt(), ownerid.decrypt(), secret.decrypt(), version.decrypt(), url.decrypt());

MainApp::MainApp() : App("Test Loader"), loggedIn(false), registering(false), subStatus(false), newSub(false)
{
	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		Sleep(1500);
		exit(1);
	}
	Gui::InitializeBubbles();
}

void MainApp::DisplayMessage(const std::string& message) {
	messageDisplayTime = std::chrono::steady_clock::now();
	displayedMessage = message;
}

void MainApp::Render()
{
	const char* loaderName = name.decrypt();

	Gui::RenderBubbles();

	// Initialize global items like title, icon, theme, etc
	Themes::Modern();
	ImGui::SetCursorPosX((Gui::CURRENTWIDTH - ImGui::CalcTextSize(loaderName).x));
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(Gui::LoaderIcon), ImVec2(56.f, 56.f))) {
		App::EndRender();
		ImGui::DestroyContext();
		PostQuitMessage(1);
		exit(1);
		Gui::DestroyDevice();
		Gui::DestroyHWindow();
		Gui::DestroyImGui();
	}
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::PushFont(Gui::titleFont);
	ImGui::SetCursorPosX((Gui::CURRENTWIDTH - ImGui::CalcTextSize(loaderName).x) * 0.5f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.f);
	ImGui::Text(loaderName);
	ImGui::PopFont();
	Gui::DrawRainbowBar(Gui::CURRENTWIDTH, 5.f);

	if (!newSub) {
		if (!registering) {
			if (!loggedIn) {
				RenderLoginState();
			}
			else 
			{
				RenderLoggedInState();
			}
		}
		else {
			RenderRegistrationState();
		}
	}
	else
	{
		RenderLicenseKeyState();
	}

	// Display any messages received from key auth within past 5 seconds
	if (!displayedMessage.empty() &&
		std::chrono::steady_clock::now() - messageDisplayTime <= messageDisplayDuration) {
		ImGui::Text(displayedMessage.c_str());
	}
}

void MainApp::RenderLoginState() {
	if (Gui::bubbleCount != 15) {
		Gui::bubbleCount == 15;
		// Gui::InitializeBubbles();
	}

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 loginSize = ImVec2(Gui::WIDTH, Gui::HEIGHT);

	if (windowSize.x != loginSize.x && windowSize.y != loginSize.y) {
		Gui::CURRENTWIDTH = Gui::WIDTH;
		Gui::CURRENTHEIGHT = Gui::HEIGHT;
	}

	ImGui::SetCursorPosX((Gui::WIDTH - ImGui::CalcTextSize("username").x) * 0.1f);
	ImGui::InputText(username.decrypt(), globals.user_name, IM_ARRAYSIZE(globals.user_name));
	ImGui::SetCursorPosX((Gui::WIDTH - ImGui::CalcTextSize("password").x) * 0.1f);
	ImGui::InputText(password.decrypt(), globals.pass_word, IM_ARRAYSIZE(globals.pass_word), ImGuiInputTextFlags_Password);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.29f, 0.4275f, 0.4863f, 1.f));
	ImGui::SetCursorPosX((Gui::WIDTH - ImGui::CalcTextSize("Login").x) * 0.15f);
	if (ImGui::Button(login.decrypt()))
	{
		KeyAuthApp.login(globals.user_name, globals.pass_word);
		if (!KeyAuthApp.data.success)
		{
			if (KeyAuthApp.data.message == "No active subscription(s) found") {
				loggedIn = true;
				subStatus = false;
				KeyAuthApp.fetchstats();
				KeyAuthApp.fetchonline();
				Cs2Status = KeyAuthApp.var("CS2-Status");
			}
			else
			{
				DisplayMessage(KeyAuthApp.data.message);
			}
		}
		else
		{
			subStatus = true;
			loggedIn = true;
			KeyAuthApp.fetchstats();
			KeyAuthApp.fetchonline();
			Cs2Status = KeyAuthApp.var("CS2-Status");
		}
	}
	ImGui::PopStyleColor(1);

	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.384f, 0.5725f, 0.6196f, 1.f));
	//ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Create New Account").x) * 0.15f);
	if (ImGui::Button("Create New Account"))
	{
		registering = true;
	}
	ImGui::PopStyleColor(1);
}

void MainApp::RenderLoggedInState() {

	if (Gui::bubbleCount != 50) {
		Gui::bubbleCount == 50;
		// Gui::InitializeBubbles();
	}

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 loginSize = ImVec2(Gui::MAXWIDTH, Gui::MAXHEIGHT);

	if (windowSize.x != loginSize.x && windowSize.y != loginSize.y) {
		Gui::CURRENTWIDTH = Gui::MAXWIDTH;
		Gui::CURRENTHEIGHT = Gui::MAXHEIGHT;
	}

	ImGui::Text("Logged In as: %s", globals.user_name);
	ImGui::SameLine();
	ImGui::Text("Subscription Status: ");

	if (subStatus)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Valid");


	}
	else {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid");
		if (ImGui::Button("Enter New license key")) {
			newSub = true;
		}
	}

	ImGui::Text("HWID: %s", KeyAuthApp.data.hwid.c_str());
	ImGui::SameLine();
	ImGui::Text("Online Users: %s", KeyAuthApp.data.numOnlineUsers.c_str());

	ImGui::SetCursorPosX((Gui::MAXWIDTH - ImGui::CalcTextSize("CS2 External").x) * 0.4f);
	ImGui::PushFont(Gui::titleFont);
	ImGui::Text("CS2 External");
	ImGui::PopFont();

	ImGui::SetCursorPosX((Gui::MAXWIDTH - ImGui::CalcTextSize("CS2 External").x) * 0.25f);

	if (ImGui::ImageButton(reinterpret_cast<ImTextureID*>(Gui::CS2Image), ImVec2(312.f, 156.f))) {
		// std::vector<std::uint8_t> bytes = KeyAuthApp.download(fid.decrypt());
		// ExecuteInMemory(bytes);
	}

	ImGui::SetCursorPosX((Gui::MAXWIDTH - ImGui::CalcTextSize("Detection Status:").x) * 0.25f);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Detection Status:");

	ImGui::SetCursorPosX((Gui::MAXWIDTH - ImGui::CalcTextSize("ndetected").x) * 0.25f);
	if (Cs2Status == "Detected") {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), " %s", Cs2Status.c_str());
	}
	else if (Cs2Status == "Undetected") {
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), " %s", Cs2Status.c_str());
	}
	else {
		ImGui::SameLine();
		ImGui::Text(" %s", Cs2Status.c_str());
	}

	ImGui::SetCursorPosX(ImGui::GetWindowWidth());
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.886f, 0.4275f, 0.353f, 1.f));
	if (ImGui::Button("Exit")) {
		Gui::DestroyImGui();
		Gui::DestroyDevice();
		Gui::DestroyHWindow();

		PostQuitMessage(0);
	}
	ImGui::PopStyleColor(1);
}

void MainApp::RenderLicenseKeyState() {
	ImGui::InputText(licensekey.decrypt(), globals.license_key, sizeof(globals.license_key), ImGuiInputTextFlags_Password);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.357f, 0.459f, 0.3255f, 1.f));
	if (ImGui::Button("Use New License Key")) {
		KeyAuthApp.upgrade(globals.user_name, globals.license_key);
		if (KeyAuthApp.data.message == "Upgraded successfully")
		{
			subStatus = true;
			newSub = false;
		}
		else
		{
			DisplayMessage(KeyAuthApp.data.message);
		}
	}
	ImGui::PopStyleColor(1);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.886f, 0.4275f, 0.353f, 1.f));
	if (ImGui::Button("Back To Menu"))
	{
		newSub = false;
	}
	ImGui::PopStyleColor(1);
}

void MainApp::RenderRegistrationState() {
	ImGui::InputText(username.decrypt(), globals.user_name, IM_ARRAYSIZE(globals.user_name));
	ImGui::InputText(password.decrypt(), globals.pass_word, IM_ARRAYSIZE(globals.pass_word), ImGuiInputTextFlags_Password);
	ImGui::InputText(licensekey.decrypt(), globals.license_key, sizeof(globals.license_key), ImGuiInputTextFlags_Password);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.357f, 0.459f, 0.3255f, 1.f));
	if (ImGui::Button("Create Account")) {
		KeyAuthApp.regstr(globals.user_name, globals.pass_word, globals.license_key);
		if (!KeyAuthApp.data.success)
		{
			DisplayMessage(KeyAuthApp.data.message);
		}
		else
		{
			subStatus = true;
			loggedIn = true;
			registering = false;
			KeyAuthApp.fetchstats();
			KeyAuthApp.fetchonline();
			Cs2Status = KeyAuthApp.var("CS2-Status");
		}
	}
	ImGui::PopStyleColor(1);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.886f, 0.4275f, 0.353f, 1.f));
	if (ImGui::Button("Back To Login"))
	{
		registering = false;
	}
	ImGui::PopStyleColor(1);
}

bool MainApp::ExecuteInMemory(std::vector<std::uint8_t>& exeBytes) {
	// Create a buffer to store the executable bytes
	LPVOID buffer = VirtualAlloc(NULL, exeBytes.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (buffer == NULL) {
		std::cerr << "Failed to allocate memory." << std::endl;
		return false;  // Return false instead of 1
	}

	// Copy the executable bytes into the allocated buffer
	memcpy(buffer, exeBytes.data(), exeBytes.size());

	// Execute the code in the allocated buffer
	try {
		// Use a function pointer with the correct signature
		using FunctionType = void (*)();
		FunctionType function = reinterpret_cast<FunctionType>(buffer);
		function();
	}
	catch (...) {
		std::cerr << "Exception occurred during execution." << std::endl;
	}

	// Free the allocated memory
	VirtualFree(buffer, 0, MEM_RELEASE);
	return true;
}

std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}