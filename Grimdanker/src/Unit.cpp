#include "Unit.h"
#include "Utility/StringHelper.h"
#include <algorithm>

void Unit::figureTitle(string line1, bool removeUpgradeName)
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

			string toughWord = "T" + StringHelper::extractNumberStr(m_passives[i].substr(6));
			/*if (toughWord[toughWord.size() - 1] == ')')
				toughWord = toughWord.substr(0, toughWord.size() - 1);*/
			m_title += toughWord;
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
		if ((removeUpgradeName && IsUpgrade(m_passives[i])))
		{
			if (m_upgradeIndex == -1) m_upgradeIndex = i;

			string word = m_passives[i];
			int firstBracket = word.find_first_of('(');
			word = word.substr(firstBracket + 1);

			int size = word.size() - 1;

			if (word[size] == ')')
			{
				word = word.substr(0, size);
			}

			m_passives[i] = word;
		}
	}

	std::replace(m_title.begin(), m_title.end(), '[', '|');
	std::replace(m_title.begin(), m_title.end(), ']', '|');

	printf("");
}

bool Unit::IsUpgrade(string passiveName) const
{
	int fbracketIndex = passiveName.find_first_of('(');
	int bbracketIndex = passiveName.find_first_of(')');

	bool therIsbracket = false;

	//StringHelper::IsNumber(passiveName[bracketIndex + 1])
	//
	if ((fbracketIndex != -1 && !StringHelper::IsNumber(passiveName[fbracketIndex + 1])) ||
		(bbracketIndex != -1 && !StringHelper::IsNumber(passiveName[bbracketIndex - 1])))
	{
		return true;
	}

	return false;
}

//#define READINPUT if (!(ss >> newInput)) nEndOfLine = false  

#define READSTREAM if (ss >> newInput) {} \
else { \
 m_weapons.emplace_back(weapon); \
 continue; \
} \

void Unit::figureWeapons(string line2)
{
	stringstream ss(line2);
	bool nEndOfLine = true;
	string newInput;

	while (true)
	{
		Weapon weapon;

		bool endIt = false;
		weapon.name = FindFullName(ss, newInput, endIt);
		if (endIt)
		{
			break;
		}

		bool endOfWeapon = false;

		if (newInput.find('\"') != -1)
		{
			weapon.range = newInput.substr(1);
			READSTREAM;
		}
		// number of attacks
		// CCW (A2), Gun (12", A5)
		{
			string numAttacks = newInput;
			int firstBrack = numAttacks.find('(');
			int lastBrack = numAttacks.find(')');

			int hasBracket = int(firstBrack != -1);

			numAttacks = numAttacks.substr(hasBracket, (unsigned int)lastBrack - hasBracket);
			weapon.attacks = numAttacks;
			endOfWeapon = lastBrack != -1;

			if (newInput.find(')') != -1)
			{
				m_weapons.emplace_back(weapon);
				continue;
			}

			READSTREAM;

		}

		if (/*!endOfWeapon && */newInput.find("AP(") != -1)
		{
			weapon.ap = "AP" + StringHelper::extractNumberStr(newInput);
			char num = newInput[newInput.size() - 3];

			if (newInput.find(')') != -1 && !StringHelper::IsNumber(num))
			{
				m_weapons.emplace_back(weapon);
				continue;
			}
			READSTREAM;
		}
		// Fusion Rifle (A1), Dumb rock(12", A5, rending, yeeh)
		// effects
		while (true)
		{
			weapon.effects.emplace_back(newInput);
			
			char num = newInput[newInput.size() - 3];

			if (newInput.find(')') != -1 && !StringHelper::IsNumber(num))
			{
				weapon.effects[weapon.effects.size() - 1] = newInput.substr(0, newInput.size() - 1);
				break;
			}
			if (!(ss >> newInput)) break;
			//READSTREAM;
		}
			

		m_weapons.emplace_back(weapon);
		/*while (true)
		{
			if (newInput.find(',') != -1)
			{
				weapon.effects.emplace_back(newInput + " ");
			}


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
		m_weapons.emplace_back(weapon);*/

	}
	printf("");
}

string Unit::FindFullName(stringstream& ss, string& lastinput, bool& endOfLine)
{
	string newInput;
	string fullname;
	while (true)
	{
		if (!(ss >> newInput))
		{
			endOfLine = true;
			return fullname;
		}
		if (newInput.find("(") != -1) // if end of name break
		{
			lastinput = newInput;
			return fullname;
		}

		fullname += newInput + " ";
	}
}

string Weapon::GetAsString()
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
