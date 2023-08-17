#include <vector>
#include <iostream>
#include "DxWinHelper.h"
#include "Utility/Clipboard.h"
#include <sstream> 
#include <algorithm>
#include<ctime>
#include <Windows.h>
#include <d3d11.h>
#include "Unit.h"

using uint = unsigned int;

std::vector<Unit> GetUnit(string text, bool clearUpgradeName)
{
	stringstream ss1(text);

	string line1;
	string line2;
	//std::getline(ss1, linetest);

	int n = line1.size();

	std::vector<Unit> units;

	while (std::getline(ss1, line1))
	{
		if (line1[0] == '+' || line1.size() == 0)
		{
			continue;
		}
		std::getline(ss1, line2);

		Unit newUnit;
		newUnit.figureTitle(line1, clearUpgradeName);
		newUnit.figureWeapons(line2);

		std::getline(ss1, line2);
		line1 = "";
		line2 = "";
		units.emplace_back(newUnit);
	}
	return units;
}

void GetColor(ImVec4& clr, char* cstr, int charArrSize)
{
	ImGui::SameLine();
	//static float upgradeClrF[4]{ 0.f, 1.f, 1.f, 1.f };
	ImGuiColorEditFlags clrEditFlags = ImGuiColorEditFlags_NoInputs;

	ImGui::ColorEdit4("Upgrade color", &clr.x, clrEditFlags);

	uint colorByte = ImGui::ColorConvertFloat4ToU32(clr);

	sprintf_s(cstr, charArrSize, "%08x", colorByte);

	char temp[2] = { cstr[0], cstr[1] };

	// flip bytes
	std::swap(cstr[0], cstr[6]);
	std::swap(cstr[1], cstr[7]);
	std::swap(cstr[2], cstr[4]);
	std::swap(cstr[3], cstr[5]);
}

void mainWindow(uint currentWidth, uint currentHeight)
{
	ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentHeight));
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Once);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove /*| ImGuiWindowFlags_NoBackground*/;
	ImGui::Begin("mainwidow", 0, flags);

	static const UINT buffSize = 3000;
	static char buffer[buffSize]{ "" };
	ImGui::InputTextMultiline("##Input", buffer, buffSize);
	static bool clearUpgradeNames = true;
	ImGui::Checkbox("Clear Upgrade Names", &clearUpgradeNames);

	static char colorCStr[9];
	static ImVec4 upgradeColor{ 0.f, 1.f, 1.f, 1.f };
	GetColor(upgradeColor, colorCStr, 9);

	static std::vector<Unit> units;
	if (ImGui::Button("Copy from input"))
	{
		if (buffer[0] == '+') units = GetUnit(buffer, clearUpgradeNames);
		else Popup::Info("lmao fail");
	}

	ImGui::SameLine();
	if (ImGui::Button("Copy from clipboard"))
	{
		string clipboard = GetClipboardText();
		if (clipboard[0] == '+')
		{
			clipboard.erase(std::remove(clipboard.begin(), clipboard.end(), '\r'), clipboard.end());
			units = GetUnit(clipboard, clearUpgradeNames);
		}
		else
			Popup::Info("Invalid text");
	}
	//uint unitSize = units.size();
	if (units.size() > 0)
	{
		ImGui::SameLine();
		if (ImGui::Button("Clear")) units.clear();
		for (int i = 0; i < units.size(); i++)
		{
			ImGui::Text(units[i].m_title.c_str());
			std::string displayStr = "";
			std::string displayUpgrStr = "";
			int nOfPassives = units[i].m_passives.size();
			for (int j = 0; j < nOfPassives; j++)
			{
				int upgradeIdx = units[i].GetUpgradeIndex();
				if (j < upgradeIdx || upgradeIdx == -1)
					displayStr += units[i].m_passives[j] + ", ";
				else
					displayUpgrStr += units[i].m_passives[j] + ", ";

			}
			ImGui::Text(displayStr.c_str());
			ImGui::SameLine();
			ImGui::TextColored(upgradeColor, displayUpgrStr.c_str());

			for (int j = 0; j < units[i].m_weapons.size(); j++)
			{
				displayStr = "";
				displayStr += units[i].m_weapons[j].name + ": ";
				displayStr += units[i].m_weapons[j].range;
				displayStr += " " + units[i].m_weapons[j].attacks + " ";
				displayStr += units[i].m_weapons[j].ap;
				for (int k = 0; k < units[i].m_weapons[j].effects.size(); k++)
				{
					displayStr += " " + units[i].m_weapons[j].effects[k];
				}
				ImGui::Text(displayStr.c_str());
			}
			string outputStr = "";
			std::string btnStr = "Title to clipboard##" + std::to_string(i);
			if (ImGui::Button(btnStr.c_str()))
			{
				outputStr = units[i].m_title;

				toClipboard(outputStr);
			}
			btnStr = "Desc to clipboard##" + std::to_string(i);


			if (ImGui::Button(btnStr.c_str()))
			{
				for (int j = 0; j < units[i].m_passives.size(); j++)
				{
					if (units[i].GetUpgradeIndex() == j)
					{
						outputStr += "[" + string(colorCStr) + "]";
					}

					outputStr += units[i].m_passives[j];
					if (j != units[i].m_passives.size() - 1)
					{
						outputStr += ", ";
					}

				}
				outputStr += "[-]\n\n";

				for (int j = 0; j < units[i].m_weapons.size(); j++)
				{
					outputStr += units[i].m_weapons[j].GetAsString() + "\n";
				}
				toClipboard(outputStr);
			}
		}
	}
	ImGui::End();
}

#ifdef _DEBUG
int main()
#else
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#endif // _DEBUG
{
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
		L"3D-printing minis is probably legal, it should be!",
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

			mainWindow(currentWidth, currentheight);

			//ImGui::ShowDemoWindow();

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



	//system("pause");
	return 0;
}
