#include <vector>
#include <iostream>
#include "DxWinHelper.h"
#include "Utility/Popup.h"
//#include "Utility/CommonDialogs.h"
#include <sstream> 
#include <algorithm>
#include<ctime>
#include <Windows.h>
#include <d3d11.h>

std::string GetClipboardText()
{
	// Try opening the clipboard
	if (!OpenClipboard(nullptr))
	{
		Popup::Error("fuck you 1");
		return "nuu";
	}

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr)
	{
		Popup::Error("fuck you 2");
		return "nuu";
	}

	// Lock the handle to get the actual text pointer
	char* pszText = static_cast<char*>(GlobalLock(hData));
	if (pszText == nullptr)
	{
		Popup::Error("fuck you 3");
		return "nuu";
	}

	// Save text in a string class instance
	std::string text(pszText);

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	return text;
}

void toClipboard(const std::string& s) {
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size()+1);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size()+1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

using namespace std;
using uint = unsigned int;

struct Weapon
{
	string name;
	string range;
	string attacks;
	string ap;
	vector<string> effects;

	string GetAsString()
	{
		string output = "";
		output += name + ": ";
		if (range.size() > 0) output += range + " ";
		output += attacks + " ";
		if (ap.size() > 0) output += ap + " ";
		for (int i = 0; i < effects.size(); i++)
		{
			output += effects[i] + " ";
		}
		return output;
	}
};

// Fear less, Fearless, Hero, Tough(12), 1x Brood Leader(Pheromones), 1x Hive Protector(Psy-Barrier), 1x Wings(Ambush, Flying)

std::vector<std::string> SeparatePassives(std::string fullstring)
{
	std::vector<std::string> v;
	stringstream ss(fullstring);
	string input = "";
	while (ss >> input)
	{
		string word = input;
		bool foundBracket = input.find(')') != -1;
		while (word.find(',') == -1 && !foundBracket)
		{
			if (!(ss >> input))
				break;
			foundBracket = input.find(')') != -1;
			word += " " + input;
		}
		v.emplace_back(word);
	}
	return v;
}

class Unit
{
public:
	void figureTitle(string line1, bool removeUpgradeName)
	{
		int firstSeparator = line1.find_first_of('|');
		m_title = line1.substr(0, firstSeparator);


		string stripped(line1);
		for (int i = 0; i < 2; i++)
		{
			firstSeparator = stripped.find_first_of('|') + 1;
			stripped = stripped.substr(firstSeparator);
		}
		m_passives = SeparatePassives(stripped);
		for (int i = 0; i < m_passives.size(); i++)
		{
			if (m_passives[i][0] == ' ')
			{
				m_passives[i] = m_passives[i].substr(1);
			}
		}
		//m_passives = Dialogs::CharsToStringsVector(stripped.c_str(), stripped.size(), ',');

		for (int i = 0; i < m_passives.size(); i++)
		{
			if (m_passives[i].find("Tough(") != -1)
			{
				m_title += "T" + m_passives[i].substr(6);
				break;
			}
		}
		for (int i = 0; i < m_passives.size(); i++)
		{
			if (m_passives[i][m_passives[i].size()-1] == ',')
			{
				m_passives[i] = m_passives[i].substr(0, m_passives[i].size() - 1);
				printf("");
			}
			if (removeUpgradeName && IsUpgrade(m_passives[i]))
			{
				if (m_upgradeIndex == -1) m_upgradeIndex = i;

				string word = m_passives[i];
				int firstBracket = word.find_first_of('(');
				word = word.substr(firstBracket + 1);

				int size = word.size();

				word = word.substr(0, word.size() - 1);
				m_passives[i] = word;
				printf("");
			}
		}

		printf("");
	}
	bool IsUpgrade(string passiveName) const
	{
		int bracketIndex = passiveName.find_first_of('(');
		if (bracketIndex == -1) return false;
		else if (!StringHelper::IsNumber(passiveName[bracketIndex+1]))
		{
			cout << passiveName << " is upgrade.\n";
			return true;
		}
		return false;
	}
	void figureWeapons(string line2)
	{
		stringstream ss(line2);
		bool nEndOfLine = true;
		string newInput;

		while (nEndOfLine)
		{
			Weapon weapon;
			
			weapon.name = FindFullName(ss, newInput);

			if (newInput.find('\"') != -1)
			{
				weapon.range = newInput.substr(1);
				ss >> newInput;
			}
			if (newInput.find('(') != -1)
			{
				newInput = newInput.substr(1);
			}
			if (newInput.find(','))
			{
				newInput = newInput.substr(0, newInput.size() - 1);
			}
			if (newInput.find(')') != -1) newInput = newInput.substr(0, newInput.size() - 1);
			weapon.attacks = newInput;
			ss >> newInput;
			if (newInput.find("AP(") != -1)
			{
				weapon.ap = "AP" + StringHelper::extractNumberStr(newInput);
				ss >> newInput;
			}
			// Fusion Rifle (12", A1, AP(4), Deadly(3), Psyched(35))
			// effects
			
			string convert = "k";
			//convert = newInput[find - 1];
			while (true)
			{
				weapon.effects.emplace_back(newInput + " ");

				int find = newInput.find("),");
				bool yesComma = find != -1;
				bool foundNumber = false;
				if (yesComma)
				{
					foundNumber = StringHelper::IsNumber(newInput[find - 1]);
				}

				if (yesComma && !foundNumber)
				{
					// clear final ) char from word
					int numEffects = weapon.effects.size();
					int numChars = weapon.effects[numEffects - 1].size();

					weapon.effects[numEffects - 1] = weapon.effects[numEffects - 1].substr(0, numChars - 3);

					break;
				}

				if (!(ss >> newInput))
				{
					printf("");
					nEndOfLine = false;
					break;
				}
				find = newInput.find("),");
				if (find != -1)
				{
					convert = newInput[find - 1];
				}
			}
			m_weapons.emplace_back(weapon);

		}
		printf("");
	}
	string m_title;
	vector<string> m_passives;
	vector<Weapon> m_weapons;

