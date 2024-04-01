#include "SceneLoader.hxx"
#include "TypeConversion.hxx"
#include "EngineInterface.hxx"
#include "MacroHelper.h"
namespace ScriptAPI
{
	void SceneLoader::LoadMainMenu()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("MainMenu");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();

	}
	void SceneLoader::LoadMainGame()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("M5_MansionSoap");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();
	}

	void SceneLoader::LoadStartingCutscene()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("StartingCutscene");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();
	}

	void SceneLoader::LoadLoseScreen()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("LoseScene");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();
	}

	void SceneLoader::LoadEndGameCredits()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("AfterGameCredits");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();
	}

	void SceneLoader::LoadQuitGame()
	{
		TDS::QuitEngine();
	}

	void SceneLoader::LoadOptionsScreen()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("MainMenuOptions");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();
	}

	void SceneLoader::LoadTempEndScene()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("TempEndScene");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();
	}

	void SceneLoader::LoadTempJumpscare()
	{
		EngineInterface::Reload();
		TDS::SceneManager::GetInstance()->loadScene("TempFinalJumpscare");
		TDS::SceneManager::GetInstance()->awake();
		TDS::SceneManager::GetInstance()->start();
	}

}