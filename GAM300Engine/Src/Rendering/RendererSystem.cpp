#include "Rendering/RendererSystem.h"
#include "components/components.h"
#include "Rendering/RenderDataManager.h"
#include "Rendering/Renderer3D.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/VulkanPipeline.h"
#include "vulkanTools/vulkanSwapChain.h"
#include "vulkanTools/VulkanTexture.h"
#include "vulkanTools/Renderer.h"
#include "Physics/CollisionSystem.h"
#include "Rendering/Revamped/DeferredController.h"
#include "components/PointLightComponent.h"
#include "components/DirLightComponent.h"
#include "components/SpotLightComponent.h"
namespace TDS
{
#define _OLD
	GlobalUBO ubo{};

	struct PushConstantData {
		Mat4 ModelMat{ 1.f };
		Mat4 NormalMat{ 1.f };
		unsigned int textureIndex = 499;
		float padding[3];
		uint32_t Id{};

	};




	float RendererSystem::lightPosX = 0.f;
	void RendererSystem::OnInit()
	{

	}
	void RendererSystem::OnUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _TransformComponent, GraphicsComponent* _Graphics)
	{
		//Update point lights
		//GraphicsManager::getInstance().m_PointLightRenderer->newupdate(ubo, entities, _TransformComponent, _Graphics);


#ifdef _OLD
		OnRenderDeferred(dt, entities, _TransformComponent, _Graphics);
#else
		OnRenderDeferred(dt, entities, _TransformComponent, _Graphics);
#endif
	}

	void RendererSystem::OnRenderDeferred(const float dt, const std::vector<EntityID>& entities, Transform* _TransComponent, GraphicsComponent* _Graphics)
	{
		std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		VkCommandBuffer commandBuffer = GraphicsManager::getInstance().getCommandBuffer();

		ubo.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();

		ubo.m_Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
		ubo.m_Projection.m[1][1] *= -1;

		auto& graphmgr = GraphicsManager::getInstance();

		//graphmgr.m_PointLightRenderer->newupdate(ubo, entities, _TransComponent, _Graphics);

		auto deferredController = graphmgr.GetDeferredController();
		auto& debugRenderer = graphmgr.GetDebugRenderer();
		deferredController->ClearBatchSubmission();
		deferredController->globalUBO = ubo;
		deferredController->GetSceneUniform().m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
		deferredController->GetSceneUniform().m_Proj = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);

		deferredController->GetSceneUniform().m_Proj.m[1][1] *= -1;

		
		for (size_t i = 0; i < entities.size(); ++i)
		{

			if (Vec3 Scale = _TransComponent[i].GetScale(); Scale.x < FLT_MIN || Scale.y < FLT_MIN || Scale.z < FLT_MIN)
			{

			}
			else
			{
				_TransComponent[i].GenerateTransform();


				
				_TransComponent[i].GenerateFakeTransform();
				

			}

			//if (_Graphics[i].ShowMesh() == false)
			//	continue;
			if (!ecs.getEntityIsEnabled(entities[i]) || !ecs.getComponentIsEnabled<GraphicsComponent>(entities[i]))
			{
				continue;
			}

			if (graphmgr.IsViewingFrom2D())
			{
				if (_Graphics[i].m_UsedIn2D == false)
					continue;
			}

			SpotLightComponent* spotlight = ecs.getComponent<SpotLightComponent>(entities[i]);
			DirectionalLightComponent* dirLight = ecs.getComponent<DirectionalLightComponent>(entities[i]);
			PointLightComponent* pointLight = ecs.getComponent<PointLightComponent>(entities[i]);

			//An entity CANNOT BE BOTH A LIGHT AND A OBJECT
			if (spotlight || dirLight || pointLight)
			{
				if (spotlight)
					deferredController->SubmitSpotLight(entities[i], spotlight, &_TransComponent[i]);


				if (dirLight)
					deferredController->SubmitDirectionalLight(entities[i], dirLight, &_TransComponent[i]);


				if (pointLight)
					deferredController->SubmitPointLight(entities[i], pointLight, &_TransComponent[i]);


			}
			else
			{
				deferredController->SubmitMesh(entities[i], &_Graphics[i], &_TransComponent[i], dt);
				if (CollisionSystem::m_RenderDebugDrawing)
					debugRenderer.GetDebugInstance().SubmitToDebugGroup(&_TransComponent[i], entities[i]);
			}



		}

	}

	//void RendererSystem::OnRender(const float dt, const std::vector<EntityID>& entities, Transform* _TransformComponent, GraphicsComponent* _Graphics)
	//{
	//	std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
	//	VkCommandBuffer commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
	//	GraphicsManager::getInstance().m_PointLightRenderer->newupdate(ubo, entities, _TransformComponent, _Graphics);
	//	//std::cout << ubo.m_activelights << std::endl;
	//	for (size_t i = 0; i < entities.size(); ++i)
	//	{
	//		if (!ecs.getEntityIsEnabled(entities[i]) || !ecs.getComponentIsEnabled<GraphicsComponent>(entities[i]))
	//		{
	//			continue;
	//		}

	//		if (GraphicsManager::getInstance().IsViewingFrom2D())
	//		{
	//			if (_Graphics[i].m_UsedIn2D == false)
	//				continue;
	//		}

	//		PushConstantData pushData{};
	//		pushData.Id = entities[i];
	//		if (_Graphics[i].m_ModelName != _Graphics[i].m_AssetReference.m_AssetName)
	//		{
	//			AssetModel* pModel = AssetManager::GetInstance()->GetModelFactory().GetModel(_Graphics[i].m_ModelName, _Graphics[i].m_AssetReference);

	//			if (pModel == nullptr)
	//			{
	//				//TDS_WARN("No such model called {}", _Graphics[i].m_AssetReference.m_AssetName);
	//			}
	//			else
	//			{
	//				_Graphics[i].m_AssetReference.m_AssetName = _Graphics[i].m_ModelName;
	//				_Graphics[i].m_AssetReference.m_ResourcePtr = pModel;
	//			}

	//		}
	//		std::string texName = _Graphics[i].m_TextureName;
	//		int textureID = AssetManager::GetInstance()->GetTextureFactory().GetTextureIndex(_Graphics[i].m_TextureName, _Graphics[i].m_TextureReference);

	//		if (textureID == -1)
	//		{
	//			pushData.textureIndex = 499;
	//		}
	//		else
	//		{
	//			pushData.textureIndex = textureID;
	//		}

	//		Renderer3D::getPipeline()->SetCommandBuffer(commandBuffer);
	//		GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().SetCommandBuffer(commandBuffer);
	//		GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().SetCommandBuffer(commandBuffer);


	//		if (Renderer3D::getPipeline()->GetCreateEntry().m_EnableDoubleBuffering)
	//		{
	//			if (Vec3 Scale = _TransformComponent[i].GetScale(); Scale.x <= 0.f || Scale.y <= 0.f || Scale.z <= 0.f) {
	//			}
	//			else {
	//				_TransformComponent[i].GenerateTransform();
	//				_TransformComponent[i].GenerateFakeTransform();
	//			}
	//			Mat4 temp;

	//			pushData.ModelMat = temp;
	//			temp.inverse();
	//			temp.transpose();
	//			pushData.NormalMat = temp;
	//			ubo.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
	//			//GraphicsManager::getInstance().m_PointLightRenderer->update(ubo, &_Graphics[i], &_TransformComponent[i]);
	//			ubo.m_Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
	//				GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
	//			ubo.m_Projection.m[1][1] *= -1;

	//			if (_Graphics[i].IsPointLight())
	//			{
	//				GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().BindDescriptor(frame, 1);
	//				GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().UpdateUBO(&ubo, sizeof(GlobalUBO), 1, frame);
	//				GraphicsManager::getInstance().m_PointLightRenderer->render(&_Graphics[i], &_TransformComponent[i]);
	//			}
	//			else {//if not point light render using model
	//				if (_Graphics[i].m_AssetReference.m_ResourcePtr != nullptr)
	//				{

	//					auto& ModelReference = _Graphics[i].m_AssetReference;

	//					//Means we did not split any mesh and the whole model is one mesh
	//					if (ModelReference.m_ResourcePtr->m_Meshes.size() == 1)
	//					{
	//						if (ModelReference.m_ResourcePtr->BufferIsNull())
	//							ModelReference.m_ResourcePtr->CreateBuffers();


	//					}
	//					else
	//					{
	//						MeshData* ptr = nullptr;
	//						auto findItr = ModelReference.m_ResourcePtr->m_Meshes.find(_Graphics[i].m_MeshName);
	//						if (findItr != ModelReference.m_ResourcePtr->m_Meshes.end())
	//						{
	//							ptr = &findItr->second;
	//							ModelReference.m_ResourcePtr->m_CurrMeshData = ptr;
	//							if (ModelReference.m_ResourcePtr->m_CurrMeshData->BufferIsNull())
	//								ModelReference.m_ResourcePtr->m_CurrMeshData->CreateBuffers();
	//						}
	//						else
	//						{
	//							findItr = ModelReference.m_ResourcePtr->m_Meshes.begin();
	//							ptr = &findItr->second;
	//							ModelReference.m_ResourcePtr->m_CurrMeshData = ptr;
	//							if (ModelReference.m_ResourcePtr->m_CurrMeshData->BufferIsNull())
	//								ModelReference.m_ResourcePtr->m_CurrMeshData->CreateBuffers();
	//						}
	//					}



	//					Renderer3D::getPipeline()->BindPipeline();

	//					if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D)
	//					{
	//						Renderer3D::getPipeline()->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
	//						AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D = false;
	//					}



	//					Renderer3D::getPipeline()->UpdateUBO(&ubo, sizeof(GlobalUBO), 0, frame);

	//					Renderer3D::getPipeline()->SubmitPushConstant(&pushData, sizeof(PushConstantData), SHADER_FLAG::VERTEX | SHADER_FLAG::FRAGMENT);
	//					Renderer3D::getPipeline()->BindVertexBuffer(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetVertexBuffer());
	//					Renderer3D::getPipeline()->BindIndexBuffer(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetIndexBuffer());
	//					Renderer3D::getPipeline()->BindDescriptor(frame, 1);
	//					Renderer3D::getPipeline()->BindArrayDescriptorSet(0, 1, 1);
	//					if (_Graphics[i].ShowMesh())
	//					{
	//						Renderer3D::getPipeline()->DrawIndexed(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetVertexBuffer(),
	//							*_Graphics[i].m_AssetReference.m_ResourcePtr->GetIndexBuffer(),
	//							frame);
	//					}
	//					if (_Graphics[i].IsDebugOn()) {
	//						GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().BindDescriptor(frame, 1);
	//						GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().UpdateUBO(&ubo, sizeof(GlobalUBO), 0, frame);
	//						GraphicsManager::getInstance().m_DebugRenderer->Render(&_Graphics[i], &_TransformComponent[i]);
	//						GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().DrawIndexed(*_Graphics[i].m_AssetReference.m_ResourcePtr->GetVertexBuffer(),
	//							*_Graphics[i].m_AssetReference.m_ResourcePtr->GetIndexBuffer(),
	//							frame);
	//					}
	//				}
	//			}

	//		}
	//	}
	//}

	void RendererSystem::onRenderTempFixed(const float dt, const std::vector<EntityID>& entities, Transform* _TransformComponent, GraphicsComponent* _Graphics)
	{
		std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		VkCommandBuffer commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
		GraphicsManager::getInstance().m_PointLightRenderer->newupdate(ubo, entities, _TransformComponent, _Graphics);

		for (size_t i = 0; i < entities.size(); ++i)
		{
			if (!ecs.getEntityIsEnabled(entities[i]) || !ecs.getComponentIsEnabled<GraphicsComponent>(entities[i]))
			{
				continue;
			}

			if (GraphicsManager::getInstance().IsViewingFrom2D())
			{
				if (_Graphics[i].m_UsedIn2D == false)
					continue;
			}

			PushConstantData pushData{};
			pushData.Id = entities[i];



			std::string texName = _Graphics[i].m_TextureName;
			int textureID = AssetManager::GetInstance()->GetTextureFactory().GetTextureIndex(_Graphics[i].m_TextureName, _Graphics[i].m_TextureReference);

			if (textureID == -1)
			{
				pushData.textureIndex = 499;
			}
			else
			{
				pushData.textureIndex = textureID;
			}

			Renderer3D::getTempPipeline()->SetCommandBuffer(commandBuffer);
			GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().SetCommandBuffer(commandBuffer);
			GraphicsManager::getInstance().m_DebugRenderer->GetPipeline().SetCommandBuffer(commandBuffer);

			if (Renderer3D::getTempPipeline()->GetCreateEntry().m_EnableDoubleBuffering)
			{
				MeshController* pModelController = _Graphics[i].m_MeshControllerRef.m_ResourcePtr;
				if (_Graphics[i].m_ModelName != _Graphics[i].m_MeshControllerRef.m_AssetName)
				{
					MeshController* temp = AssetManager::GetInstance()->GetMeshFactory().GetMeshController(_Graphics[i].m_ModelName, _Graphics[i].m_MeshControllerRef);
					if (temp == nullptr)
					{
						temp = _Graphics[i].m_MeshControllerRef.m_ResourcePtr;
						//TDS_WARN("No such model called {}", _Graphics[i].m_AssetReference.m_AssetName);
					}
					else
					{
						_Graphics[i].m_MeshControllerRef.m_AssetName = _Graphics[i].m_ModelName;
						_Graphics[i].m_MeshControllerRef.m_ResourcePtr = temp;
						pModelController = temp;
					}
					pModelController = temp;
				}




				Mat4 temp{};

				if (Vec3 Scale = _TransformComponent[i].GetScale(); Scale.x < 0.0001f || Scale.y < 0.0001f || Scale.z < 0.0001f)
				{

				}
				else {
					_TransformComponent[i].GenerateTransform();
					_TransformComponent[i].GenerateChildFakeTransform();

					if (_TransformComponent[i].GetPosition() == _TransformComponent[i].GetFakePosition() &&
						_TransformComponent[i].GetScale() == _TransformComponent[i].GetFakeScale()
						&& _TransformComponent[i].GetFakeRotation() == _TransformComponent[i].GetFakeRotation())
					{
						temp = _TransformComponent[i].GetTransformMatrix();
					}
					else
					{

						temp = _TransformComponent[i].GetFakeTransform();
					}
				}


				if (_Graphics[i].IsPointLight() == false)
				{//if not point light render using model

					if (pModelController == nullptr)
						continue;



					Mat4 temp{};

					if (Vec3 Scale = _TransformComponent[i].GetScale(); Scale.x < FLT_MIN || Scale.y < FLT_MIN || Scale.z < FLT_MIN)
					{

					}
					else {
						_TransformComponent[i].GenerateTransform();
						_TransformComponent[i].GenerateFakeTransform();

						if (_TransformComponent[i].GetPosition() == _TransformComponent[i].GetFakePosition() &&
							_TransformComponent[i].GetScale() == _TransformComponent[i].GetFakeScale()
							&& _TransformComponent[i].GetFakeRotation() == _TransformComponent[i].GetFakeRotation())
						{
							temp = _TransformComponent[i].GetTransformMatrix();
						}
						else
						{

							temp = _TransformComponent[i].GetFakeTransform();
						}
					}



					auto& sceneNodeContainer = pModelController->GetRoots();
					auto& rootNode = sceneNodeContainer[_Graphics[i].m_MeshNodeName];
					auto& MeshList = sceneNodeContainer[_Graphics[i].m_MeshNodeName].m_MeshList;
					auto sceneNodeItr = MeshList.find(_Graphics[i].m_MeshName);

					if (sceneNodeContainer.size() > 1 && sceneNodeItr == MeshList.end())
						continue;

					pushData.ModelMat = temp;
					temp.inverse();
					temp.transpose();
					pushData.NormalMat = temp;

					ubo.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();

					ubo.m_Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
						GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
					ubo.m_Projection.m[1][1] *= -1;



					if (_Graphics[i].m_MeshControllerRef.m_ResourcePtr != nullptr)
					{

						MeshBuffer* meshBuffer = pModelController->GetMeshBuffer();

						if (meshBuffer == nullptr) continue;

						Renderer3D::getTempPipeline()->BindPipeline();

						if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D)
						{
							Renderer3D::getTempPipeline()->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
							AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D = false;
						}

						Renderer3D::getTempPipeline()->UpdateUBO(&ubo, sizeof(GlobalUBO), 0, frame);

						Renderer3D::getTempPipeline()->SubmitPushConstant(&pushData, sizeof(PushConstantData), SHADER_FLAG::VERTEX | SHADER_FLAG::FRAGMENT);
						Renderer3D::getTempPipeline()->BindVertexBuffer(*meshBuffer->m_VertexBuffer);
						Renderer3D::getTempPipeline()->BindIndexBuffer(*meshBuffer->m_IndexBuffer);
						Renderer3D::getTempPipeline()->BindDescriptor(frame, 1);
						Renderer3D::getTempPipeline()->BindArrayDescriptorSet(0, 1, 1);
						if (_Graphics[i].ShowMesh())
						{
							Renderer3D::getTempPipeline()->DrawIndexed(*meshBuffer->m_VertexBuffer, *meshBuffer->m_IndexBuffer, frame);
						}
						if (_Graphics[i].IsDebugOn())
						{

						}
					}
				}
				else
				{
					GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().BindDescriptor(frame, 1);
					GraphicsManager::getInstance().m_PointLightRenderer->GetPipeline().UpdateUBO(&ubo, sizeof(GlobalUBO), 1, frame);
					GraphicsManager::getInstance().m_PointLightRenderer->render(&_Graphics[i], &_TransformComponent[i]);

				}

			}
		}
	}






}