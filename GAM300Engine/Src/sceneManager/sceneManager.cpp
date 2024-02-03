/*!*************************************************************************
****
\file sceneManager.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the functions in the SceneManager class
****************************************************************************
***/

#include <fstream>

#include "sceneManager/sceneManager.h"
#include "propertyManager/registration.h"
#include "eventManager/eventManager.h"

namespace TDS
{
	// Unique pointer to instance of Scene
	std::unique_ptr<SceneManager> SceneManager::m_instance;
	bool SceneManager::isPlaying;

	/*!*************************************************************************
	Returns an instance of the SceneManager
	****************************************************************************/
	std::unique_ptr<SceneManager>& SceneManager::GetInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = std::make_unique<SceneManager>();

			std::filesystem::path currentPath = std::filesystem::current_path();
			while (!std::filesystem::exists(currentPath.string() + "\\assets"))
			{
				if (currentPath == "C:\\")
				{
					std::cout << "No asset directory found" << std::endl;
					break;
				}
				currentPath = currentPath.parent_path();
			}
			m_instance->parentFilePath = currentPath.string() + "\\assets\\";
			m_instance->filePath = currentPath.string() + "\\assets\\scenes\\";
			m_instance->scriptFilePath = currentPath.string() + "\\ManagedScripts\\";
		}
		return m_instance;
	}

	/*!*************************************************************************
	Initialize all registered components
	****************************************************************************/
	void SceneManager::Init()
	{
		ecs.registerComponent<NameTag>("Name Tag");
		ecs.registerComponent<Transform>("Transform");
		ecs.registerComponent<AI>("AI");
		ecs.registerComponent<BoxCollider>("Box Collider");
		ecs.registerComponent<CameraComponent>("Camera Component");
		ecs.registerComponent<CapsuleCollider>("Capsule Collider");
		ecs.registerComponent<GraphicsComponent>("Graphics Component");
		ecs.registerComponent<RigidBody>("Rigid Body");
		ecs.registerComponent<SphereCollider>("Sphere Collider");
		ecs.registerComponent<Sprite>("Sprite");
		ecs.registerComponent<Tag>("Tag");
		ecs.registerComponent<WinData>("Win Data");
		ecs.registerComponent<UISprite>("UI Sprite");
		ecs.registerComponent<DirectionalLightComponent>("DirectionalLight");
		ecs.registerComponent<SpotLightComponent>("SpotLight");
		ecs.registerComponent<PointLightComponent>("PointLight");


		startScene = "";
		//startScene = "MainMenu";

		//Entity entity1;
		//ecs.addComponent<NameTag>(entity1.getID());
		//ecs.getComponent<NameTag>(entity1.getID())->SetName("entity1");
		//ecs.addComponent<Transform>(entity1.getID());
		//ecs.addComponent<GraphicsComponent>(entity1.getID());
		//Entity entity2;
		//ecs.addComponent<NameTag>(entity2.getID());
		//ecs.getComponent<NameTag>(entity1.getID())->SetName("entity2");
		//ecs.addComponent<Transform>(entity2.getID());
		//ecs.addComponent<GraphicsComponent>(entity2.getID());
		//Entity entity3;
		//ecs.addComponent<NameTag>(entity3.getID());
		//ecs.getComponent<NameTag>(entity1.getID())->SetName("entity3");
		//ecs.addComponent<Transform>(entity3.getID());
		//ecs.addComponent<GraphicsComponent>(entity3.getID());
		//Entity entity4;
		//ecs.addComponent<NameTag>(entity4.getID());
		//ecs.getComponent<NameTag>(entity1.getID())->SetName("entity4");
		//ecs.addComponent<Transform>(entity4.getID());
		//ecs.addComponent<GraphicsComponent>(entity4.getID());

		//SerializeToFile(filePath + "MainMenu.json");

		//ecs.removeAllEntities();

		//ecs.removeEntity(entity4.getID());
		//SerializeToFile(filePath + "Game.json");

		bindSystemFunctions();
		// Setting default scene
		sceneDeserialize();
	}

	/*!*************************************************************************
	Deserializes ECS entities and data from JSON file to build ECS (File loading)
	****************************************************************************/
	bool SceneManager::Deserialize(const rapidjson::Value& obj, rapidjson::Document& doc)
	{
		// Archetype Sizes =======================
		obj.GetObject();
		for (rapidjson::Value::ConstMemberIterator itr = obj["Archetype Sizes"].MemberBegin(); itr != obj["Archetype Sizes"].MemberEnd(); ++itr)
		{
			std::string archetypeID = itr->name.GetString();
			auto archetypeSizes = itr->value.GetObject();

			while (archetypeID.length() < ecs.getNumberOfComponents())
			{
				archetypeID += "0";
			}

			ecs.addArchetype(archetypeID, false);

			for (rapidjson::Value::ConstMemberIterator componentItr = archetypeSizes.MemberBegin(); componentItr != archetypeSizes.MemberEnd(); ++componentItr)
			{
				int componentID = std::stoi(componentItr->name.GetString());
				auto componentSize = componentItr->value.GetInt();

				ecs.setComponentSize(archetypeID, componentID, componentSize);
			}

			ecs.commitArchetype(archetypeID);
		}

		// Engine component data =================
		EntityID currentEntity = 0;
		EntityID lastEntity = 0;
		int i = 0;
		for (rapidjson::Value::ConstMemberIterator itr = obj["Entity Data"].MemberBegin(); itr != obj["Entity Data"].MemberEnd(); ++itr, ++i)
		{
			if (itr == obj["Entity Data"].MemberEnd())
			{
				continue;
			}

			//EntityID newEntity = ecs.getNewID();
			currentEntity = static_cast<EntityID>(std::stoi(itr->name.GetString()));
			ecs.registerEntity(currentEntity);

			for (auto& m : itr->value.GetObject())	
			{
				std::string componentName = m.name.GetString();
				if (componentName == "ArchetypeID") // First "componentName" to immediately find the archetype of entity
				{
					std::string archetypeID = m.value.GetString();

					// Add all components at once
					while (archetypeID.length() < ecs.getNumberOfComponents())
					{
						archetypeID += "0";
					}

					ecs.addComponentsByArchetype(currentEntity, archetypeID);

					continue;
				}
				if (componentName == "Enabled")
				{
					ecs.setEntityIsEnabled(currentEntity, m.value.GetBool());
					ecs.setEntityPreviouslyEnabled(currentEntity);
					continue;
				}

				rapidjson::Value componentData;
				componentData.CopyFrom(m.value, doc.GetAllocator());

				rttr::type component = rttr::type::get_by_name(componentName);

				rttr::instance addedComponent = getComponentByName(component, currentEntity);
				fromJsonRecur(addedComponent, componentData);
			}
			updateName(currentEntity, ecs.getComponent<NameTag>(currentEntity)->GetName());

			if (currentEntity > lastEntity)
			{
				lastEntity = currentEntity;
			}
		}

		ecs.setIDCounter(lastEntity + 1);

		// Script Component data =================
		// EntityID, ScriptName, ScriptValues to set
		std::vector<std::tuple<EntityID, std::string, ScriptValues>> scriptReferences;

		for (rapidjson::Value::ConstMemberIterator itr = obj["Scripts"].MemberBegin(); itr != obj["Scripts"].MemberEnd(); ++itr, ++i)
		{
			EntityID currentEntity = static_cast<EntityID>(std::stoi(itr->name.GetString()));

			for (auto& script : itr->value.GetObject()) // For each script
			{
				std::string scriptName = script.name.GetString(); // Script Name

				addScript(currentEntity, scriptName);
				std::vector<ScriptValues> allVariableInfo;

				for (auto& variable : script.value.GetObject()) // For each variable
				{
					ScriptValues values;
					values.name = variable.name.GetString();
					auto variableTypeValue = variable.value.GetObject();

					values.type = variableTypeValue.MemberBegin()->name.GetString();

					if (values.type.find("System") != values.type.npos && values.type.find("[]") == values.type.npos)
						values.value = variableTypeValue.MemberBegin()->value.GetString();
					else if (values.type == "ScriptAPI.Vector2")
					{
						auto object = variableTypeValue.MemberBegin()->value.GetObj();
						values.vectorValueX = object["X"].GetFloat();
						values.vectorValueY = object["Y"].GetFloat();
					}
					else if (values.type == "ScriptAPI.Vector3")
					{
						auto object = variableTypeValue.MemberBegin()->value.GetObj();
						values.vectorValueX = object["X"].GetFloat();
						values.vectorValueY = object["Y"].GetFloat();
						values.vectorValueZ = object["Z"].GetFloat();
					}
					else if (values.type == "ScriptAPI.Vector4" || values.type == "ScriptAPI.Quaternion")
					{
						auto object = variableTypeValue.MemberBegin()->value.GetObj();
						values.vectorValueX = object["X"].GetFloat();
						values.vectorValueY = object["Y"].GetFloat();
						values.vectorValueZ = object["Z"].GetFloat();
						values.vectorValueW = object["W"].GetFloat();
					}
					else if (values.type.find("GameObject") != values.type.npos || values.type.find("Component") != values.type.npos) // Other entities & components
					{
						values.referenceEntityID = variableTypeValue.MemberBegin()->value.GetUint64();
					}
					else if (values.type.find("ScriptAPI") != values.type.npos) // Scripts
					{
						values.referenceEntityID = variableTypeValue.MemberBegin()->value.GetUint64();
						scriptReferences.emplace_back(std::tuple<EntityID, std::string, ScriptValues>(currentEntity, scriptName, values));

						continue; // Set value later
					}
					else
					{
						std::cout << "Deserialize: Invalid type" << std::endl;

						// Old code, in case there is old files using the old serialization method
						if (values.type == "Bool")
							values.value = variableTypeValue.MemberBegin()->value.GetBool() ? "true" : "false";
						else if (values.type == "Int")
							values.value = std::to_string(static_cast<int>(variableTypeValue.MemberBegin()->value.GetInt()));
						else if (values.type == "UInt")
							values.value = std::to_string(static_cast<uint32_t>(variableTypeValue.MemberBegin()->value.GetUint()));
						else if (values.type == "Double")
							values.value = std::to_string(static_cast<double>(variableTypeValue.MemberBegin()->value.GetDouble()));
						else if (values.type == "Float")
							values.value = std::to_string(static_cast<double>(variableTypeValue.MemberBegin()->value.GetDouble()));
						else if (values.type == "String")
							values.value = variableTypeValue.MemberBegin()->value.GetString();
						else if (values.type == "Vector3")
						{
							auto object = variableTypeValue.MemberBegin()->value.GetObj();
							values.vectorValueX = object["X"].GetFloat();
							values.vectorValueY = object["Y"].GetFloat();
							values.vectorValueZ = object["Z"].GetFloat();
						}
					}

					allVariableInfo.emplace_back(values);

				}

				setScriptValues(currentEntity, scriptName, allVariableInfo);
			}
		}
		for (auto reference : scriptReferences)
		{
			// add in a parameter for isEnabled
			//setScriptReference(reference.entityHoldingScript, reference.scriptName, reference.variableName, reference.entityScriptReference, reference.scriptReference);
			setScriptValue(std::get<EntityID>(reference), std::get<std::string>(reference), std::get<ScriptValues>(reference));
		}

		// Active Archetypes =====================
		if (obj.FindMember("Active Archetype") != obj.MemberEnd())
		{
			for (rapidjson::Value::ConstMemberIterator itr = obj["Active Archetype"].MemberBegin(); itr != obj["Active Archetype"].MemberEnd(); ++itr)
			{
				EntityID currentEntity = static_cast<EntityID>(std::stoi(itr->name.GetString()));
				auto activeArchetype = itr->value.GetString();

				ecs.setActiveArchetype(currentEntity, activeArchetype);
			}
		}

		return true;
	}

	/*!*************************************************************************
	Serializes ECS entities and data to JSON file to save ECS (File saving)
	****************************************************************************/
	bool SceneManager::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
	{
		writer->StartObject();


		// Archetype Sizes =======================
		// Serialize archetype sizes first
		writer->String("Archetype Sizes", static_cast<rapidjson::SizeType>(std::string("Archetype Sizes").length()), false);

		writer->StartObject();

		for (Archetype* a : ecs.getAllArchetypes())
		{
			if (!a->entityIds.size())
			{
				continue;	// there is no entities under this archetype
			}

			writer->String(a->type.c_str(), static_cast<rapidjson::SizeType>(a->type.length()), false);

			// Start of data sizes
			writer->StartObject();

			for (unsigned int i = 0; i < ecs.getNumberOfComponents(); ++i)
			{
				if (a->type[i] == '1')
				{
					writer->Key(std::to_string(i).c_str());
					writer->Int(a->componentDataSize[i]);
				}
			}

			writer->EndObject();
		}

		// End of archetype sizes
		writer->EndObject();

		// Engine component data =================
		// Serialize entity data
		writer->String("Entity Data", static_cast<rapidjson::SizeType>(std::string("Entity Data").length()), false);
		writer->StartObject();

		std::string entityNo = "EntityNo";
		//std::string component = "Component";

		std::vector<EntityID> entityList = ecs.getEntities();

		for (int i = 0; i < entityList.size(); ++i)
		{
			writer->String(std::to_string(entityList[i]).c_str(), static_cast<rapidjson::SizeType>(std::to_string(entityList[i]).length()), false);
			writer->StartObject();

			std::string archetype = ecs.getArchetypeID(entityList[i]);

			writer->Key("ArchetypeID");
			writer->String(archetype.c_str());

			writer->String("Enabled", static_cast<rapidjson::SizeType>(std::string("Enabled").length()), false);
			writer->Bool(ecs.getEntityIsEnabled(entityList[i]));

			for (std::string j : ecs.getEntityComponents(entityList[i]))
			{
				writer->String(j.c_str(), static_cast<rapidjson::SizeType>(j.length()), false);

				rttr::type type = rttr::type::get_by_name(j);
				ToJsonRecur(getComponentByName(type, entityList[i]), *writer);
			}

			writer->EndObject();
		}

		// End of entity data
		writer->EndObject();

		// Script Component data =================
		writer->String("Scripts", static_cast<rapidjson::SizeType>(std::string("Scripts").length()), false);
		writer->StartObject();

		for (int i = 0; i < entityList.size(); ++i)
		{
			// EntityID
			writer->String(std::to_string(entityList[i]).c_str(), static_cast<rapidjson::SizeType>(std::to_string(entityList[i]).length()), false);
			writer->StartObject();

			for (std::string scriptName : getAllScripts())
			{
				if (hasScript(entityList[i], scriptName))
				{
					writer->String(scriptName.c_str(), static_cast<rapidjson::SizeType>(scriptName.length()), false);
					writer->StartObject();

					std::vector<ScriptValues> allValues = getScriptVariables(entityList[i], scriptName);
					for (ScriptValues& scriptValues : allValues)
					{
						writer->String(scriptValues.name.c_str(), static_cast<rapidjson::SizeType>(scriptValues.name.length()), false);
						writer->StartObject();

						writer->String(scriptValues.type.c_str(), static_cast<rapidjson::SizeType>(scriptValues.type.length()), false);

						// Updated
						if (scriptValues.type.find("System") != scriptValues.type.npos)
						{
							writer->String(scriptValues.value.c_str(), static_cast<rapidjson::SizeType>(scriptValues.value.length()), false);
						}
						else if (scriptValues.type == "ScriptAPI.Vector2")
						{
							writer->StartObject();
							{
								writer->String("X", 1, false);
								writer->Double(scriptValues.vectorValueX);
								writer->String("Y", 1, false);
								writer->Double(scriptValues.vectorValueY);
							}
							writer->EndObject();
						}
						else if (scriptValues.type == "ScriptAPI.Vector3")
						{
							writer->StartObject();
							{
								writer->String("X", 1, false);
								writer->Double(scriptValues.vectorValueX);
								writer->String("Y", 1, false);
								writer->Double(scriptValues.vectorValueY);
								writer->String("Z", 1, false);
								writer->Double(scriptValues.vectorValueZ);
							}
							writer->EndObject();
						}
						else if (scriptValues.type == "ScriptAPI.Vector4" || scriptValues.type == "ScriptAPI.Quaternion")
						{
							writer->StartObject();
							{
								writer->String("X", 1, false);
								writer->Double(scriptValues.vectorValueX);
								writer->String("Y", 1, false);
								writer->Double(scriptValues.vectorValueY);
								writer->String("Z", 1, false);
								writer->Double(scriptValues.vectorValueZ);
								writer->String("W", 1, false);
								writer->Double(scriptValues.vectorValueW);
							}
							writer->EndObject();
						}
						else if (scriptValues.type.find("GameObject") != scriptValues.type.npos	// Game objects 
							|| scriptValues.type.find("Component") != scriptValues.type.npos	// Components 
							|| scriptValues.type.find("ScriptAPI") != scriptValues.type.npos)	// Scripts 
						{
							writer->Uint64(scriptValues.referenceEntityID);
						}
						else
						{
							std::cout << "Serialize: Invalid type" << std::endl;
						}

						//if (scriptValues.type == "System.Boolean")
						//{
						//	writer->String("Bool", static_cast<rapidjson::SizeType>(std::string("Bool").length()), false);
						//	scriptValues.value == "False" ? writer->Bool(false) : writer->Bool(true);
						//}
						//else if (scriptValues.type == "System.UInt16"
						//	|| scriptValues.type == "System.UInt32"
						//	|| scriptValues.type == "System.UInt64")
						//{
						//	writer->String("UInt", static_cast<rapidjson::SizeType>(std::string("UInt").length()), false);
						//	scriptValues.value == "" ? writer->Uint(0) : writer->Uint(std::stoi(scriptValues.value));
						//}
						//else if (scriptValues.type == "System.Int16"
						//	|| scriptValues.type == "System.Int32"
						//	|| scriptValues.type == "System.Int64"
						//	|| scriptValues.type == "System.Byte"
						//	|| scriptValues.type == "System.SByte")
						//{
						//	writer->String("Int", static_cast<rapidjson::SizeType>(std::string("Int").length()), false);
						//	scriptValues.value == "" ? writer->Int(0) : writer->Int(std::stoi(scriptValues.value));
						//}
						//else if (scriptValues.type == "System.Double")
						//{
						//	writer->String("Double", static_cast<rapidjson::SizeType>(std::string("Double").length()), false);
						//	scriptValues.value == "" ? writer->Double(0) : writer->Double(std::stod(scriptValues.value));
						//}
						//else if (scriptValues.type == "System.Single")
						//{
						//	writer->String("Float", static_cast<rapidjson::SizeType>(std::string("Float").length()), false);
						//	scriptValues.value == "" ? writer->Double(0) : writer->Double(std::stod(scriptValues.value));
						//}
						//else if (scriptValues.type == "System.String")
						//{
						//	writer->String("String", static_cast<rapidjson::SizeType>(std::string("String").length()), false);
						//	writer->String(scriptValues.value.c_str(), static_cast<rapidjson::SizeType>(scriptValues.value.length()), false);
						//}
						//else if (scriptValues.type == "ScriptAPI.GameObject")
						//{
						//	writer->String("GameObject", static_cast<rapidjson::SizeType>(std::string("GameObject").length()), false);
						//	writer->Int(scriptValues.referenceEntityID);
						//}
						//else if (scriptValues.type == "Component")
						//{
						//	writer->String("Component", static_cast<rapidjson::SizeType>(std::string("Component").length()), false);
						//	writer->Int(scriptValues.referenceEntityID);
						//}
						//else if (scriptValues.type == "System.Char")
						//{
						//	writer->String("Char", static_cast<rapidjson::SizeType>(std::string("Char").length()), false);
						//	writer->String(scriptValues.value.c_str(), static_cast<rapidjson::SizeType>(scriptValues.value.length()), false);
						//}
						//else if (scriptValues.type == "ScriptAPI.Vector3")
						//{
						//	writer->String("Vector3", static_cast<rapidjson::SizeType>(std::string("Vector3").length()), false);
						//	writer->StartObject();
						//	{
						//		writer->String("X", 1, false);
						//		writer->Double(scriptValues.vectorValueX);
						//		writer->String("Y", 1, false);
						//		writer->Double(scriptValues.vectorValueY);
						//		writer->String("Z", 1, false);
						//		writer->Double(scriptValues.vectorValueZ);
						//	}
						//	writer->EndObject();
						//}
						//else // scripts 
						//{
						//	// To Do 
						//	writer->String(scriptValues.type.c_str(), static_cast<rapidjson::SizeType>(scriptValues.type.length()), false);
						//	writer->Int(scriptValues.referenceEntityID);
						//}
						writer->EndObject();
					}

					writer->EndObject();
				}
			}

			writer->EndObject();
		}

		// End of scripts
		writer->EndObject();

		// Active Archetypes =====================
		writer->String("Active Archetype", static_cast<rapidjson::SizeType>(std::string("Active Archetype").length()), false);
		writer->StartObject();

		for (int i = 0; i < entityList.size(); ++i)
		{
			writer->String(std::to_string(entityList[i]).c_str(), static_cast<rapidjson::SizeType>(std::to_string(entityList[i]).length()), false);
			writer->String(ecs.getActiveArchetype(entityList[i]).c_str(), static_cast<rapidjson::SizeType>(ecs.getActiveArchetype(entityList[i]).length()), false);
		}

		// End of active archetypes
		writer->EndObject();

		// End of file
		writer->EndObject();

		return true;
	}

	/*!*************************************************************************
	This function serializes scenes into JSON files
	****************************************************************************/
	bool SceneManager::sceneSerialize()
	{
		std::ofstream ofs(parentFilePath + "scene.json");

		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

		writer.StartObject();
		writer.String("start", 5, false);
		writer.String(startScene.c_str(), static_cast<rapidjson::SizeType>(startScene.length()), false);

		writer.EndObject();

		ofs << sb.GetString();
		ofs.flush();
		ofs.close();

		return true;
	}

	/*!*************************************************************************
	This function deserializes scenes from JSON files
	****************************************************************************/
	bool SceneManager::sceneDeserialize()
	{
		std::string allScenesFilepath = parentFilePath + "scene.json";
		std::ifstream ifs(allScenesFilepath, std::ios::in);
		std::stringstream buffer;
		buffer << ifs.rdbuf();
		ifs.close();

		rapidjson::Document doc;
		doc.Parse(buffer.str().c_str());

		if (!InitDocument(buffer.str().c_str(), doc))
		{
			return false;
		}

		rapidjson::Value& value = doc.GetObject();
		
		// Starting scene
		rapidjson::Value::MemberIterator theItr = value.FindMember("start");
		std::string startingScene = theItr->value.GetString();

		bool fileCheck = false;
		for (auto& directory_entry : std::filesystem::directory_iterator(filePath))
		{
			if (directory_entry.path().extension() == ".json" && startingScene == directory_entry.path().stem().string()) // found
			{
				loadScene(startingScene);
				fileCheck = true;
			}
		}
		if (!fileCheck) // entry scene not found
		{
			newScene("NewScene");
			startingScene = "NewScene";
			loadScene("NewScene");
		}

		currentScene = startingScene;
		startScene = startingScene;
		currentSceneSaved = true;

		return true;
	}

	bool SceneManager::stringCompare(std::string a, std::string b)
	{
		for (int index = 0; index < a.size() && index < b.size(); ++index)
		{
			char lowerA = std::tolower(a[index]);
			char lowerB = std::tolower(b[index]);

			if (lowerA < lowerB)
			{
				return true;
			}
			else if (lowerA > lowerB)
			{
				return false;
			}
		}
		if (a.size() > b.size())
		{
			return false;
		}
		return true;
	}

	/*!*************************************************************************
	This function adds a new scene into the Scene Manager
	****************************************************************************/
	void SceneManager::newScene(std::string scene)
	{
		//allScenes.emplace_back(scene);

		if (!std::filesystem::exists(filePath + scene + ".json"))
		{
			std::ofstream ofs(filePath + scene + ".json");
			ofs.close();
		}

		//std::sort(allScenes.begin(), allScenes.end(), stringCompare);
	}
	/*!*************************************************************************
	This function loads given scene
	****************************************************************************/
	void SceneManager::loadScene(std::string scene)
	{
		ecs.removeAllEntities();
		eventManager.clearQueues();
		DeserializeFromFile(filePath + scene + ".json");
		currentScene = scene;
		currentSceneSaved = true;

		if (isPlaying)
		{
			awake();
		}
	}

	/*!*************************************************************************
	This function saves the given scene 
	****************************************************************************/
	void SceneManager::saveCurrentScene()
	{
		setCurrentSceneSaved(true);

		SerializeToFile(filePath + currentScene + ".json");

		//if (std::find(allScenes.begin(), allScenes.end(), currentScene) == allScenes.end())
		//{
		//	newScene(currentScene);
		//}
	}
	/*!*************************************************************************
	This function deletes the given scene
	****************************************************************************/
	void SceneManager::deleteScene(std::string scene)
	{
		if (scene == currentScene)
		{
			setCurrentSceneSaved(false);
		}

		std::filesystem::remove(filePath + scene + ".json");
		//auto sceneInVector = std::find(allScenes.begin(), allScenes.end(), scene);

		//if (sceneInVector != allScenes.end())
		//{
		//	allScenes.erase(sceneInVector);
		//}
	}

	/*!*************************************************************************
	This function is the getter function for current scene
	****************************************************************************/
	std::string SceneManager::getCurrentScene()
	{
		return currentScene;
	}

	/*!*************************************************************************
	This function is the setter function for current scene
	****************************************************************************/
	void SceneManager::setCurrentScene(std::string _currentScene)
	{
		currentScene = _currentScene;
	}
	/*!*************************************************************************
	This function is the getter function for current scene saved
	****************************************************************************/
	bool SceneManager::getCurrentSceneSaved()
	{
		return currentSceneSaved;
	}
	/*!*************************************************************************
	This function is the setter function for current scene saved
	****************************************************************************/
	void SceneManager::setCurrentSceneSaved(bool _currentSceneSaved)
	{
		if (!_currentSceneSaved && currentSceneSaved)
		{
			currentScene += "*";
		}
		else if (_currentSceneSaved && !currentSceneSaved)
		{
			currentScene.erase(currentScene.end() - 1);
		}

		currentSceneSaved = _currentSceneSaved;
	}

	/*!*************************************************************************
	This function renames a file
	****************************************************************************/
	bool SceneManager::renameScene(std::string oldName, std::string newName)
	{
		//if (std::find(allScenes.begin(), allScenes.end(), newName) != allScenes.end())
		//{
		//	return false;
		//}
		if (std::filesystem::exists(filePath + newName + ".json"))
		{
			return false;
		}

		std::filesystem::rename(filePath + oldName + ".json", filePath + newName + ".json");
		//std::replace(allScenes.begin(), allScenes.end(), oldName, newName);
		//std::sort(allScenes.begin(), allScenes.end(), stringCompare);
		return true;
	}

	/*!*************************************************************************
	This function is the getter function for all scenes in Scene Browser
	****************************************************************************/
	//std::vector<std::string>& SceneManager::getScenes()
	//{
	//	return allScenes;
	//}

	/*!*************************************************************************
	This function is the getter function for the path to assets
	****************************************************************************/
	std::string SceneManager::getAssetPath()
	{
		return parentFilePath;
	}

	/*!*************************************************************************
	This function is the getter function for the path to scenes
	****************************************************************************/
	std::string SceneManager::getScenePath()
	{
		return filePath;
	}

	/*!*************************************************************************
	This function is the setter function to reset starting scene
	****************************************************************************/
	DLL_API std::string SceneManager::getStartingScene()
	{
		return startScene;
	}

	/*!*************************************************************************
	This function is the setter function to reset starting scene
	****************************************************************************/
	void SceneManager::setStartingScene(std::string newStartScene)
	{
		startScene = newStartScene;
		sceneSerialize();
	}
}