/*!*************************************************************************
****
\file ImguiAudio.cpp
\author w.chongsheng
\par DP email: w.chongsheng@digipen.edu
\par Course: CSD3400
\par Section: a
\brief  Imgui UI in charge of audio manipulations
****************************************************************************
***/

#include "imguiHelper/ImguiAudio.h"
#include <string.h>

namespace TDS
{
	AudioImgui::AudioImgui()
	{
		//flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
		panelTitle = "Audio Files";
		windowPadding = ImVec2(0.f, 0.f);

		appear = playing = false;
		bar = 0.f;

		music.clear();
		SFX.clear();
		background.clear();
		VO.clear();

		std::cout << "AudImg Constructor" << '\n';

		audeng = AudioWerks::AudioEngine::get_audioengine_instance();
		add_audio_files("../assets/audioFiles/");
	}

	AudioImgui::~AudioImgui()
	{
		//delete audeng;
	}

	bool AudioImgui::ToggleControls(bool state)
	{
		appear = state;

		if (appear)
		{
			flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
		}
		else
		{
			flags = ImGuiWindowFlags_None;

			if (playing)
			{
				

				playing = false;
			}
		}

		return appear;
	}

	void AudioImgui::add_audio_files(std::filesystem::path folder_path)
	{
		
	}

	std::vector<std::filesystem::path> AudioImgui::go_deeper(std::filesystem::path f_path)
	{
		std::vector<std::filesystem::path> folders;
		std::vector<std::filesystem::path> files;

		for (auto& temp : std::filesystem::directory_iterator(f_path))
		{
			if (temp.is_directory())
			{
				folders = go_deeper(temp);

				for (auto& temp2 : folders)
				{
					files.push_back(temp2);
				}
			}
			else
			{
				files.push_back(temp);
			}
		}

		return files;
	}

	void AudioImgui::play(std::string file)
	{
		/*SoundInfo incase{};
		SoundInfo* this_one{ &incase };

		for (SoundInfo& temp : music)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}
		for (SoundInfo& temp : background)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}
		for (SoundInfo& temp : SFX)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}
		for (SoundInfo& temp : VO)
		{
			if (temp.getFilePath().find(file) != std::string::npos && temp.getFilePath().find(".meta") == std::string::npos)
			{
				this_one = &temp;
				goto FOUND;
			}
		}

	FOUND:

		audeng->loadSound(*this_one);
		audeng->playSound(*this_one);

		playing = true;*/
	}

	void AudioImgui::update()
	{
		//int AmountOfSound = (int)audeng->GetAmountOfChannelsPlaying();
		Vec3 playerPos, playerVel, playerForward, playerUp, ghostPos, ghostVel, ghostForward, ghostUp;
		audeng->get3DListenerCharacteristics(playerPos, playerVel, playerForward, playerUp);

		ImGui::Text("INFORMATION");
		// Begin the table with two columns
		ImGui::BeginTable("Audio table", 7);

		// Add headers for the two columns
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("3D");
		ImGui::TableSetupColumn("Loop");
		ImGui::TableSetupColumn("Position X");
		ImGui::TableSetupColumn("Position Y");
		ImGui::TableSetupColumn("Position Z");
		ImGui::TableSetupColumn("State");

		ImGui::Text("Name");
		ImGui::TableNextColumn();
		ImGui::Text("3D");
		ImGui::TableNextColumn();
		ImGui::Text("Loop");
		ImGui::TableNextColumn();
		ImGui::Text("Position X");
		ImGui::TableNextColumn();
		ImGui::Text("Position Y");
		ImGui::TableNextColumn();
		ImGui::Text("Position Z");
		ImGui::TableNextColumn();
		ImGui::Text("State");
		ImGui::TableNextRow();

		ImGui::Text("Player: ");
		ImGui::TableNextColumn();
		ImGui::Text("Yes");
		ImGui::TableNextColumn();
		ImGui::Text("No");
		ImGui::TableNextColumn();
		ImGui::Text("%f", playerPos.x);
		ImGui::TableNextColumn();
		ImGui::Text("%f", playerPos.y);
		ImGui::TableNextColumn();
		ImGui::Text("%f", playerPos.z);
		ImGui::TableNextColumn();
		ImGui::Text("Active");
		ImGui::TableNextRow();

		audeng->get3DListenerCharacteristics(ghostPos, ghostVel, ghostForward, ghostUp);
		ImGui::Text("Ghost: ");
		ImGui::TableNextColumn();
		ImGui::Text("Yes");
		ImGui::TableNextColumn();
		ImGui::Text("No");
		ImGui::TableNextColumn();
		ImGui::Text("%f", ghostPos.x);
		ImGui::TableNextColumn();
		ImGui::Text("%f", ghostPos.y);
		ImGui::TableNextColumn();
		ImGui::Text("%f", ghostPos.z);
		ImGui::TableNextColumn();
		ImGui::Text("Active");
		ImGui::TableNextRow();

		// Add data to the table
		/*for (int row = 0; row < AmountOfSound; ++row)
		{
			
			
			ImGui::TableNextRow();

			for (int col = 0; col < 7; ++col)
			{
				ImGui::TableNextColumn();
				ImGui::Text("%c", col);
			}
		}*/

		// End the table
		ImGui::EndTable();
	}
}