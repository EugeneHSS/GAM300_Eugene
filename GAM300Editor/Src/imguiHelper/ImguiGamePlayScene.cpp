#include "imguiHelper/ImguiGamePlayScene.h"
#include "Rendering/GraphicsManager.h"
#include "Rendering/RenderTarget.h"


TDS::GamePlayScene::GamePlayScene()
{
	flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiDockNodeFlags_AutoHideTabBar;
	panelTitle = "GamePlayScene";
	windowPadding = ImVec2(0.f, 0.f);

	Input::setExitCursor(true);
	show_cursor = 1;
}

void TDS::GamePlayScene::init()
{
	m_GamePlayDesc = ImGui_ImplVulkan_AddTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}
void TDS::GamePlayScene::update()
{
	isFocus = ImGui::IsWindowFocused() && ImGui::IsItemVisible();
	if (ImGui::BeginMenuBar())
	{
		if (isPlaying)
		{
			if (Input::isKeyPressed(TDS_ESCAPE))
			{
				Input::setExitCursor(false);
				Input::releaseTheKey(TDS_ESCAPE);
			}
			else if (!Input::getExitCursor() && isFocus && Input::isMouseButtonPressed(TDS_MOUSE_LEFT))
			{
				Input::setExitCursor(true);
			}
			

			window_size = ImGui::GetWindowSize();
			window_pos = ImGui::GetWindowPos();
			Input::mousePosition globalMousePos = Input::getMousePosition();

			if (Input::getExitCursor() && !GraphicsManager::getInstance().IsViewingFrom2D())
			{

				ImGuiIO& cursor_input = ImGui::GetIO();
				cursor_input.WantSetMousePos = true;
				cursor_input.MousePos = { window_pos.x + (window_size.x * .5f), window_pos.y + (window_size.y * .5f) };
				show_cursor = 0;

				Input::setCenteredMouse(cursor_input.MousePos.x, cursor_input.MousePos.y);
				
				//ImGui::SetMouseCursor(ImGuiMouseCursor_None);

			}
			else
			{
				ImGuiIO& cursor_input = ImGui::GetIO();
				cursor_input.WantSetMousePos = false;
				show_cursor = 1;

				float normalizedLocalMouseX = ((globalMousePos.x - window_pos.x) / (window_size.x * 0.5f)) - 1.f;
				float normalizedLocalMouseY = ((window_pos.y + window_size.y - globalMousePos.y) / (window_size.y * 0.5f)) - 1.f;
				Vec2 localMousePos = { normalizedLocalMouseX, normalizedLocalMouseY };
				
				Input::setLocalMousePos(localMousePos);

			}
			ShowCursor(show_cursor);

			if (ImGui::BeginMenu("Game is Running..."))
			{
				ImGui::EndMenu();
			}
		}
		else
		{
			// Reset values
			CameraSystem::SetIsPlaying(false);
			Input::setExitCursor(true);
			Input::releaseTheKey(TDS_ESCAPE);
			if (ImGui::BeginMenu("Game is Paused"))
			{
				ImGui::EndMenu();
			}
		}

		ImGui::EndMenuBar();
	}
	ImVec2 vSize = ImGui::GetContentRegionAvail();

	ImGui::Image((ImTextureID)m_GamePlayDesc, vSize);
}

void TDS::GamePlayScene::Resize()
{
	if (m_GamePlayDesc)
	{
		/*ImGui_ImplVulkan_RemoveTexture(m_GamePlayDesc);
		m_GamePlayDesc = ImGui_ImplVulkan_AddTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);*/
		ImGui_ImplVulkan_UpdateTexture(GraphicsManager::getInstance().getFinalImage().getSampler(), m_GamePlayDesc, GraphicsManager::getInstance().getFinalImage().getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

