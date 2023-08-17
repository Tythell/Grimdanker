#pragma once
#include <string>
#include <vector>
#include <sstream>

using string = std::string;
using stringstream = std::stringstream;



inline std::vector<std::string> SeparatePassives(std::string fullstring)
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

struct Weapon
{
	string name;
	string range;
	string attacks;
	string ap;
	std::vector<string> effects;

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
			if (m_passives[i][m_passives[i].size() - 1] == ',')
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
		else if (!StringHelper::IsNumber(passiveName[bracketIndex + 1]))
		{
			//cout << passiveName << " is upgrade.\n";
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
	std::vector<string> m_passives;
	std::vector<Weapon> m_weapons;

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