/*!*************************************************************************
****
\file ImguiProperties.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the functions in the Level Editor Properties
		panel
****************************************************************************
***/


#include "imguiHelper/ImGuiFunctionHelper.h"

#include "imguiHelper/ImguiProperties.h"
#include "imguiHelper/ImguiHierarchy.h"

#include "eventManager/eventHandler.h"
#include "imguiHelper/ImguiAssetBrowser.h"

#include "Input/Input.h"

#include "GraphicsResource/Revamped/MeshController.h"


namespace TDS
{
	/*!*************************************************************************
	This function initializes the Properties panel
	****************************************************************************/
	Properties::Properties()
	{
		flags = ImGuiWindowFlags_NoCollapse;
		panelTitle = "Properties";
		windowPadding = ImVec2(0.f, 0.f);

		addComponentWidth = 200.0f;
		addComponentHeight = 300.0f;
	}

	/*"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Left Arrow",
	"Up Arrow",
	"Right Arrow",
	"Down Arrow",
	"Numpad 0",		// 30
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",

	"Semicolon",	//40
	"Plus",
	"Comma",
	"Minus",
	"Period",
	"Slash",

	"Shift",		//46
	"Control",
	"Alt",

	"Left Shift",	//49
	"Right Shift",
	"Left Control",
	"Right Control",

	"Space",		//53
	"Escape",
	"Capslock",
	"Tab",
	"Enter",
	"Backspace",
	"Tilde",

	"F1",			//60
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",

	"Left Mouse",		//72
	"Right Mouse",
	"Middle Mouse",
	"Mouse 4",
	"Mouse 5",
	"Mouse 6",
	"Scroll"*/

	uint32_t toKeyCode(uint32_t index)
	{
		if (index >= 0 && index <= 25) // A to Z
		{
			return index + 0x41;
		}
		else if (index >= 26 && index <= 29) // Up Down Left Right
		{
			return index - 26 + 0x25;
		}
		else if (index >= 30 && index <= 39) // Numbers
		{
			return index - 30 + 0x30;
		}
		else if (index >= 40 && index <= 45) // Special Characters
		{
			return index - 40 + 0xBA;
		}
		else if (index >= 46 && index <= 48) // Shift, Ctrl & Alt
		{
			return index - 46 + 0x10;
		}
		else if (index >= 49 && index <= 52) // Left and Right Shift + Control
		{
			return index - 49 + 0xA0;
		}
		else if (index >= 60 && index <= 71) // Function Keys
		{
			return index - 60 + 0x70;
		}

		else if (index == 53) // Space
		{
			return 0x20;
		}
		else if (index == 54) // Esc
		{
			return 0x1B;
		}
		else if (index == 55) // Caps
		{
			return 0x14;
		}
		else if (index == 56) // Tab
		{
			return 0x09;
		}
		else if (index == 57) // Enter
		{
			return 0x0D;
		}
		else if (index == 58) // Backspace
		{
			return 0x08;
		}
		else if (index == 59) // Tilde
		{
			return 0xC0;
		}

		else if (index == 72) // Left Mouse
		{
			return 0x01;
		}
		else if (index == 73) // Right Mouse
		{
			return 0x02;
		}
		else if (index == 74) // Middle Mouse
		{
			return 0x04;
		}
		else if (index == 75) // M4
		{
			return 0x05;
		}
		else if (index == 76) // M5
		{
			return 0x06;
		}
		else if (index == 77) // Scroll
		{
			return 0x07;
		}

		return 0;
	}
	uint32_t toKeyCodeIndex(uint32_t keyCode)
	{
		if (keyCode >= 0x41 && keyCode <= 0x5A) // A to Z
		{
			return keyCode - 0x41;
		}
		else if (keyCode >= 0x25 && keyCode <= 0x28) // Up Down Left Right
		{
			return keyCode - 0x25 + 26;
		}
		else if (keyCode >= 0x30 && keyCode <= 0x39) // Numbers
		{
			return keyCode - 0x30 + 30;
		}
		else if (keyCode >= 0xBA && keyCode <= 0xBF) // Special Characters
		{
			return keyCode - 0xBA + 40;
		}
		else if (keyCode >= 0x10 && keyCode <= 0x10) // Shift, Ctrl & Alt
		{
			return keyCode - 0x10 + 46;
		}
		else if (keyCode >= 0xA0 && keyCode <= 0xA3) // Left and Right Shift + Control
		{
			return keyCode - 0xA0 + 49;
		}
		else if (keyCode >= 0x70 && keyCode <= 0x7B) // Function Keys
		{
			return keyCode - 0x70 + 60;
		}

		else if (keyCode == 0x20) // Space
		{
			return 53;
		}
		else if (keyCode == 0x1B) // Esc
		{
			return 54;
		}
		else if (keyCode == 0x14) // Caps
		{
			return 55;
		}
		else if (keyCode == 0x09) // Tab
		{
			return 56;
		}
		else if (keyCode == 0x0D) // Enter
		{
			return 57;
		}
		else if (keyCode == 0x08) // Backspace
		{
			return 58;
		}
		else if (keyCode == 0xC0) // Tilde
		{
			return 59;
		}

		else if (keyCode == 0x01) // Left Mouse
		{
			return 72;
		}
		else if (keyCode == 0x02) // Right Mouse
		{
			return 73;
		}
		else if (keyCode == 0x04) // Middle Mouse
		{
			return 74;
		}
		else if (keyCode == 0x05) // M4
		{
			return 75;
		}
		else if (keyCode == 0x06) // M5
		{
			return 76;
		}
		else if (keyCode == 0x07) // Scroll
		{
			return 77;
		}

		return 0;
	}

	/*!*************************************************************************
	This function is the update function for Properties panel
	****************************************************************************/
	void Properties::update()
	{
		std::shared_ptr<Hierarchy> hierarchyPanel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[PanelTypes::HIERARCHY]);
		auto& sceneManagerInstance = SceneManager::GetInstance();

		// If there is a selected entity
		if (EntityID selectedEntity = hierarchyPanel->getSelectedEntity())
		{
			std::vector<std::string> allComponentNames = ecs.getAllRegisteredComponents();

			ImGuiTreeNodeFlags nodeFlags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_SpanFullWidth |
				ImGuiTreeNodeFlags_Selected |
				ImGuiTreeNodeFlags_AllowOverlap;

			// Component Display
			int i = 0;
			for (auto componentName : allComponentNames)
			{
				IComponent* componentBase = getComponentByName(componentName, selectedEntity);

				if (!componentBase)
				{
					continue;
				}

				if (componentName == "Name Tag"/* && ImGui::BeginTable("###", 2)*/)
				{
					auto nameTagComponent = reinterpret_cast<NameTag*>(componentBase);

					bool entityEnabled = ecs.getEntityIsEnabled(selectedEntity);

					ImGui::Checkbox(("##" + componentName + "CheckBox").c_str(), &entityEnabled);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						ecs.setEntityIsEnabled(selectedEntity, entityEnabled);
					}

					ImGui::SameLine();
					ImGui::PushItemWidth(-FLT_MIN);

					std::string newValue = nameTagComponent->GetName();
					char temp[100];
					strcpy_s(temp, newValue.c_str());
					ImGui::InputText(("##" + newValue).c_str(), temp, 100);

					if (ImGui::IsItemDeactivatedAfterEdit() && std::string(temp) != "")
					{
						nameTagComponent->SetName(std::string(temp));
						sceneManagerInstance->updateName(selectedEntity, std::string(temp));
					}

					ImGui::PopItemWidth();

					//int parent = static_cast<int>(nameTagComponent->GetHierarchyParent());
					//ImguiInput("", parent);
					//int indexInParent = static_cast<int>(nameTagComponent->GetHierarchyIndex());
					//ImguiInput("", indexInParent);

					//ImGui::EndTable();

					continue;
				}

				bool componentOpen = ImGui::TreeNodeEx(("##" + componentName).c_str(), nodeFlags);
				bool componentRemoved = false;

				ImGui::PushID(i);
				if (ImGui::BeginPopupContextItem("componentEditPopup"))
				{
					if (componentName != "Name Tag" && componentName != "Transform" && ImGui::Selectable("Remove Component"))
					{
						removeComponentByName(componentName, selectedEntity);
						TDS_INFO("Removed Component");
						componentRemoved = true;
					}

					ImGui::EndPopup();
				}

				if (rightClick && ImGui::IsItemHovered())
				{
					ImGui::OpenPopup("componentEditPopup");
				}
				ImGui::PopID();

				if (componentName != "Transform")
				{
					bool componentEnabled = getComponentIsEnable(componentName, selectedEntity);

					ImGui::SameLine();
					ImGui::Checkbox(("##" + componentName + "CheckBox").c_str(), &componentEnabled);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						setComponentIsEnable(componentName, selectedEntity, componentEnabled);
					}
				}

