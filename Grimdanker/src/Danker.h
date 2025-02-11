#pragma once

#include <vector>
#include <iostream>
#include "DxWinHelper.h"
#include "Utility/Clipboard.h"
#include <sstream> 
#include <algorithm>
#include<ctime>
#include <Windows.h>
#include <d3d11.h>
//#include "Unit.h"
#include <fstream>
#include <map>
#include <functional>

using uint = unsigned int;
using InputData_t = std::vector<std::string>;

namespace danker
{
	void GetColorHexStr(ImVec4& clr, char* cstr, int charArrSize)
	{
		ImGui::SameLine();
		ImGuiColorEditFlags clrEditFlags = ImGuiColorEditFlags_NoInputs;

		ImGui::ColorEdit4("Upgrade color", &clr.x, clrEditFlags);

		uint colorByte = ImGui::ColorConvertFloat4ToU32(clr);

		sprintf_s(cstr, charArrSize, "%08x", colorByte);

		// flip bytes
		std::swap(cstr[0], cstr[6]);
		std::swap(cstr[1], cstr[7]);
		std::swap(cstr[2], cstr[4]);
		std::swap(cstr[3], cstr[5]);
	}

	class AdventCodeData
	{
	public:
		AdventCodeData(std::string path, std::function<void(InputData_t&)> onStart, std::function<void(InputData_t&)> onUpdate, bool rawData = false):
			m_onUpdate(onUpdate)
		{
			if (rawData)
			{
				std::stringstream ss(path);
				for (std::string line; std::getline(ss, line);)
				{
					m_data.emplace_back(line);
				}
			}
			else
			{
				ReloadFile(path);
			}
			onStart(m_data);
			ReadColorConfig();
		}
		void Update()
		{
			m_onUpdate(m_data);
		}
		void ReloadFile(std::string path = "input.txt")
		{
			std::ifstream reader(path, std::ios::in);
			THROW_POPUP_ERROR(reader.is_open(), "File: " + path + " not found.");
			//if (!reader.is_open()) return;
			m_data.resize(0);
			for (std::string line; std::getline(reader, line);)
			{
				m_data.emplace_back(line);
			}
			reader.close();
		}
		InputData_t GetInputData() const
		{
			return m_data;
		}
		uint32_t GetColorByChar(const char& c)
		{
			if (!m_colorConfig.count(c))
				return 0xff000000;
			return m_colorConfig[c];
		}
	private:
		std::function<void(InputData_t&)> m_onUpdate;
		uint32_t ConvertToColor(uint32_t input)
		{
			uint8_t bytes[4]{ 0,0,0,0 };
			memcpy(bytes, &input, sizeof(uint32_t));

			std::swap(bytes[0], bytes[2]);
			bytes[3] = (bytes[3] == 0) ? 255 : bytes[3];
			memcpy(&input, bytes, sizeof(uint32_t));

			return input;
		}
		void ReadColorConfig(std::string path = "colorconfig.txt")
		{
			std::ifstream reader(path, std::ios::in);
			std::string errMsg = "File " + path + " was not found";
			POPUP_MESSAGE(reader.is_open(), errMsg);


			char c = 0;
			uint32_t color = 0;

			while (reader >> c)
			{
				char dummy = 0;
				reader >> dummy >> std::hex >> color;

				m_colorConfig[c] = ConvertToColor(color);
			}
			reader.close();
		}
		std::map<char, uint32_t> m_colorConfig;
		InputData_t m_data;
	};

	void DisplayRawInput(AdventCodeData& buffer)
	{
		if (ImGui::BeginTabItem("RawInput"))
		{
			for (uint i = 0; i < buffer.GetInputData().size(); i++)
			{
				std::string text = buffer.GetInputData()[i];
				ImGui::Text(text.c_str());
			}
			ImGui::EndTabItem();
		}
	}

	void Visualizer(AdventCodeData& buffer)
	{
		uint buttnIndex = 0;
		const uint amountOfLines = buffer.GetInputData().size();
		for (uint y = 0; y < amountOfLines; y++)
		{
			const std::string line = buffer.GetInputData()[y];
			const uint amountOfLetters = line.size();
			for (uint x = 0; x < amountOfLetters; x++)
			{
				std::string label = "";
				label.append(1, line[x]);
				label += "##" + std::to_string(buttnIndex);
				ImGui::PushID(buttnIndex);
				//ImGui::PushStyleColor(ImGuiCol_Button, 0xff000000);
				ImGui::PushStyleColor(ImGuiCol_Button, buffer.GetColorByChar(line[x]));
				//ImGui::PushStyleColor(ImGuiCol_Button, buffer.(colorIdx));
				ImGui::Button(label.c_str(), ImVec2(30, 30));
				ImGui::PopStyleColor(1);
				ImGui::PopID();
				if (x != amountOfLetters - 1)
				{
					ImGui::SameLine();
				}
				buttnIndex++;
			}
		}
		ImGui::EndTabItem();
	}

