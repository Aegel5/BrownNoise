#include <iostream>
#include <map>
#include <random>
#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>
#include <numbers>
#include <filesystem>
#include <fstream>
#include <format>
#include <future>
#include <charconv> // std::from_chars для float

namespace fs = std::filesystem;

#include <simple_enum/simple_enum.hpp>

#include "json.hpp"
using json = nlohmann::json;

#include "GetDir.h"
#include "Settings.h"
#include "Rand.h"
#include "Player.h"

#include "imgui.h"
#include "backends_layer.h"

Player player;

static void Draw() {
	if (ImGui::BeginCombo("Mode", SoundTypeName(Settings::data.last_mode))) {
		for (auto type : { SND_Voss, SND_BrownStandart, SND_PinkStandart, SND_BrownRibbon }) {
			if (ImGui::Selectable(SoundTypeName(type), Settings::data.last_mode == type)) {
				if (Settings::data.last_mode != type) {
					Settings::data.last_mode = type;
					Settings::Save();
					player.SetMode(type);
				}
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::SliderFloat("Volume", &Settings::data.volume, 0, 2)) {
		Settings::Save();
		player.set_volume(Settings::data.volume);
	}
	if (ImGui::Button("Restore")) {
		Settings::data.volume = Settings::init_vol;
		Settings::Save();
		player.set_volume(Settings::data.volume);
	}
	if (player.IsUnderflow()) {
		ImGui::Text("deflow");
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    setlocale(LC_ALL, ".utf8");

    Settings::Load();
	player.set_volume(Settings::data.volume);
	player.SetMode(Settings::data.last_mode);
	player.start();

	if (!ImBackends::Init(L"Window", 600, 300))	return 1;
	//if (!ImBackends::InitDisableMainViewport())	return 1;

	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigInputTextCursorBlink = false;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(ImBackends::main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
		style.FontScaleDpi = ImBackends::main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
		io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
		io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
	}

	// Setup Platform/Renderer backends
	ImBackends::InitRenders();


	ImBackends::CreateTimer([&]() {
		player.step();
		}, 200);

	while (ImBackends::WaitNewFrame()) {

		ImBackends::NewFrame();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings;
		//ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
		static bool open = true;
		if (ImGui::Begin("Hello, world!", &open, window_flags)) {
			Draw();
			ImGui::End();
		}
		if (!open) break;

		ImBackends::RenderVSync();

	}

	ImBackends::Cleanup();


    return 0;
}

