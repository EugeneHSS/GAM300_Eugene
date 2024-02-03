#include "imguiHelper/ImguiScene.h"
#include "imguiHelper/ImguiAssetBrowser.h"
#include "GraphicsResource/TextureInfo.h"
#include "vulkanTools/VulkanTexture.h"
#include <sstream>
#include <corecrt_wstring.h>
#include "imguiHelper/ImguiConsole.h"
#include "Rendering/GraphicsManager.h"
#include "Rendering/RenderTarget.h"
#include "vulkanTools/Renderer.h"
#include "imgui/ImGuizmo.h"
#include "eventManager/eventHandler.h"
#include "../EditorApp.h"
//#include "Input/Input.h"
namespace TDS
{
	Texture data{};
	VkDescriptorSet m_DescSet = nullptr;
	std::shared_ptr<EditorConsole> consolelog = static_pointer_cast<EditorConsole>(LevelEditorManager::GetInstance()->panels[PanelTypes::CONSOLE]);

	EditorScene::EditorScene()
	{
		//selected = 0;
		//selectedFolder = -1;
		//renameCheck = false;

		flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse| ImGuiDockNodeFlags_AutoHideTabBar;
		panelTitle = "Scene";
		windowPadding = ImVec2(0.f, 0.f);

	}

	std::string tempPath = "../assets/textures/texture.dds";
	void EditorScene::init()
	{
		m_DescSet = ImGui_ImplVulkan_AddTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	void EditorScene::update()
	{
		isFocus = ImGui::IsWindowFocused() && ImGui::IsItemVisible();
		/*TDS_INFO("Window Height is: ");
		TDS_INFO(ImGui::GetWindowHeight());
		TDS_INFO("Content Height is: ");
		TDS_INFO(ImGui::GetContentRegionAvail().y);*/
		if (ImGui::BeginMenuBar())
		{
			if (isPlaying)
			{
				if (ImGui::BeginMenu("Scene is Playing..."))
				{
					ImGui::EndMenu();
				}
			}
			else
			{
				if (ImGui::BeginMenu("Scene is Paused"))
				{
					ImGui::EndMenu();
				}
			}
			ImGui::EndMenuBar();
		}
		//ImGui::Image(reinterpret_cast<void*>(vkTexture.m_DescSet), ImVec2{ 200, 200 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		//	{
		//		const wchar_t* path = (const wchar_t*)payload->Data;
		//		std::wstring ws(path);
		//		// your new String
		//		std::string str(ws.begin(), ws.end());
		//		const std::filesystem::path filesystempath = str;

		//		if (filesystempath.extension() != ".dds")
		//		{
		//			consolelog->AddLog("Invalid file type, please select a .DDS file.");
		//		}
		//		else
		//		{
		//			tempPath = str;
		//			std::wcout << " Path is: " << path << std::endl;

		//			//currently i am only changing filepath names, next time i need to impose pictures onto pictures, create new image layer 
		//			//and also parse each image's width and height, rn its hardcoded to 500 x 500 px
		//			/*tempPath = path;
		//			data.LoadTexture(tempPath);
		//			vkTexture.CreateBasicTexture(data.m_TextureInfo);*/

		//		}
		//	}
		//	ImGui::EndDragDropTarget();
		//}

		isFocus = ImGui::IsWindowFocused() && ImGui::IsItemVisible();
		static bool view2D = false;
		
		ImVec2 vSize = ImGui::GetContentRegionAvail();

		GraphicsManager::getInstance().getViewportScreen().x = ImGui::GetWindowPos().x;
		GraphicsManager::getInstance().getViewportScreen().y = ImGui::GetWindowPos().y;
		GraphicsManager::getInstance().getViewportScreen().z = ImGui::GetContentRegionAvail().x;
		GraphicsManager::getInstance().getViewportScreen().w = ImGui::GetContentRegionAvail().y;
		GraphicsManager::getInstance().getOffset() = ImGui::GetWindowHeight();
		ImGui::Image((ImTextureID)m_DescSet, vSize);
		//drag drop code MUST be directly under imgui::image code
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::wstring ws(path);
				// your new String
				std::string str(ws.begin(), ws.end());
				const std::filesystem::path filesystempath = str;

				//if (filesystempath.extension() != ".dds")
				//{
				//	consolelog->AddLog("Invalid file type, please select a .DDS file.");
				//}
				//else
				//{
				//	tempPath = str;
					std::wcout << " Path of dragged file is: " << path << std::endl;

				//	//currently i am only changing filepath names, next time i need to impose pictures onto pictures, create new image layer 
				//	//and also parse each image's width and height, rn its hardcoded to 500 x 500 px
				//	/*tempPath = path;
				//	data.LoadTexture(tempPath);
				//	vkTexture.CreateBasicTexture(data.m_TextureInfo);*/

				//}
			}
			ImGui::EndDragDropTarget();
		}