	void mainWindowd(uint currentWidth, uint currentHeight, AdventCodeData& buffer, std::function<void()> imguiCave)
	{
		ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
		ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Once);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove /*| ImGuiWindowFlags_NoBackground*/;
		ImGui::Begin("mainwidow", 0, flags);

		if (ImGui::BeginTabBar("maintab"))
		{

			if (ImGui::BeginTabItem("other tab"))
				Visualizer(buffer);
			DisplayRawInput(buffer);

			ImGui::EndTabBar();

			if (ImGui::ArrowButton("##left", ImGuiDir_Right))
				buffer.Update();

			imguiCave();

			ImGui::End();
		}
	}

#ifdef _DEBUG
	int run(std::string buffer, std::function<void(InputData_t&)> onStart, std::function<void(InputData_t&)> onUpdate, std::function<void()> imguiWindowCave)
#else
	int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#endif // _DEBUG
	{
		bool enableConsole = false;
		FILE* fp = NULL;

		AdventCodeData aoc(buffer, onStart, onUpdate);

		if (enableConsole)
		{
			AllocConsole();
			freopen_s(&fp, "conin$", "r", stdin);
			freopen_s(&fp, "conout$", "w", stdout);
			freopen_s(&fp, "conout$", "w", stderr);
			printf("Console enabled:\n");
		}


		std::wstring subtitles[] = {
			L"Now with extra grimdank!",
			L"Det var en fågel",
			L"Roger hette han",
			L"Han bodde på landet",
			L"She took the kids",
			L"Garrosh did everything wrong",
			L"Dabbin dab dab",
			L"Probably (not) bug-free!",
			L"My favourute color is blue... no YELLOWWWWW!",
			L"To seek the holy grail!",
			L"and he rolls TWO ONES FAIL FAIL FAIL",
			L"Remeber Knög Karola",
			L"I HAVE BEEN CHOSEN",
			L"Haha termagant injured a general in melee",
			L"Subscribe to the fucking channel!",
			L"Sub to Xisumavoid!",
			L"Sub to Alfabusa!",
			L"Is 3D-printing minis legal?, I hope so!",
			L"Alfabusa gang should make more narrative battle reports.",
			L"For great hair care!",
			L"Program made by Tythell!",
		};

		bool enableWindow = true;
		if (enableWindow)
		{
			// Window and DX11 code taken and modified from imgui example

			HWND hwnd;
			WNDCLASSEX wc;

			d3dStuff d3d;

			srand(time(0));

			std::wstring title = L"Grimdanker - " + subtitles[rand() % ARRAYSIZE(subtitles)];

			SetupShit(hwnd, wc, d3d, title);
			ImGuiIO& io = ImGui::GetIO(); (void)io;

			io.IniFilename = NULL;
			io.LogFilename = NULL;


			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			bool done = false;
			while (!done)
			{
				// Poll and handle messages (inputs, window resize, etc.)
				// See the WndProc() function below for our to dispatch events to the Win32 backend.

				MSG msg;
				while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
					if (msg.message == WM_QUIT)
						done = true;
				}
				if (done)
					break;

				// Handle window resize (we don't resize directly in the WM_SIZE handler)

				static UINT currentWidth = 0;
				static UINT currentheight = 0;

				if (Resolution::g_ResizeWidth != 0 && Resolution::g_ResizeHeight != 0)
				{
					CleanupRenderTarget(d3d);
					d3d.g_pSwapChain->ResizeBuffers(0, Resolution::g_ResizeWidth, Resolution::g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);

					currentWidth = Resolution::g_ResizeWidth;
					currentheight = Resolution::g_ResizeHeight;

					Resolution::g_ResizeWidth = Resolution::g_ResizeHeight = 0;

					CreateRenderTarget(d3d);
				}

				// Start the Dear ImGui frame
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				mainWindowd(currentWidth, currentheight, aoc, imguiWindowCave);

				// Rendering
				ImGui::Render();
				const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
				d3d.g_pd3dDeviceContext->OMSetRenderTargets(1, &d3d.g_mainRenderTargetView, nullptr);
				d3d.g_pd3dDeviceContext->ClearRenderTargetView(d3d.g_mainRenderTargetView, clear_color_with_alpha);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				// Update and Render additional Platform Windows
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					ImGui::UpdatePlatformWindows();
					ImGui::RenderPlatformWindowsDefault();
				}

				d3d.g_pSwapChain->Present(1, 0); // Present with vsync
				//g_pSwapChain->Present(0, 0); // Present without vsync
			}

			cleanup(hwnd, wc, d3d);
		}

		if (enableConsole && fp != NULL)
		{
			fclose(fp);
			FreeConsole();
		}

		//system("pause");
		return 0;
	}
}


