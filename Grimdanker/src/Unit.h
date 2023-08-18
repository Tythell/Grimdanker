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

	string GetAsString();
};

class Unit
{
public:
	void figureTitle(string line1, bool removeUpgradeName);
	bool IsUpgrade(string passiveName) const;
	void figureWeapons(string line2);
	string m_title;
	std::vector<string> m_passives;
	std::vector<Weapon> m_weapons;

	int GetUpgradeIndex() const { return m_upgradeIndex; }
private:
	int m_upgradeIndex = -1;
	string FindFullName(stringstream& ss, string& lastinput);
};