#include "Globals.h"
#include "Console.h"

void Console::Log(const std::string& msg, LogType type)
{
	entries.push_back({ msg, type });
}

void Console::Draw(bool* open)
{
	if (!ImGui::Begin("Console", open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Clear"))
	{
		entries.clear();
	}

	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &autoScroll);
	ImGui::Separator();
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	for (const auto& entry : entries)
	{
		ImVec4 color;
		switch (entry.type)
		{
		case LogType::INFO:
			color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case LogType::WARNING:
			color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case LogType::Error:
			color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		default:
			color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextUnformatted(entry.message.c_str());
		ImGui::PopStyleColor();
	}
	if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();
	ImGui::End();
}
