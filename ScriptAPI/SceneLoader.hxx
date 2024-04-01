#pragma once
#include "sceneManager/sceneManager.h"

namespace ScriptAPI
{
	//Bobo class to load scene - shift it to scripts next time
	public ref class SceneLoader
	{
	public:
		static void LoadMainMenu();

		static void LoadMainGame();

		static void LoadStartingCutscene();

		static void LoadLoseScreen();

		static void LoadEndGameCredits();

		static void LoadQuitGame();

		static void LoadOptionsScreen();

		static void LoadTempEndScene();

		static void LoadTempJumpscare();

		static System::String^ dataPath;
	};
}