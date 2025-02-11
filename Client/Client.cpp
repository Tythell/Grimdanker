#include "../Grimdanker/src/Danker.h"

void OnStart(InputData_t& inputMap)
{

}

void OnUpdate(InputData_t& inputMap)
{
	for (auto& line : inputMap)
		for (auto& cha : line)
		{
			cha = cha == '3' ? 'l' : cha;
		}
}

void imguiWindowCave()
{
	ImGui::Text("hej hej");
}

int main()
{
	return danker::run("input.txt", OnStart, OnUpdate, imguiWindowCave);
}