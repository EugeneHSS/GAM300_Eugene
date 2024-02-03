#include "Rendering/UiSystem.h"
#include "components/components.h"
#include "Rendering/Renderer2D.h"
#include "Rendering/FontRenderer.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#define _BATCH
namespace TDS
{
	std::unordered_map<int, bool> UiSystem::m_Layers;
	void UiSystem::Init()
	{
		GraphicsManager::getInstance().ToggleRenderAllLayer(true);
	}
	void UiSystem::ToggleEnableLayer(int layerID, bool condition)
	{
		m_Layers[layerID] = condition;
	}
	void UiSystem::Update(const float dt, const std::vector<EntityID>& entities, Transform* transform, UISprite* _Sprite)
	{
		
		if (_Sprite == nullptr)
			return;

		auto& mgr = GraphicsManager::getInstance();

		auto frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		auto cmdBuffer = GraphicsManager::getInstance().getCommandBuffer();
		FontBatch& Fontbatch = mgr.GetFontRenderer()->GetBatchList();
		SpriteBatch& Spritebatch = mgr.GetRenderer2D()->GetBatchList();
		for (size_t i = 0; i < entities.size(); ++i)
		{
			//UpdatePropertiesFromParent(entities[i]);
			if (_Sprite[i].m_LayerID == -1)
				continue;

			if (GraphicsManager::getInstance().RenderAllLayer() == false)
			{
				int getLayerToRender = GraphicsManager::getInstance().LayerToRender();
				if (getLayerToRender != _Sprite[i].m_LayerID)
					continue;
			}

			if (_Sprite[i].m_EnableSprite == false)
				continue;
			if (!ecs.getEntityIsEnabled(entities[i]) || !ecs.getComponentIsEnabled<UISprite>(entities[i]))
			{
				continue;
			}

			if (_Sprite[i].m_IsFont)
				Fontbatch.AddToBatch(&_Sprite[i], &transform[i], entities[i]);

			else
			{
				Spritebatch.AddToBatch(&_Sprite[i], &transform[i], entities[i]);
				if (transform[i].GetTransformMatrix() != _Sprite->m_TransformMatrix)
				{
					UpdateAABB(&_Sprite[i], &transform[i]);
				}
			}

		}
		//TDS_INFO(Input::getLocalMousePos());
	/*	Spritebatch.PrepareBatch();
		Fontbatch.PrepareBatch();

		Renderer2D::GetInstance()->Update(cmdBuffer, frame);
		Renderer2D::GetInstance()->Draw(cmdBuffer, frame);

		FontRenderer::GetInstance()->Update(cmdBuffer, frame);
		FontRenderer::GetInstance()->Draw(cmdBuffer, frame);

		Spritebatch.Clear();
		Fontbatch.Clear();*/


		/*if (_Sprite->m_IsFont)
		{
			FontBatch& batch = FontRenderer::GetInstance()->GetBatchList();

			for (size_t i = 0; i < entities.size(); ++i)
				batch.AddToBatch(&_Sprite[i], &transform[i]);

			batch.PrepareBatch();
			FontRenderer::GetInstance()->Update(cmdBuffer, frame);
			FontRenderer::GetInstance()->Draw(cmdBuffer, frame);
			batch.Clear();
		}
		else
		{
			SpriteBatch& batch = Renderer2D::GetInstance()->GetBatchList();

			for (size_t i = 0; i < entities.size(); ++i)
				batch.AddToBatch(&_Sprite[i], &transform[i]);

			batch.PrepareBatch();
			Renderer2D::GetInstance()->Update(cmdBuffer, frame);
			Renderer2D::GetInstance()->Draw(cmdBuffer, frame);
			batch.Clear();
		}*/

	}
	void UiSystem::UpdateAABB(UISprite* _Sprite, Transform* _trans) {
		Vec3 scale = _trans->GetScale();
		Vec3 translation = _trans->GetPosition();
		_Sprite->m_TransformMatrix = _trans->GetTransformMatrix();
		std::vector<Vec3> originalBBoxCorners = {
			Vec3(-0.5f, -0.5f, 0.0f),
			Vec3(0.5f, -0.5f, 0.0f),
			Vec3(0.5f, 0.5f, 0.0f),
			Vec3(-0.5f, 0.5f, 0.0f)
		};

		Vec3 newMin = Vec3(FLT_MAX, FLT_MAX, 0.0f);
		Vec3 newMax = Vec3(-FLT_MAX, -FLT_MAX, 0.0f);

		for (const auto& corner : originalBBoxCorners) {
			Vec3 transformedCorner = Vec3(corner.x * scale.x, corner.y * scale.y, corner.z * scale.z) + translation;
			// Update newMin and newMax
			newMin.x = std::min(newMin.x, transformedCorner.x);
			newMin.y = std::min(newMin.y, transformedCorner.y);
			newMax.x = std::max(newMax.x, transformedCorner.x);
			newMax.y = std::max(newMax.y, transformedCorner.y);
		}

		// Update the sprite's AABB
		_Sprite->m_BoundingBoxMin = Vec2(newMin.x, newMin.y);
		_Sprite->m_BoundingBoxMax = Vec2(newMax.x, newMax.y);
	}

	void UiSystem::UpdatePropertiesFromParent(EntityID current)
	{
		auto tag = ecs.getComponent<NameTag>(current);
		if (tag == nullptr)
			return;

		UISprite* sprite = ecs.getComponent<UISprite>(current);

		if (sprite == nullptr) return;

		UpdateDescendantsActiveness(current, sprite->m_EnableSprite);
		
	}

	bool UiSystem::IsDirectChildOfMainParent(EntityID entity)
	{
		auto tag = ecs.getComponent<NameTag>(entity);
		if (tag == nullptr)
			return false;

		// Check if the parent of this entity is the main parent
		EntityID parentID = tag->GetHierarchyParent();
		if (parentID == 0)
			return true; // The entity itself is the main parent

		auto parentTag = ecs.getComponent<NameTag>(parentID);
		return parentTag != nullptr && parentTag->GetHierarchyParent() == 0;
	}

	void UiSystem::UpdateDescendantsActiveness(EntityID parent, bool isActive)
	{
		auto parentTag = ecs.getComponent<NameTag>(parent);
		

		if (parentTag == nullptr)
			return;


		for (auto& child : parentTag->GetHierarchyChildren())
		{
			auto childTag = ecs.getComponent<NameTag>(child);
			UISprite* sprite = ecs.getComponent<UISprite>(parent);

			if (sprite != nullptr)
			{
				sprite->m_EnableSprite = isActive;
			}

			if (childTag != nullptr)
			{
				
				UpdateDescendantsActiveness(child, isActive);
			}
		}
	}

}