				ImGui::SameLine();
				ImGui::Text(componentName.c_str());

				++i;

				if (componentOpen && !componentRemoved)
				{
					if (ImGui::BeginTable("###", 2, /*ImGuiTableFlags_Borders |*/ ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_SizingStretchSame, ImVec2(0.0f, 5.5f)))
					{
						ImGui::TableSetupColumn("Component variable name", ImGuiTableColumnFlags_None, ImGui::GetContentRegionAvail().x * 0.4f);
						ImGui::TableSetupColumn("Component variable value", ImGuiTableColumnFlags_None, ImGui::GetContentRegionAvail().x * 0.6f);

						ImGui::TableNextRow();

						ImGui::TableNextColumn();
						ImGui::TableNextColumn();
						ImGui::PushItemWidth(-FLT_MIN); // Right-aligned

						// For child transform change
						Vec3 oldPosition, oldScale, oldRotation;
						if (componentName == "Transform")
						{
							Transform* transformComponent = reinterpret_cast<Transform*>(componentBase);
							oldPosition = transformComponent->GetPosition();
							oldScale = transformComponent->GetScale();
							oldRotation = transformComponent->GetRotation();

							NameTag* nameTagComponent = GetNameTag(selectedEntity);
							if (nameTagComponent->GetHierarchyParent() > 0)
							{
								EntityID parent = nameTagComponent->GetHierarchyParent();
								Transform* parentTransformComponent = GetTransform(parent);

								Vec4 localPosition = transformComponent->getLocalPosition(parent);
								Vec3 localPositionVec3 = localPosition;
								ImguiInput("Position", localPositionVec3);
								transformComponent->setLocalPosition(parent, Vec4(localPositionVec3, localPosition.w));

								Vec3 parentScale = parentTransformComponent->GetScale();
								Vec3 reflectedScale = oldScale - parentScale;
								ImguiInput("Scale", reflectedScale);
								transformComponent->SetScale(reflectedScale + parentScale);

								Vec3 parentRotation = parentTransformComponent->GetRotation();
								Vec3 reflectedRotation = oldRotation - parentRotation;
								ImguiInput("Rotation", reflectedRotation);
								transformComponent->SetRotation(reflectedRotation + parentRotation);
							}
							else
							{
								ImguiComponentDisplay(componentName, componentBase);

								if (nameTagComponent->GetHierarchyChildren().size() > 0 &&
									(oldPosition != transformComponent->GetPosition() ||
										oldScale != transformComponent->GetScale() ||
										oldRotation != transformComponent->GetRotation()))
								{
									EventHandler::postChildTransformationEvent(selectedEntity, oldPosition, oldScale, oldRotation);
								}
							}
						}
						else
						{
							ImguiComponentDisplay(componentName, componentBase);
						}
						if (componentName == "Graphics Component")
						{
							if (ImGui::Button("Reset all Mesh position(MANSION MESH ONLY)"))
							{
								GraphicsComponent* graphComponent = reinterpret_cast<GraphicsComponent*>(componentBase);
								if (graphComponent->m_MeshControllerRef.m_ResourcePtr != nullptr)
								{
									graphComponent->m_MeshControllerRef.m_ResourcePtr->ResetMeshPosition();
								}


							}
						}
						ImGui::PopItemWidth();
						ImGui::EndTable();
					}

					ImGui::TreePop();
				}
				else if (componentRemoved)
				{
					ImGui::TreePop();
				}
			}

			// Script Display
			int buttonID = 10000;
			for (auto scriptName : sceneManagerInstance->getAllScripts())
			{
				if (!sceneManagerInstance->hasScript(selectedEntity, scriptName))
				{
					continue;
				}

				bool removedScript = false;
				if (ImGui::TreeNodeEx(("##" + scriptName).c_str(), nodeFlags))
				{
					ImGui::PushID(i);
					if (ImGui::BeginPopupContextItem("scriptEditPopup"))
					{
						if (ImGui::Selectable("Remove Component"))
						{
							sceneManagerInstance->removeScript(selectedEntity, scriptName);
							TDS_INFO("Removed Component");
							removedScript = true;
						}
						if (ImGui::Selectable("Edit Script"))
						{
							std::filesystem::path filePath = std::filesystem::path(sceneManagerInstance->getScenePath()).parent_path().parent_path().parent_path().string() + std::string("\\ManagedScripts\\");
							std::string directory_entry = filePath.string() + scriptName + ".cs";
							system((std::string("code ") + directory_entry).c_str());
						}

						ImGui::EndPopup();
					}

					if (rightClick && ImGui::IsItemHovered())
					{
						ImGui::OpenPopup("scriptEditPopup");
					}
					ImGui::PopID();

					if (removedScript)
					{
						ImGui::TreePop();
						continue;
					}

					bool componentEnabled = sceneManagerInstance->isScriptEnabled(selectedEntity, scriptName);

					ImGui::SameLine();
					ImGui::Checkbox(("##" + scriptName + "CheckBox").c_str(), &componentEnabled);

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						sceneManagerInstance->toggleScript(selectedEntity, scriptName.c_str());
						ImGui::TreePop();
						continue;
					}

					ImGui::SameLine();
					ImGui::Text(scriptName.c_str());

					//ImGui::PushID(selectedEntity);
					if (ImGui::BeginTable("components", 2, /*ImGuiTableFlags_Borders |*/ ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_SizingStretchSame, ImVec2(0.0f, 5.5f)))
					{
						ImGui::TableSetupColumn("Component variable name", ImGuiTableColumnFlags_None, ImGui::GetContentRegionAvail().x * 0.4f);
						ImGui::TableSetupColumn("Component variable value", ImGuiTableColumnFlags_None, ImGui::GetContentRegionAvail().x * 0.6f);

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::Text("Script");
						//ImGui::SetColumnWidth(ImGui::GetContentRegionAvail().x * 0.3f);
						ImGui::TableNextColumn();
						ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.15f);
						//ImGui::

						ImGui::PushID(0);
						ImGui::BeginDisabled();

						char temp[100];
						strcpy_s(temp, scriptName.c_str());
						ImGui::InputText("##scriptName", temp, 100, ImGuiInputTextFlags_ReadOnly);
						ImGui::SameLine();
						ImGui::Button("O");

						ImGui::EndDisabled();
						ImGui::PopID();

						ImGui::PopItemWidth();
						ImGui::PushItemWidth(-FLT_MIN); // Right-aligned

						std::vector<ScriptValues> allValues = getScriptVariables(selectedEntity, scriptName);

						for (ScriptValues scriptValue : allValues)
						{
							if (scriptValue.headerString != "") // NOTE: So cursed can u shorten please ry what is wrong with u HAHA
							{
								ImGui::PopItemWidth();
								ImGui::EndTable();

								ImGui::NewLine();
								ImGui::Text(scriptValue.headerString.c_str());

								ImGui::BeginTable("components", 2, /*ImGuiTableFlags_Borders |*/ ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_SizingStretchSame, ImVec2(0.0f, 5.5f));

								ImGui::TableSetupColumn("Component variable name", ImGuiTableColumnFlags_None, ImGui::GetContentRegionAvail().x * 0.4f);
								ImGui::TableSetupColumn("Component variable value", ImGuiTableColumnFlags_None, ImGui::GetContentRegionAvail().x * 0.6f);

								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								//ImGui::SetColumnWidth(ImGui::GetContentRegionAvail().x * 0.3f);
								ImGui::TableNextColumn();
								ImGui::PushItemWidth(-FLT_MIN); // Right-aligned

								continue;
							}

							if (scriptValue.type == "System.Boolean")
							{
								bool value = scriptValue.value == "False" ? false : true;
								if (ImguiInput(scriptValue.name, value))
								{
									scriptValue.value = value ? "True" : "False";
									sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
									//sceneManagerInstance->setBool(selectedEntity, scriptName, scriptValue.name, value);
								}
							}
							else if (scriptValue.type == "System.UInt16" || scriptValue.type == "System.UInt32" || scriptValue.type == "System.UInt64")
							{
								static std::vector<std::string> KeyCodeTypeString =
								{
									"A",
									"B",
									"C",
									"D",
									"E",
									"F",
									"G",
									"H",
									"I",
									"J",
									"K",
									"L",
									"M",
									"N",
									"O",
									"P",
									"Q",
									"R",
									"S",
									"T",
									"U",
									"V",
									"W",
									"X",
									"Y",
									"Z",
									"Left Arrow",
									"Up Arrow",
									"Right Arrow",
									"Down Arrow",
									"Numpad 0",		// 30
									"Numpad 1",
									"Numpad 2",
									"Numpad 3",
									"Numpad 4",
									"Numpad 5",
									"Numpad 6",
									"Numpad 7",
									"Numpad 8",
									"Numpad 9",
									"Semicolon",	//40
									"Plus",
									"Comma",
									"Minus",
									"Period",
									"Slash",

									"Shift",		//46
									"Control",
									"Alt",

									"Left Shift",	//49
									"Right Shift",
									"Left Control",
									"Right Control",

									"Space",		//53
									"Escape",
									"Capslock",
									"Tab",
									"Enter",
									"Backspace",
									"Tilde",

									"F1",			//60
									"F2",
									"F3",
									"F4",
									"F5",
									"F6",
									"F7",
									"F8",
									"F9",
									"F10",
									"F11",
									"F12",

									"Left Mouse",		//62
									"Right Mouse",
									"Middle Mouse",
									"Mouse 4",
									"Mouse 5",
									"Mouse 6",
									"Scroll"
								};

								int value = toKeyCodeIndex(std::stoi(scriptValue.value));

								if (ImguiInput(scriptValue.name, KeyCodeTypeString, value))
								{
									scriptValue.value = std::to_string(toKeyCode(value));
									sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
								}
							}
							else if (scriptValue.type == "System.Int16"
								|| scriptValue.type == "System.Int32"
								|| scriptValue.type == "System.Int64"
								|| scriptValue.type == "System.Byte"
								|| scriptValue.type == "System.SByte")
							{
								int value = std::stoi(scriptValue.value);
								if (ImguiInput(scriptValue.name, value))
								{
									scriptValue.value = std::to_string(value);
									sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
								}
							}
							else if (scriptValue.type == "System.Double")
							{
								float value = std::stod(scriptValue.value);
								if (ImguiInput(scriptValue.name, value))
								{
									scriptValue.value = std::to_string(value);
									sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
								}
							}
							else if (scriptValue.type == "System.Single")
							{
								float value = std::stod(scriptValue.value);
								if (ImguiInput(scriptValue.name, value))
								{
									scriptValue.value = std::to_string(value);
									sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
								}
							}
							else if (scriptValue.type == "System.String")
							{
								std::string value = scriptValue.value;
								if (ImguiInput(scriptValue.name, value))
								{
									scriptValue.value = value;
									sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
								}
							}
							else if (scriptValue.type == "System.Char")
							{
								//char value = scriptValue.value[0];
								//value = ImguiInput(scriptValue.name, value);
								//sceneManagerInstance->setChar(selectedEntity, scriptName, scriptValue.name, value);
							}
							else if (scriptValue.type == "ScriptAPI.Vector3")
							{
								Vec3 value(scriptValue.vectorValueX, scriptValue.vectorValueY, scriptValue.vectorValueZ);
								if (ImguiInput(scriptValue.name, value))
								{
									scriptValue.vectorValueX = value.x;
									scriptValue.vectorValueY = value.y;
									scriptValue.vectorValueZ = value.z;
									sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
								}
							}
							else /*if (scriptValue.type.find("ScriptAPI") != scriptValue.type.npos)*/ // scripts & game object
							{
								// To Do 
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								ImGui::Text(scriptValue.name.c_str());

								ImGui::TableNextColumn();

								// Game Object
								ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.15f);
								ImGui::BeginDisabled();

								char temp[100] = "None";

								ImGui::PushID(selectedEntity);
								if (scriptValue.referenceEntityID) // there is a entity reference 
								{
									if (scriptValue.type == "ScriptAPI.GameObject")
									{
										strcpy_s(temp, ecs.getComponent<NameTag>(scriptValue.referenceEntityID)->GetName().c_str());
									}
									else
									{
										strcpy_s(temp, (ecs.getComponent<NameTag>(scriptValue.referenceEntityID)->GetName() + " (" + scriptValue.type + ")").c_str());
									}
								}
								ImGui::InputText("###", temp, 100, ImGuiInputTextFlags_ReadOnly);

								if (ImGui::BeginDragDropTarget())
								{
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("draggedEntity"))
									{
										// Changing either component / script
										// Need to check if dragged entity has such component / script
										EntityID payloadEntityID = *(static_cast<EntityID*>(payload->Data));

										if (scriptValue.type.find("GameObject") != scriptValue.type.npos)
										{
											// Changing game object
											scriptValue.referenceEntityID = payloadEntityID;
											sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
										}
										else if (scriptValue.type.find("Component") != scriptValue.type.npos) // Component
										{
											if (/*payloadEntityID != selectedEntity &&*/ getComponentByName(scriptValue.type, payloadEntityID))
											{
												scriptValue.referenceEntityID = payloadEntityID;
												sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
											}
										}
										else // Script
										{
											if (/*payloadEntityID != selectedEntity &&*/ sceneManagerInstance->hasScript(payloadEntityID, scriptValue.type))
											{
												scriptValue.referenceEntityID = payloadEntityID;
												sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
											}
										}
									}
									ImGui::EndDragDropTarget();
								}

								ImGui::PopID();
								ImGui::EndDisabled();

								ImGui::PushID(buttonID++);
								ImGui::SameLine();
								if (ImGui::Button("O"))
								{
									popupPosition = ImGui::GetCursorPos();
									popupPosition.x += ImGui::GetWindowPos().x;
									popupPosition.y -= ImGui::GetScrollY();
									ImGui::SetNextWindowPos(popupPosition);
									ImGui::SetNextWindowSize(ImVec2(300.f, 175.f));

									ImVec4* colors = ImGui::GetStyle().Colors;
									colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.00f);

									ImGui::OpenPopup(("Select Game Object: " + scriptValue.name).c_str());
								}

								bool unused_open = true;
								if (ImGui::BeginPopupModal(("Select Game Object: " + scriptValue.name).c_str(), &unused_open, ImGuiWindowFlags_AlwaysVerticalScrollbar))
								{
									if (scriptValue.type == "ScriptAPI.GameObject")
									{
										ImGuiTextFilter gameObjectFilter;
										gameObjectFilter.Draw("##gameObjectFilter");
										ImGui::SameLine();
										if (ImGui::Button("Clear", ImVec2(-FLT_MIN, 0)))
										{
											gameObjectFilter.Clear();
										}

										if (gameObjectFilter.InputBuf[0] == 0 && ImGui::Selectable("None", false, ImGuiSelectableFlags_SpanAllColumns))
										{
											scriptValue.referenceEntityID = 0;
											sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
											ImGui::CloseCurrentPopup();
										}

										for (EntityID entityID : ecs.getEntities())
										{
											if (entityID == selectedEntity)
											{
												continue;
											}

											std::string entityName = ecs.getComponent<NameTag>(entityID)->GetName();

											if (!gameObjectFilter.PassFilter(entityName.c_str()))
											{
												continue;
											}

											bool clicked = ImGui::Selectable(entityName.c_str(), entityID == selectedEntity, ImGuiSelectableFlags_SpanAllColumns);

											if (clicked || (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
											{
												scriptValue.referenceEntityID = entityID;
												sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
												ImGui::CloseCurrentPopup();
											}
										}
									}
									// Components =====================================================================================
									else
										if (scriptValue.type.find("Component") != scriptValue.type.npos)
										{
											ImGuiTextFilter componentFilter;
											componentFilter.Draw("##gameObjectFilter");
											ImGui::SameLine();
											if (ImGui::Button("Clear", ImVec2(-FLT_MIN, 0)))
											{
												componentFilter.Clear();
											}

											if (componentFilter.InputBuf[0] == 0 && ImGui::Selectable("None", false, ImGuiSelectableFlags_SpanAllColumns))
											{
												scriptValue.referenceEntityID = 0;
												sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
												ImGui::CloseCurrentPopup();
											}

											for (EntityID entityID : ecs.getEntities())
											{
												if (entityID == selectedEntity || getComponentByName(scriptValue.type, entityID) == nullptr)
												{
													continue;
												}

												std::string entityName = ecs.getComponent<NameTag>(entityID)->GetName();

												if (!componentFilter.PassFilter(entityName.c_str()))
												{
													continue;
												}

												bool clicked = ImGui::Selectable(entityName.c_str(), entityID == selectedEntity, ImGuiSelectableFlags_SpanAllColumns);

												if (clicked || (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
												{
													scriptValue.referenceEntityID = entityID;
													sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
													ImGui::CloseCurrentPopup();
												}
											}
										}
									// Script =========================================================================================
										else
										{
											ImGuiTextFilter scriptFilter;
											scriptFilter.Draw("##scriptFilter");
											ImGui::SameLine();
											if (ImGui::Button("Clear", ImVec2(-FLT_MIN, 0)))
											{
												scriptFilter.Clear();
											}

											if (scriptFilter.InputBuf[0] == 0 && ImGui::Selectable("None", false, ImGuiSelectableFlags_SpanAllColumns))
											{
												scriptValue.referenceEntityID = 0;
												sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
												ImGui::CloseCurrentPopup();
											}

											for (EntityID entityID : ecs.getEntities())
											{
												if (entityID == selectedEntity || !sceneManagerInstance->hasScript(entityID, scriptValue.type))
												{
													continue;
												}

												std::string entityName = ecs.getComponent<NameTag>(entityID)->GetName();

												if (!scriptFilter.PassFilter(entityName.c_str()))
												{
													continue;
												}

												bool clicked = ImGui::Selectable(entityName.c_str(), entityID == selectedEntity, ImGuiSelectableFlags_SpanAllColumns);

												if (clicked || (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
												{
													scriptValue.referenceEntityID = entityID;
													sceneManagerInstance->setScriptValue(selectedEntity, scriptName, scriptValue);
													ImGui::CloseCurrentPopup();
												}
											}
										}

									ImGui::EndPopup();
								}
								ImGui::PopID();

								ImGui::PopItemWidth();
							}
						}

						ImGui::PopItemWidth();
						ImGui::EndTable();
					}
					ImGui::TreePop();
				}
			}

			// Add component button
			static ImGuiStyle& style = ImGui::GetStyle();

			//float size = ImGui::CalcTextSize("Add Component").x + style.FramePadding.x * 2.0f;
			float size = 200.0f;
			float avail = ImGui::GetContentRegionAvail().x;

			float off = (avail - size) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

			if (ImGui::Button("Add Component", ImVec2(size, 20.f)))
			{
				currentAddComponentStage = AddComponentStage::CATEGORIES;
				ImGui::OpenPopup("componentAddPopup");
			}

			if (ImGui::GetCursorPosY() < ImGui::GetWindowHeight())
			{
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetContentRegionAvail().x / 2 + ImGui::GetWindowPos().x + style.FramePadding.x * 2.0f, ImGui::GetCursorPosY()), ImGuiCond_Appearing, ImVec2(0.5f, 0.0f));
			}
			else
			{
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetContentRegionAvail().x / 2 + ImGui::GetWindowPos().x + style.FramePadding.x * 2.0f, ImGui::GetWindowHeight()), ImGuiCond_Appearing, ImVec2(0.5f, 0.0f));
			}
			ImGui::SetNextWindowSize(ImVec2(addComponentWidth, addComponentHeight));

			// Add component popup
			if (ImGui::BeginPopupContextItem("componentAddPopup"))
			{
				static bool newScriptToggle = false;

				ImGuiTextFilter addComponentFilter;
				addComponentFilter.Draw("##addComponentFilter", -FLT_MIN);

				if (currentAddComponentStage != AddComponentStage::CATEGORIES)
				{
					ImGui::Text("<");
					ImGui::SameLine();
				}
				static std::string categories[] = { "Component", "Audio", "Cinemachine", "Effects", "Event", "Layout", "Mesh", "Miscellaneous", "Navigation", "Physics 2D", "Physics", "Playables", "Rendering", "Scripts", "Tilemap", "UI", "Video", "New script" };
				if (ImGui::Button(categories[static_cast<int>(currentAddComponentStage)].c_str(), ImVec2(-FLT_MIN, 0)))
				{
					currentAddComponentStage = AddComponentStage::CATEGORIES;
				}

				switch (currentAddComponentStage)
				{
				case AddComponentStage::CATEGORIES:

					for (int i = 1; i <= static_cast<int>(AddComponentStage::NEWSCRIPT); ++i)
					{
						if (ImGui::Selectable(categories[i].c_str(), false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_SpanAllColumns))
						{
							currentAddComponentStage = static_cast<AddComponentStage>(i);

							if (currentAddComponentStage == AddComponentStage::NEWSCRIPT)
							{
								newScriptToggle = true;
							}
						}
						ImGui::SameLine(170);
						ImGui::Text(">");
					}

					break;
				case AddComponentStage::AUDIO:

					break;
				case AddComponentStage::MISCELLANEOUS:

					break;
				case AddComponentStage::NAVIGATION:

					break;
				case AddComponentStage::PHYSICS:

					if (!GetBoxCollider(selectedEntity) && ImGui::Selectable("Box Collider"))
					{
						addComponentByName("Box Collider", selectedEntity);
					}
					if (!GetCapsuleCollider(selectedEntity) && ImGui::Selectable("Capsule Collider"))
					{
						addComponentByName("Capsule Collider", selectedEntity);
					}
					if (!GetRigidBody(selectedEntity) && ImGui::Selectable("Rigid Body"))
					{
						addComponentByName("Rigid Body", selectedEntity);
					}
					if (!GetSphereCollider(selectedEntity) && ImGui::Selectable("Sphere Collider"))
					{
						addComponentByName("Sphere Collider", selectedEntity);
					}

					break;
				case AddComponentStage::RENDERING:

					if (!GetCameraComponent(selectedEntity) && ImGui::Selectable("Camera"))
					{
						addComponentByName("Camera Component", selectedEntity);
					}
					if (!GetGraphicsComponent(selectedEntity) && ImGui::Selectable("Graphics Component"))
					{
						addComponentByName("Graphics Component", selectedEntity);
					}
					if (!GetDirLightComponent(selectedEntity) && ImGui::Selectable("DirectionalLight"))
					{
						addComponentByName("DirectionalLight", selectedEntity);
					}
					if (!GetSpotLightComponent(selectedEntity) && ImGui::Selectable("SpotLight"))
					{
						addComponentByName("SpotLight", selectedEntity);
					}
					if (!GetPointLightComponent(selectedEntity) && ImGui::Selectable("PointLight"))
					{
						addComponentByName("PointLight", selectedEntity);
					}
					if (!GetAnimationComponent(selectedEntity) && ImGui::Selectable("Animation"))
					{
						addComponentByName("AnimationComponent", selectedEntity);
					}
					if (!GetParticle_Component(selectedEntity) && ImGui::Selectable("Particle Component"))
					{
						addComponentByName("Particle Component", selectedEntity);
					}
					break;
				case AddComponentStage::SCRIPTS:

					for (auto scriptName : sceneManagerInstance->getAllScripts())
					{
						if (!sceneManagerInstance->hasScript(selectedEntity, scriptName) && ImGui::Selectable(scriptName.c_str()))
						{
							sceneManagerInstance->addScript(selectedEntity, scriptName);
						}
					}

					break;
				case AddComponentStage::UI:

					if (!GetUISprite(selectedEntity) && ImGui::Selectable("UI Sprite"))
					{
						addComponentByName("UI Sprite", selectedEntity);
					}

					break;
				case AddComponentStage::NEWSCRIPT:

					static char newScriptName[400] = "NewBehaviourScript";

					ImGui::Text("Name");

					if (newScriptToggle)
					{
						strcpy_s(newScriptName, "NewBehaviourScript");
						ImGui::SetKeyboardFocusHere();
						newScriptToggle = false;
					}
					float padding = ImGui::GetStyle().FramePadding.x;
					ImGui::PushItemWidth(addComponentWidth - padding * 2 - 5);
					bool entered = ImGui::InputText("##newScriptName", newScriptName, 400, ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();

					std::string newScriptNameString{ newScriptName };

					// Check if such script exist alr
					bool exist = false;
					std::filesystem::path filePath = sceneManagerInstance->getScenePath();
					for (auto& directory_entry : std::filesystem::directory_iterator(filePath))
					{
						std::string fileName = directory_entry.path().stem().string();
						if (newScriptNameString == fileName)
						{
							exist = true;
							break;
						}
					}

					if (exist)
					{
						ImGui::Text(("A script called \"" + newScriptNameString + "\"").c_str());
						ImGui::Text("already exist in that path.");
						ImGui::BeginDisabled();
					}

					if (ImGui::Button("Create and Add", ImVec2(-FLT_MIN, 0)) || entered)
					{
						std::filesystem::path filePath = std::filesystem::path(sceneManagerInstance->getScenePath()).parent_path().parent_path().parent_path().string() + std::string("\\ManagedScripts\\");
						std::ofstream ofs(filePath.string() + newScriptNameString + ".cs");
						std::string newClassName = newScriptNameString;
						remove_if(newClassName.begin(), newClassName.end(), isspace);
						toupper(newClassName[0]);
						std::string templateToWrite = "using ScriptAPI;\nusing System;\n\npublic class " + newScriptNameString + " : Script \n{ \n\tpublic override void Awake()\n\t{\n\n\t}\n\tpublic override void Update()\n\t{\n\n\t}\n }";
						ofs << templateToWrite;
						ofs.close();

						ImGui::CloseCurrentPopup();
					}

					if (exist)
					{
						ImGui::EndDisabled();
					}

					break;
				}

				//for (auto componentName : allComponentNames)
				//{
				//	if (getComponentByName(componentName, selectedEntity))
				//	{
				//		continue;
				//	}

				//	if (ImGui::Selectable(componentName.c_str()))
				//	{
				//		addComponentByName(componentName, selectedEntity);
				//		TDS_INFO("Added Component");
				//	}

				//	//ImGui::SameLine(170);
				//	//ImGui::Text(">");
				//}
				//for (auto scriptName : sceneManagerInstance->getAllScripts())
				//{
				//	if (!sceneManagerInstance->hasScript(selectedEntity, scriptName) && ImGui::Selectable(scriptName.c_str()))
				//	{
				//		sceneManagerInstance->addScript(selectedEntity, scriptName);
				//	}
				//}
				ImGui::EndPopup();
			}
		}
	}


	/*!*************************************************************************
	This function is the helper function for Properties panel
	****************************************************************************/
	void Properties::ImguiComponentDisplay(std::string componentName, IComponent* componentBase)
	{

		rttr::type type = rttr::type::get_by_name(componentName);
		rttr::instance componentInstance = componentBase;

		for (rttr::property propertyName : type.get_properties())
		{
			if (propertyName.get_type() == rttr::type::get<int>())
			{
				int newValue = propertyName.get_value(componentInstance).convert<int>();
				ImguiInput(propertyName.get_name().to_string(), newValue);
				propertyName.set_value(componentInstance, newValue);
			}
			if (propertyName.get_type() == rttr::type::get<EntityID>())
			{
				int newValue = propertyName.get_value(componentInstance).convert<int>();
				ImguiInput(propertyName.get_name().to_string(), newValue);
				propertyName.set_value(componentInstance, newValue);
			}
			else if (propertyName.get_type() == rttr::type::get<float>())
			{
				float newValue = propertyName.get_value(componentInstance).convert<float>();
				ImguiInput(propertyName.get_name().to_string(), newValue, 0.0001f);
				propertyName.set_value(componentInstance, newValue);
			}
			else if (propertyName.get_type() == rttr::type::get<bool>())
			{
				bool newValue = propertyName.get_value(componentInstance).convert<bool>();
				ImguiInput(propertyName.get_name().to_string(), newValue);
				propertyName.set_value(componentInstance, newValue);
			}
			else if (propertyName.get_type() == rttr::type::get<MaterialAttributes>())
			{
				if (componentName == "Graphics Component")
				{
					GraphicsComponent* g = reinterpret_cast<GraphicsComponent*>(componentBase);

					#undef ADD_SHADING_ENUM


					#define ADD_SHADING_ENUM(x, y) \
								materialShadingEnum[y] = #x;


					if (g->m_UseMaterials)
					{
						static std::vector<std::string> materialShadingEnum;

						if (materialShadingEnum.empty())
						{
							materialShadingEnum.resize(3);
							MATERIAL_SHADING_LIST
						}
						
						static int newValue = g->m_MaterialAttributes.m_shading;
						
						if (ImGui::BeginCombo("Shading", materialShadingEnum[newValue].c_str()))
						{
							for (int i = 0; i < materialShadingEnum.size(); i++)
							{
								bool isSelected = (newValue == i);
								if (ImGui::Selectable(materialShadingEnum[i].c_str(), isSelected))
								{
									newValue = i;
								}
								if (isSelected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
						g->m_MaterialAttributes.m_shading = (MaterialAttributes::SHADING_TYPE)newValue;
					
						if (g->m_MaterialAttributes.m_shading == MaterialAttributes::SHADING_PHONG_BLINN)
						{
	
							Vec3 newDiffuseColor = g->m_MaterialAttributes.m_phongBlinn.m_DiffuseColor;
							ImguiInput("Diffuse Color", newDiffuseColor);
							g->m_MaterialAttributes.m_phongBlinn.m_DiffuseColor = newDiffuseColor;

							Vec3 newSpecularColor = g->m_MaterialAttributes.m_phongBlinn.m_SpecularColor;
							ImguiInput("Specular Color", newSpecularColor);
							g->m_MaterialAttributes.m_phongBlinn.m_SpecularColor = newSpecularColor;

							Vec3 newEmissiveColor = g->m_MaterialAttributes.m_phongBlinn.m_EmissiveColor;
							ImguiInput("Emissive Color", newEmissiveColor);
							g->m_MaterialAttributes.m_phongBlinn.m_EmissiveColor = newEmissiveColor;

							Vec3 newAmbientColor = g->m_MaterialAttributes.m_phongBlinn.m_AmbientColor;
							ImguiInput("Ambient Color", newAmbientColor);
							g->m_MaterialAttributes.m_phongBlinn.m_AmbientColor = newAmbientColor;

							float newShininess = g->m_MaterialAttributes.m_phongBlinn.m_Shininess;
							ImguiInput("Shininess", newShininess);
							g->m_MaterialAttributes.m_phongBlinn.m_Shininess = newShininess;

							float newShininessStrength = g->m_MaterialAttributes.m_phongBlinn.m_ShininessStrength;
							ImguiInput("Shininess Strength", newShininessStrength);
							g->m_MaterialAttributes.m_phongBlinn.m_ShininessStrength = newShininessStrength;

							std::string diffuseTex = g->m_MaterialAttributes.m_phongBlinn.m_DiffuseTex;
							ImguiInput("Diffuse Texture", diffuseTex);
							if (ImGui::BeginDragDropTarget())
							{
								std::string finaltexture = g->m_MaterialAttributes.m_phongBlinn.m_DiffuseTex;
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									const wchar_t* path = (const wchar_t*)payload->Data;
									std::wstring ws(path);
									// your new String
									std::string str(ws.begin(), ws.end());
									const std::filesystem::path filesystempath = str;
									if (filesystempath.extension() == ".dds")
									{
										std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
										Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finaltexture, nullptr);
										
										if (filesystempath.extension() == ".dds")
										{
											g->m_MaterialAttributes.m_phongBlinn.m_DiffuseTex = finaltexture;
										}
										std::wcout << " Path of dragged file is: " << path << std::endl;
									}
								}
							}

							std::string specularTex = g->m_MaterialAttributes.m_phongBlinn.m_SpecularTex;
							ImguiInput("Specular Texture", specularTex);
							//Do the same for the drag and drop for the rest of the textures
							if (ImGui::BeginDragDropTarget())
							{
								std::string finaltexture = g->m_MaterialAttributes.m_phongBlinn.m_SpecularTex;
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									const wchar_t* path = (const wchar_t*)payload->Data;
									std::wstring ws(path);
									// your new String
									std::string str(ws.begin(), ws.end());
									const std::filesystem::path filesystempath = str;
									if (filesystempath.extension() == ".dds")
									{
										std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
										Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finaltexture, nullptr);
										if (filesystempath.extension() == ".dds")
										{
											g->m_MaterialAttributes.m_phongBlinn.m_SpecularTex = finaltexture;
										}
										std::wcout << " Path of dragged file is: " << path << std::endl;
									}
								}
							}

							std::string ambientTex = g->m_MaterialAttributes.m_phongBlinn.m_AmbientTex;
							ImguiInput("Ambient Texture", ambientTex);

							if (ImGui::BeginDragDropTarget())
							{
								std::string finaltexture = g->m_MaterialAttributes.m_phongBlinn.m_AmbientTex;
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									const wchar_t* path = (const wchar_t*)payload->Data;
									std::wstring ws(path);
									// your new String
									std::string str(ws.begin(), ws.end());
									const std::filesystem::path filesystempath = str;
									if (filesystempath.extension() == ".dds")
									{
										std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
										Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finaltexture, nullptr);
										if (filesystempath.extension() == ".dds")
										{
											g->m_MaterialAttributes.m_phongBlinn.m_AmbientTex = finaltexture;
										}
										std::wcout << " Path of dragged file is: " << path << std::endl;
									}
								}
							}

							std::string emissiveTex = g->m_MaterialAttributes.m_phongBlinn.m_EmissiveTex;
							ImguiInput("Emissive Texture", emissiveTex);

							if (ImGui::BeginDragDropTarget())
							{
								std::string finaltexture = g->m_MaterialAttributes.m_phongBlinn.m_EmissiveTex;
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									const wchar_t* path = (const wchar_t*)payload->Data;
									std::wstring ws(path);
									// your new String
									std::string str(ws.begin(), ws.end());
									const std::filesystem::path filesystempath = str;
									if (filesystempath.extension() == ".dds")
									{
										std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
										Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finaltexture, nullptr);
										if (filesystempath.extension() == ".dds")
										{
											g->m_MaterialAttributes.m_phongBlinn.m_EmissiveTex = finaltexture;
										}
										std::wcout << " Path of dragged file is: " << path << std::endl;
									}
								}
							}

							std::string shininessTex = g->m_MaterialAttributes.m_phongBlinn.m_ShininessTex;
							ImguiInput("Shininess Texture", shininessTex);

							if (ImGui::BeginDragDropTarget())
							{
								std::string finaltexture = g->m_MaterialAttributes.m_phongBlinn.m_ShininessTex;
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									const wchar_t* path = (const wchar_t*)payload->Data;
									std::wstring ws(path);
									// your new String
									std::string str(ws.begin(), ws.end());
									const std::filesystem::path filesystempath = str;
									if (filesystempath.extension() == ".dds")
									{
										std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
										Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finaltexture, nullptr);
										if (filesystempath.extension() == ".dds")
										{
											g->m_MaterialAttributes.m_phongBlinn.m_ShininessTex = finaltexture;
										}
										std::wcout << " Path of dragged file is: " << path << std::endl;
									}
								}
							}

						}
						//else if (g->m_MaterialAttributes.m_shading == MaterialAttributes::SHADING_PBR)
						//{
						//
						//    float newMetalness = g->m_MaterialAttributes.m_PBRAttributes.m_Metalness;
						//	ImguiInput("Metalness", newMetalness);

						//	float newRoughness = g->m_MaterialAttributes.m_PBRAttributes.m_Roughness;
						//	ImguiInput("Roughness", newRoughness);

						//	float newEmissive = g->m_MaterialAttributes.m_PBRAttributes.m_Emissive;
						//	ImguiInput("Emissive", newEmissive);

						//	std::string normalTex = g->m_MaterialAttributes.m_PBRAttributes.m_NormalTex;
						//	ImguiInput("normal Texture", normalTex);
						//	if (ImGui::BeginDragDropTarget())
						//	{
						//		std::string finalNormalTex = g->m_MaterialAttributes.m_PBRAttributes.m_NormalTex;
						//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						//		{
						//			const wchar_t* path = (const wchar_t*)payload->Data;
						//			std::wstring ws(path);
						//			// your new String
						//			std::string str(ws.begin(), ws.end());
						//			const std::filesystem::path filesystempath = str;
						//			if (filesystempath.extension() == ".dds")
						//			{
						//				std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
						//				Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalNormalTex, nullptr);
						//				if (filesystempath.extension() == ".dds")
						//				{
						//					g->m_MaterialAttributes.m_PBRAttributes.m_NormalTex = finalNormalTex;
						//				}
						//				std::wcout << " Path of dragged file is: " << path << std::endl;
						//			}
						//		}
						//	}

						//	std::string roughnessTex = g->m_MaterialAttributes.m_PBRAttributes.m_RoughnessTex;
						//	ImguiInput("Roughness Texture", roughnessTex);
						//	if (ImGui::BeginDragDropTarget())
						//	{
						//		std::string finalRoughnessTex = g->m_MaterialAttributes.m_PBRAttributes.m_RoughnessTex;
						//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						//		{
						//			const wchar_t* path = (const wchar_t*)payload->Data;
						//			std::wstring ws(path);
						//			// your new String
						//			std::string str(ws.begin(), ws.end());
						//			const std::filesystem::path filesystempath = str;
						//			if (filesystempath.extension() == ".dds")
						//			{
						//				std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
						//				Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalRoughnessTex, nullptr);
						//				if (filesystempath.extension() == ".dds")
						//				{
						//					g->m_MaterialAttributes.m_PBRAttributes.m_RoughnessTex = finalRoughnessTex;
						//				}
						//				std::wcout << " Path of dragged file is: " << path << std::endl;
						//			}
						//		}
						//	}

						//	std::string metallicTex = g->m_MaterialAttributes.m_PBRAttributes.m_MetallicTex;
						//	ImguiInput("Metallic Texture", metallicTex);
						//	if (ImGui::BeginDragDropTarget())
						//	{
						//		std::string finalMetallicTex = g->m_MaterialAttributes.m_PBRAttributes.m_MetallicTex;
						//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						//		{
						//			const wchar_t* path = (const wchar_t*)payload->Data;
						//			std::wstring ws(path);
						//			// your new String
						//			std::string str(ws.begin(), ws.end());
						//			const std::filesystem::path filesystempath = str;
						//			if (filesystempath.extension() == ".dds")
						//			{
						//				std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
						//				Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalMetallicTex, nullptr);
						//				if (filesystempath.extension() == ".dds")
						//				{
						//					g->m_MaterialAttributes.m_PBRAttributes.m_MetallicTex = finalMetallicTex;
						//				}
						//				std::wcout << " Path of dragged file is: " << path << std::endl;
						//			}
						//		}
						//	}

						//	std::string emissiveTex = g->m_MaterialAttributes.m_PBRAttributes.m_EmissiveTex;
						//	ImguiInput("Emissive Texture", emissiveTex);
						//	if (ImGui::BeginDragDropTarget())
						//	{
						//		std::string finalEmissiveTex = g->m_MaterialAttributes.m_PBRAttributes.m_EmissiveTex;
						//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						//		{
						//			const wchar_t* path = (const wchar_t*)payload->Data;
						//			std::wstring ws(path);
						//			// your new String
						//			std::string str(ws.begin(), ws.end());
						//			const std::filesystem::path filesystempath = str;
						//			if (filesystempath.extension() == ".dds")
						//			{
						//				std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
						//				Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalEmissiveTex, nullptr);
						//				if (filesystempath.extension() == ".dds")
						//				{
						//					g->m_MaterialAttributes.m_PBRAttributes.m_EmissiveTex = finalEmissiveTex;
						//				}
						//				std::wcout << " Path of dragged file is: " << path << std::endl;
						//			}
						//		}
						//	}

						//	std::string aoTex = g->m_MaterialAttributes.m_PBRAttributes.m_AOTex;
						//	ImguiInput("AO Texture", aoTex);
						//	if (ImGui::BeginDragDropTarget())
						//	{
						//		std::string finalAOTex = g->m_MaterialAttributes.m_PBRAttributes.m_AOTex;
						//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						//		{
						//			const wchar_t* path = (const wchar_t*)payload->Data;
						//			std::wstring ws(path);
						//			// your new String
						//			std::string str(ws.begin(), ws.end());
						//			const std::filesystem::path filesystempath = str;
						//			if (filesystempath.extension() == ".dds")
						//			{
						//				std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
						//				Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalAOTex, nullptr);
						//				if (filesystempath.extension() == ".dds")
						//				{
						//					g->m_MaterialAttributes.m_PBRAttributes.m_AOTex = finalAOTex;
						//				}
						//				std::wcout << " Path of dragged file is: " << path << std::endl;
						//			}
						//		}
						//	}

						//}

						float newReflectivity = g->m_MaterialAttributes.m_Reflectivity;
						ImguiInput("Reflectivity", newReflectivity);
						g->m_MaterialAttributes.m_Reflectivity = newReflectivity;

						Vec3 newReflectance = g->m_MaterialAttributes.m_Reflectance;
						ImguiInput("Reflectance", newReflectance);
						g->m_MaterialAttributes.m_Reflectance = newReflectance;

						std::string reflectanceTex = g->m_MaterialAttributes.m_ReflectanceTexture;
						ImguiInput("Reflectance Texture", reflectanceTex);
						if (ImGui::BeginDragDropTarget())
						{
							std::string finalAOTex = g->m_MaterialAttributes.m_ReflectanceTexture;
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
							{
								const wchar_t* path = (const wchar_t*)payload->Data;
								std::wstring ws(path);
								// your new String
								std::string str(ws.begin(), ws.end());
								const std::filesystem::path filesystempath = str;
								if (filesystempath.extension() == ".dds")
								{
									std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
									Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalAOTex, nullptr);
									if (filesystempath.extension() == ".dds")
									{
										g->m_MaterialAttributes.m_ReflectanceTexture = finalAOTex;
									}
									std::wcout << " Path of dragged file is: " << path << std::endl;
								}
							}
						}
						

					}
				}
			}
			else if (propertyName.get_type() == rttr::type::get<std::string>())
			{
				std::string newValue = propertyName.get_value(componentInstance).convert<std::string>();
				ImguiInput(propertyName.get_name().to_string(), newValue);

				// To drag drop texture name - will change and take out after
				if (componentName == "Graphics Component" && ImGui::BeginDragDropTarget())
				{
					GraphicsComponent* g = reinterpret_cast<GraphicsComponent*>(componentBase);
					std::string finaltexture = g->GetTextureName();
					std::string finalmodel = g->GetModelName(); 
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						// your new String
						std::string str(ws.begin(), ws.end());
						const std::filesystem::path filesystempath = str;

						if (propertyName.get_name() == "TextureName")
						{
							if (filesystempath.extension() == ".dds" || filesystempath.extension() == ".jpg" || filesystempath.extension() == ".png")
							{

								AssetBrowser assetbroswer;
								assetbroswer.getFileNameFromPath(str.c_str(), nullptr, nullptr, &finaltexture, nullptr);
								//g.m_TextureName = final;

								if (filesystempath.extension() != ".dds")
								{
									g->SetTextureName(assetbroswer.LoadAssetRevamped(finaltexture));
								}
								else
								{
									g->SetTextureName(finaltexture);
								}


								std::wcout << " Path of dragged file is: " << path << std::endl;
							}
							else
							{

								TDS_INFO("invalid file type, please drag a .dds for TextureName");
							}

						}
						if (propertyName.get_name() == "ModelName")
						{
							if (filesystempath.extension() == ".bin" || filesystempath.extension() == ".obj" || filesystempath.extension() == ".fbx"
								|| filesystempath.extension() == ".gltf")
							{

								AssetBrowser assetbroswer;
								assetbroswer.getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalmodel, nullptr);
								//g.m_TextureName = final;
								g->SetModelName(assetbroswer.LoadAssetRevamped(finalmodel));

								std::wcout << " Path of dragged file is: " << path << std::endl;
							}
							else
							{
								TDS_INFO("invalid file type, please drag a .bin or .fbx for ModelName ");
							}
						}
					}
					else
					{
						propertyName.set_value(componentInstance, newValue);
					}
					ImGui::EndDragDropTarget();
				}
				else if (componentName == "UI Sprite" && ImGui::BeginDragDropTarget())
				{
					UISprite* ui = reinterpret_cast<UISprite*>(componentBase);
					std::string finaltexture = ui->m_TextureName;
					std::string finalFont = ui->m_FontName;
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						// your new String
						std::string str(ws.begin(), ws.end());
						const std::filesystem::path filesystempath = str;
						if (propertyName.get_name() == "Texture")
						{
							if (filesystempath.extension() == ".dds" || filesystempath.extension() == ".jpg" || filesystempath.extension() == ".png")
							{
								Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finaltexture, nullptr);

								if (filesystempath.extension() == ".dds")
								{
									ui->m_TextureName = finaltexture;
								}
								else
								{
									ui->m_TextureName = Assetbrowser->LoadAssetRevamped(finaltexture);
								}


								std::wcout << " Path of dragged file is: " << path << std::endl;
							}
							else
							{
								TDS_INFO("invalid file type, please drag a .dds for TextureName");
							}
						}
						else if (propertyName.get_name() == "Font Texture")
						{
							if (filesystempath.extension() == ".ttf")
							{
								Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalFont, nullptr);
								ui->m_FontName = Assetbrowser->LoadAssetRevamped(finalFont);
								std::wcout << " Path of dragged file is: " << path << std::endl;
							}
							else if (filesystempath.extension() == ".dds")
							{
								ui->m_FontName = finalFont;
							}
							else
							{

								TDS_INFO("invalid file type, please drag a .ttf or a .dds for Font");
							}
						}
					}
				}
				else if (componentName == "Audio" && ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);
						// your new String
						std::string str(ws.begin(), ws.end());
						const std::filesystem::path filesystempath = str;

						if (propertyName.get_name() == "File path")
						{
							if (filesystempath.extension() == ".flac" || filesystempath.extension() == ".wav" || filesystempath.extension() == ".mp3")
							{
								SoundInfo* si = reinterpret_cast<SoundInfo*>(componentBase);
								si->setFilePath(str);

								AssetBrowser Assetbrowser;
								Assetbrowser.getFileNameFromPath(str.c_str(), nullptr, nullptr, &str, nullptr);

								std::wcout << " Path of dragged file is: " << path << std::endl;
							}
							else
							{
								TDS_INFO("invalid file type, please drag a .flac/.wav/.mp3 for filePath");
							}
						}
					}
				}
				else if (componentName == "AnimationComponent" && ImGui::BeginDragDropTarget())
				{
					AnimationComponent* anim = reinterpret_cast<AnimationComponent*>(componentBase);
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						std::shared_ptr<AssetBrowser> Assetbrowser = static_pointer_cast<AssetBrowser>(LevelEditorManager::GetInstance()->panels[PanelTypes::ASSETBROWSER]);
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::wstring ws(path);

						std::string finalPackName = anim->m_AnimationPackName;
						
						std::string str(ws.begin(), ws.end());

						const std::filesystem::path filesystempath = str;
						std::string filename = filesystempath.filename().string();
						if (propertyName.get_name() == "AnimationPackName")
						{
							if (filesystempath.extension() == ".json")
							{
								Assetbrowser->getFileNameFromPath(str.c_str(), nullptr, nullptr, &finalPackName, nullptr);
								anim->m_AnimationPackName = Assetbrowser->LoadAssetRevamped(finalPackName);
								//AnimationData::Serialize(anim->getAnimationData(), filename, true);
								//anim->setAnimationJsonFile(filename);
								//if (anim->getAnimationData().m_Animations.size() > 0)
								//{
								//	anim->m_AnimationPlayer.setAnimation(anim->getAnimationData().m_Animations[0]);
								//}
							}

						}
					}
				}
				else
				{
					propertyName.set_value(componentInstance, newValue);
				}
			}
			else if (propertyName.get_type() == rttr::type::get<Vec2>())
			{
				Vec2 newValue = propertyName.get_value(componentInstance).convert<Vec2>();
				ImguiInput(propertyName.get_name().to_string(), newValue);
				propertyName.set_value(componentInstance, newValue);
			}
			else if (propertyName.get_type() == rttr::type::get<Vec3>())
			{
				if (propertyName.get_name().to_string() == "FakePosition"
					|| propertyName.get_name().to_string() == "FakeRotation"
					|| propertyName.get_name().to_string() == "FakeScale"
					|| propertyName.get_name().to_string() == "OldPosition"
					|| propertyName.get_name().to_string() == "OldScale"
					|| propertyName.get_name().to_string() == "OldRotation"
					|| propertyName.get_name().to_string() == "OldFakePosition"
					|| propertyName.get_name().to_string() == "OldFakeScale"
					|| propertyName.get_name().to_string() == "OldFakeRotation") continue;

				Vec3 newValue = propertyName.get_value(componentInstance).convert<Vec3>();
				ImguiInput(propertyName.get_name().to_string(), newValue);
				propertyName.set_value(componentInstance, newValue);
			}
			else if (propertyName.get_type() == rttr::type::get<Vec4>())
			{
				Vec4 newValue = propertyName.get_value(componentInstance).convert<Vec4>();
				ImguiInput(propertyName.get_name().to_string(), newValue);
				propertyName.set_value(componentInstance, newValue);
			}
			else if (propertyName.get_type() == rttr::type::get<RigidBody::MotionType>())
			{
				static std::vector<std::string> rigidbodyMotionTypeString = { "STATIC", "DYNAMIC" };
				int newValue = propertyName.get_value(componentInstance).convert<int>();
				ImguiInput(propertyName.get_name().to_string(), rigidbodyMotionTypeString, newValue);
				propertyName.set_value(componentInstance, static_cast<RigidBody::MotionType>(newValue));
			}
		}
	}

	/*!*************************************************************************
	This function is a helper function for draw TEXT variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, std::string& textVariable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		char temp[100];
		strcpy_s(temp, textVariable.c_str());
		ImGui::InputText(("##" + variableName).c_str(), temp, 100);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			textVariable = std::string(temp);
			return true;
		}
		return false;
	}

	/*!*************************************************************************
	This function is a helper function for draw BOOl variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, bool& boolVariable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		ImGui::Checkbox(("##" + variableName).c_str(), &boolVariable);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			return true;
		}
		return false;
	}

	/*!*************************************************************************
	This function is a helper function for draw INT variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, int& intVariable, float speed, int min, int max)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		ImGui::InputInt(("##" + variableName).c_str(), &intVariable, 0/*, speed, min, max*/);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			return true;
		}
		return false;
	}

	/*!*************************************************************************
	This function is a helper function for draw FLOAT variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, float& floatVariable, float speed, float min, float max)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		ImGui::InputFloat(("##" + variableName).c_str(), &floatVariable/*, speed, min, max*/);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			return true;
		}
		return false;
	}

	/*!*************************************************************************
	This function is a helper function for draw VEC2 variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, Vec2& Vec2Variable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		float temp[2]{ Vec2Variable.x, Vec2Variable.y };
		ImGui::InputFloat2(("##" + variableName).c_str(), temp/*, 0.05f*/);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vec2Variable.x = temp[0];
			Vec2Variable.y = temp[1];

			return true;
		}
		return false;
	}

	/*!*************************************************************************
	This function is a helper function for draw VEC3 variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, Vec3& Vec3Variable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		float temp[3]{ Vec3Variable.x, Vec3Variable.y, Vec3Variable.z };

		static ImGuiStyle& style = ImGui::GetStyle();
		auto tagWidth = ImGui::CalcTextSize("X").x + style.FramePadding.x * 2;
		auto inputWidth = (ImGui::GetColumnWidth() - tagWidth * 3) / 3.f;

		std::string format = "";

		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::PushItemWidth(inputWidth);
		format = (temp[0] == floor(temp[0]) ? "%.0f" : "%.3f");
		ImGui::InputFloat(("##" + variableName + "X").c_str(), &temp[0], 0.0f, 0.0f, format.c_str());
		ImGui::PopItemWidth();

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vec3Variable.x = temp[0];

			return true;
		}

		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::PushItemWidth(inputWidth);
		format = (temp[1] == floor(temp[1]) ? "%.0f" : "%.3f");
		ImGui::InputFloat(("##" + variableName + "Y").c_str(), &temp[1], 0.0f, 0.0f, format.c_str());
		ImGui::PopItemWidth();

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vec3Variable.y = temp[1];

			return true;
		}

		ImGui::SameLine();
		ImGui::Text("Z");
		ImGui::SameLine();
		//ImGui::PushItemWidth(inputWidth);
		ImGui::PushItemWidth(-FLT_MIN); // Right-aligned
		format = (temp[2] == floor(temp[2]) ? "%.0f" : "%.3f");
		ImGui::InputFloat(("##" + variableName + "Z").c_str(), &temp[2], 0.0f, 0.0f, format.c_str());
		ImGui::PopItemWidth();

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vec3Variable.z = temp[2];

			return true;
		}
		return false;
	}

	/*!*************************************************************************
	This function is a helper function for draw VEC4 variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, Vec4& Vec4Variable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();
		float temp[4]{ Vec4Variable.x, Vec4Variable.y, Vec4Variable.z, Vec4Variable.w };

		for (auto& c : variableName)
		{
			c = std::tolower(c);
		}

		if (variableName.find("color") != std::string::npos)
		{
			ImGui::ColorEdit4(("##" + variableName).c_str(), temp);
			Vec4Variable.x = temp[0];
			Vec4Variable.y = temp[1];
			Vec4Variable.z = temp[2];
			Vec4Variable.w = temp[3];
			return true;
		}

		ImGui::InputFloat4(("##" + variableName).c_str(), temp);

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Vec4Variable.x = temp[0];
			Vec4Variable.y = temp[1];
			Vec4Variable.z = temp[2];
			Vec4Variable.w = temp[3];

			return true;
		}
		return false;
	}

	/*!*************************************************************************
	This function is a helper function for draw ENUM variables
	****************************************************************************/
	bool ImguiInput(std::string variableName, std::vector<std::string>& enumString, int& enumVariable)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text(variableName.c_str());

		ImGui::TableNextColumn();

		// Uninitialized
		if (enumVariable == -1)
		{
			enumVariable = 0;
		}

		if (ImGui::BeginCombo(("##" + variableName).c_str(), enumString[enumVariable].c_str()))
		{
			for (int n = 0; n < enumString.size(); n++)
			{
				const bool is_selected = (enumVariable == n);
				if (ImGui::Selectable(enumString[n].c_str(), is_selected))
				{
					enumVariable = n;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		return true;
	}
}