	int GetUpgradeIndex() const { return m_upgradeIndex; }
private:
	int m_upgradeIndex = -1;
	string FindFullName(stringstream& ss, string& lastinput)
	{
		string newInput;
		string fullname;
		while (true)
		{
			ss >> newInput;
			if (newInput.find("(") != -1) // if end of name break
			{
				lastinput = newInput;
				return fullname;
			}

			fullname += newInput + " ";
		}
	}
};

vector<Unit> GetUnit(string text, bool clearUpgradeName)
{
	stringstream ss1(text);

	string line1;
	string line2;
	//std::getline(ss1, linetest);

	int n = line1.size();

	vector<Unit> units;

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



#ifdef _DEBUG
int main()
#else
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#endif // _DEBUG
{
	wstring subtitles[] = {
		L"Now with extra grimdank!",
		L"Det var en fågel",
		L"Roger hette han",
		L"Han bodde på landet",
		L"She took the kids",
		L"Garrosh did everything wrong",
		L"Dabbin dab dab",
		L"Probably (not) bug-free!",
		L"My favourute color ia blue... no YELLOWWWWW!",
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

	if (true)
	{
		HWND hwnd;
		WNDCLASSEX wc;

		d3dStuff d3d;

		srand(time(0));

		wstring title = L"Grimdanker - " + subtitles[rand() % ARRAYSIZE(subtitles)];

		SetupShit(hwnd, wc, d3d, title);
		ImGuiIO& io = ImGui::GetIO(); (void)io;


		bool show_demo_window = true;
		bool show_another_window = false;
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

			{
				ImGui::SetNextWindowSize(ImVec2((float)currentWidth, (float)currentheight));
				ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Once);

				ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove /*| ImGuiWindowFlags_NoBackground*/;


				ImGui::Begin("mainwidow", 0, flags);

				static const UINT buffSize = 3000;
				static char buffer[buffSize]{ "" };
				ImGui::InputTextMultiline("##Input", buffer, buffSize);
				static bool clearUpgradeNames = true;
				ImGui::Checkbox("Clear Upgrade Names", &clearUpgradeNames);

				static char colorCStr[9];
				static ImVec4 upgradeColor{ 0.f,1.f,1.f,1.f };
				GetColor(upgradeColor, colorCStr, 9);

				static vector<Unit> units;
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
						std::string btnStr = "Title to clipboard##" + to_string(i);
						if (ImGui::Button(btnStr.c_str()))
						{
							outputStr = units[i].m_title;

							toClipboard(outputStr);
						}
						btnStr = "Desc to clipboard##" + to_string(i);


						if (ImGui::Button(btnStr.c_str()))
						{
							for (int j = 0; j < units[i].m_passives.size(); j++)
							{
								if (units[i].GetUpgradeIndex() == j)
								{
									outputStr += "[" + string(colorCStr) + "]";
								}

								outputStr += units[i].m_passives[j];
								if (j != units[i].m_passives.size()-1)
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
				//ImGui::ShowDemoWindow();
			}

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