		std::shared_ptr<Hierarchy> hierarchyPanel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[PanelTypes::HIERARCHY]);


		static EntityID selectedEntity{};

		selectedEntity = hierarchyPanel->getSelectedEntity();

		if (Input::isMouseButtonPressed(TDS_MOUSE_LEFT) && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
		{
			if (GraphicsManager::getInstance().getObjectPicker().getActiveObject() != 0 && GraphicsManager::getInstance().getObjectPicker().getActiveObject() < 10000)
			{
				selectedEntity = GraphicsManager::getInstance().getObjectPicker().getActiveObject();

				hierarchyPanel->setSelectedEntity(selectedEntity);
			}
			Input::releaseTheMouse(TDS_MOUSE_LEFT);
		}


		if (selectedEntity)
		{
			if (GraphicsManager::getInstance().IsViewingFrom2D())
			{
				GraphicsComponent* graphComp = reinterpret_cast<GraphicsComponent*>(getComponentByName("Graphics Component", selectedEntity));
				if (graphComp != nullptr && graphComp->m_UsedIn2D)
				{
					view2D = false;
					ImGuizmo::SetOrthographic(false);
				}
				else
				{
					view2D = true;
					ImGuizmo::SetOrthographic(true);
				}

			}
			else
			{
				UISprite* sprite = reinterpret_cast<UISprite*>(getComponentByName("UI Sprite", selectedEntity));

				if (sprite != nullptr)
				{
					view2D = true;
					ImGuizmo::SetOrthographic(true);
				}
				else
				{
					view2D = false;
					ImGuizmo::SetOrthographic(false);
				}
				
			}
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			Mat4 projection{};
			//projection.m[1][1] *= -1;

			Mat4 view = Mat4::identity();
			if (view2D)
			{
				projection = Mat4::Ortho(
					-1.f,
					1.f,
					-1.f,
					1.f,
					-1.f,
					1.f
				);
			}
			else
			{
				projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
					GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 10.f);
				view = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
				//projection.m[1][1] *= -1;
			}


			const auto& trans = ecs.getComponent<Transform>(selectedEntity);

			if (!trans)
			{
				return;
			}

			Vec3 snap = 1.0f;

			float* _proj = Mat4::Mat4Value_ptr(projection);
			float* _view = Mat4::Mat4Value_ptr(view);
			float* _trans = Mat4::Mat4Value_ptr(trans->GetTransformMatrix());
			float* _snap = Vec3::Vec3Value_ptr(snap);
			if (TDS::Input::isKeyPressed(TDS_1)) { m_gizmoType = static_cast<int>(ImGuizmo::OPERATION::TRANSLATE); }
			else if (TDS::Input::isKeyPressed(TDS_2)) { m_gizmoType = static_cast<int>(ImGuizmo::OPERATION::SCALE); }
			else if (TDS::Input::isKeyPressed(TDS_3)) { m_gizmoType = static_cast<int>(ImGuizmo::OPERATION::ROTATE); }
			bool val = ImGuizmo::Manipulate(
				_view, _proj, (ImGuizmo::OPERATION)m_gizmoType, ImGuizmo::WORLD, _trans,
				nullptr, false ? _snap : nullptr
			);

			m_gizmoActive = ImGuizmo::IsUsing();

			if (ImGuizmo::IsUsing() && m_gizmoType != -1)
			{
				Vec3 transl{};
				Vec3 rotat{};
				Vec3 scal{};

				Vec3 oldPosition = trans->GetPosition();
				Vec3 oldScale = trans->GetScale();
				Vec3 oldRotation = trans->GetRotation();

				float* _transl = Vec3::Vec3Value_ptr(transl);
				float* _rotat = Vec3::Vec3Value_ptr(rotat);
				float* _scal = Vec3::Vec3Value_ptr(scal);

				ImGuizmo::DecomposeMatrixToComponents(_trans, _transl, _rotat, _scal);

				trans->SetPosition(_transl[0], _transl[1], _transl[2]);
				trans->SetRotation(_rotat[0] * Mathf::Deg2Rad, _rotat[1] * Mathf::Deg2Rad, _rotat[2] * Mathf::Deg2Rad);
				if (_scal[0] > 0.f && _scal[1] > 0.f && _scal[2] > 0.f)
					trans->SetScale(_scal[0], _scal[1], _scal[2]);

				EventHandler::postChildTransformationEvent(selectedEntity, oldPosition, oldScale, oldRotation);

				delete[] _transl;
				delete[] _rotat;
				delete[] _scal;
			}
			delete[] _proj;
			delete[] _view;
			delete[] _trans;
			delete[] _snap;
		}
		
	}
	void EditorScene::Resize()
	{
		if (m_DescSet)
		{
			//ImGui_ImplVulkan_RemoveTexture(m_DescSet);
			//m_DescSet = ImGui_ImplVulkan_AddTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			ImGui_ImplVulkan_UpdateTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), m_DescSet, GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}
}