#include "Rendering/Revamped/DeferredController.h"
#include "components/transform.h"
#include "components/GraphicsComponent.h"
#include "components/DirLightComponent.h"
#include "components/PointLightComponent.h"
#include "components/SpotLightComponent.h"
#include "GraphicsResource/Revamped/MeshController.h"
#include "AssetManagement/AssetManager.h"
#include "AssetManagement/Revamped/MeshFactory.h"
#include "vulkanTools/VulkanPipeline.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "Rendering/Revamped/FrameBuffers/G_Buffer.h"
#include "Rendering/Revamped/FrameBuffers/LightingBuffer.h"
#include "Rendering/Revamped/FrameBuffers/CompositionPass.h"
#include "Rendering/Revamped/FrameBuffers/CubeShadow.h"
#include "Rendering/Renderer2D.h"
#include "Rendering/FontRenderer.h"
#include "Physics/CollisionSystem.h"
#include "components/tag.h"
#include "Rendering/Skybox.h"
#include "Animation/Animation.h"
#include "Rendering/ParticleSystem.h"
#include "Rendering/Revamped/MaterialManager.h"

namespace TDS
{



	void DeferredController::Init(std::uint32_t w, std::uint32_t h)
	{
		CreateFrameBuffers(w, h);
		CreatePipelines();



	}
	void DeferredController::CreatePipelines()
	{
		//Create illuminating light source
		{
			/*m_LightSource = std::make_unique<VulkanPipeline>();

			PipelineCreateEntry PipelineEntry;
			PipelineEntry.m_NumDescriptorSets = 1;
			PipelineEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/LightSourceVert.spv"));
			PipelineEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/LightSourceFrag.spv"));
			PipelineEntry.m_PipelineConfig.m_EnableDepthTest = true;
			PipelineEntry.m_PipelineConfig.m_EnableDepthWrite = true;
			PipelineEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_SRC_ALPHA;
			PipelineEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			PipelineEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_SRC_ALPHA;
			PipelineEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

			BufferInfo Buffer{};
			Buffer.m_Data = m_LightSourceBuffers.data();
			Buffer.m_Size = 1000 * sizeof(LightSourceProperties);
			Buffer.m_Static = false;
			PipelineEntry.m_ShaderInputs.m_InputBuffers[11] = Buffer;
			PipelineEntry.m_FBTarget = m_FrameBuffers[RENDER_PASS::RENDER_LIGTHING];

			m_LightSource->Create(PipelineEntry);*/

		}


		//Create GBuffer pipelines
		{

			//GlobalBufferPool::GetInstance()->AddToGlobalPool(sizeof(GlobalUBO), 0, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, "PL");


			GlobalBufferPool::GetInstance()->AddToGlobalPool(sizeof(MaterialBuffer) * m_MaterialList.size(), 16, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "MaterialBuffer");
			PipelineCreateEntry entry{};
			entry.m_NumDescriptorSets = 1;

			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/BatchShaderVert.spv"));
			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/GBufferFrag.spv"));
			entry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_EnableDepthTest = true;
			entry.m_PipelineConfig.m_EnableDepthWrite = true;
			entry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			entry.m_FBTarget = m_FrameBuffers[RENDER_PASS::RENDER_G_BUFFER];

			VertexLayout layout =
				VertexLayout(
					{
					  VertexBufferElement(VAR_TYPE::VEC3, "vPosition"),
					  VertexBufferElement(VAR_TYPE::VEC3, "vBiTangent"),
					  VertexBufferElement(VAR_TYPE::VEC3, "vTangent"),
					  VertexBufferElement(VAR_TYPE::VEC3, "vNormals"),
					  VertexBufferElement(VAR_TYPE::VEC2, "vUV"),
					  VertexBufferElement(VAR_TYPE::VEC4, "vColor"),
					  VertexBufferElement(VAR_TYPE::VEC4, "BoneIDs"),
					  VertexBufferElement(VAR_TYPE::VEC4, "Weights"),
					  VertexBufferElement(VAR_TYPE::VEC2, "MeshID"),
					});

			entry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(TDSModel::Vertex)));

			BufferInfo Buffer{};
			Buffer.m_Data = m_GBufferBatch3D.m_BatchBuffers.data();
			Buffer.m_Size = MAX_POSSIBLE_BATCH * sizeof(BatchData);
			Buffer.m_Static = false;
			entry.m_ShaderInputs.m_InputBuffers[15] = Buffer;

			//BufferInfo Buffer1{};
			//Buffer1.m_Data = m_GBufferBatch3D.m_MaterialList.data();
			//Buffer1.m_Size = MAX_POSSIBLE_BATCH * sizeof(MaterialBuffer);
			//Buffer1.m_Static = false;
			//entry.m_ShaderInputs.m_InputBuffers[16] = Buffer1;
			//GlobalBufferPool::GetInstance()->AddToGlobalPool(sizeof(GlobalUBO), 5, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, "PL");
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH] = std::make_shared<VulkanPipeline>();
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH]->Create(entry);

			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH]->UpdateUBO(m_MaterialList.data(), m_MaterialList.size() * sizeof(MaterialBuffer), 16, 0);
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH]->UpdateUBO(m_MaterialList.data(), m_MaterialList.size() * sizeof(MaterialBuffer), 16, 1);

			PipelineCreateEntry entry2{};


			entry2.m_NumDescriptorSets = 1;
			entry2.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/InstanceShaderVert.spv"));
			entry2.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/GBufferFrag.spv"));
			entry2.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			entry2.m_PipelineConfig.m_EnableDepthTest = true;
			entry2.m_PipelineConfig.m_EnableDepthWrite = true;
			entry2.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(TDSModel::Vertex)));
			entry2.m_FBTarget = m_FrameBuffers[RENDER_PASS::RENDER_G_BUFFER];
			BufferInfo Buffer2{};
			Buffer2.m_Data = m_GBufferInstance.m_InstanceBuffers.data();
			Buffer2.m_Size = MAX_POSSIBLE_BATCH * sizeof(BatchData);
			Buffer2.m_Static = false;
			entry2.m_ShaderInputs.m_InputBuffers[15] = Buffer2;

			BufferInfo Buffer3{};
			Buffer3.m_Data = m_Bones.data();
			Buffer3.m_Size = MAX_BONES * sizeof(Mat4);
			Buffer3.m_Static = false;
			entry2.m_ShaderInputs.m_InputBuffers[19] = Buffer3;

			//BufferInfo Buffer4{};
			//Buffer1.m_Data = m_GBufferInstance.m_MaterialList.data();
			//Buffer1.m_Size = MAX_INSTANCE_BUFFER * sizeof(MaterialBuffer);
			//Buffer1.m_Static = false;
			//entry.m_ShaderInputs.m_InputBuffers[16] = Buffer4;

			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_INSTANCE] = std::make_shared<VulkanPipeline>();
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_INSTANCE]->Create(entry2);


		}

		//Depth pass or shadow pass
		{


		}


		//Creating lighting pass
		{

			PipelineCreateEntry entry{};

			entry.m_NumDescriptorSets = 1;
			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/DeferredVert.spv"));
			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/Lighting.spv"));
			entry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			entry.m_FBTarget = m_FrameBuffers[RENDER_PASS::RENDER_LIGTHING];

			BufferInfo Buffer{};
			Buffer.m_Data = m_LightUBOs.m_PointLights.data();
			Buffer.m_Size = MAX_POINT_LIGHTS * sizeof(Pointlight);
			Buffer.m_Static = false;
			entry.m_ShaderInputs.m_InputBuffers[21] = Buffer;

			BufferInfo Buffer2{};
			Buffer2.m_Data = m_LightUBOs.m_DirectionalLight.data();
			Buffer2.m_Size = MAX_DIR_LIGHTS * sizeof(DirectionalLight);
			Buffer2.m_Static = false;
			entry.m_ShaderInputs.m_InputBuffers[22] = Buffer2;

			BufferInfo Buffer3{};
			Buffer3.m_Data = m_LightUBOs.m_SpotLights.data();
			Buffer3.m_Size = MAX_SPOT_LIGHTS * sizeof(SpotLight);
			Buffer3.m_Static = false;
			entry.m_ShaderInputs.m_InputBuffers[23] = Buffer3;

			m_DeferredPipelines[DEFERRED_STAGE::STAGE_LIGTHING] = std::make_shared<VulkanPipeline>();
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_LIGTHING]->Create(entry);

		}


		//Post processing?
		{


		}

		//Create Composition pass
		{
			PipelineCreateEntry entry{};

			entry.m_NumDescriptorSets = 1;
			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/DeferredVert.spv"));
			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/Composition.spv"));
			entry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			entry.m_FBTarget = m_FrameBuffers[RENDER_PASS::RENDER_COMPOSITION];

			m_DeferredPipelines[DEFERRED_STAGE::STAGE_COMPOSITION] = std::make_shared<VulkanPipeline>();
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_COMPOSITION]->Create(entry);
		}

		//For clean 3D objects in UI pass with no lighting applied
		{
			PipelineCreateEntry entry{};
			entry.m_NumDescriptorSets = 1;

			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/BatchShaderVert.spv"));
			entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/Composition3D.spv"));
			entry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_EnableDepthTest = true;
			entry.m_PipelineConfig.m_EnableDepthWrite = true;
			entry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry.m_PipelineConfig.m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			entry.m_FBTarget = m_FrameBuffers[RENDER_PASS::RENDER_COMPOSITION];

			VertexLayout layout =
				VertexLayout(
					{
					  VertexBufferElement(VAR_TYPE::VEC3, "vPosition"),
					  VertexBufferElement(VAR_TYPE::VEC3, "vBiTangent"),
					  VertexBufferElement(VAR_TYPE::VEC3, "vTangent"),
					  VertexBufferElement(VAR_TYPE::VEC3, "vNormals"),
					  VertexBufferElement(VAR_TYPE::VEC2, "vUV"),
					  VertexBufferElement(VAR_TYPE::VEC4, "vColor"),
					  VertexBufferElement(VAR_TYPE::VEC4, "BoneIDs"),
					  VertexBufferElement(VAR_TYPE::VEC4, "Weights"),
					  VertexBufferElement(VAR_TYPE::VEC2, "MeshID"),
					});

			entry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(TDSModel::Vertex)));

			BufferInfo Buffer{};
			Buffer.m_Data = m_Composition3DBatch.m_BatchBuffers.data();
			Buffer.m_Size = MAX_POSSIBLE_BATCH * sizeof(BatchData);
			Buffer.m_Static = false;
			entry.m_ShaderInputs.m_InputBuffers[15] = Buffer;

			//BufferInfo Buffer1{};
			//Buffer1.m_Data = m_Composition3DBatch.m_MaterialList.data();
			//Buffer1.m_Size = MAX_POSSIBLE_BATCH * sizeof(MaterialBuffer);
			//Buffer1.m_Static = false;
			//entry.m_ShaderInputs.m_InputBuffers[16] = Buffer1;

			m_DeferredPipelines[DEFERRED_STAGE::STAGE_3D_COMPOSITION_BATCH] = std::make_shared<VulkanPipeline>();
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_3D_COMPOSITION_BATCH]->Create(entry);

			PipelineCreateEntry entry2{};


			entry2.m_NumDescriptorSets = 1;
			entry2.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/InstanceShaderVert.spv"));
			entry2.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/Composition3D.spv"));
			entry2.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
			entry2.m_PipelineConfig.m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
			entry2.m_PipelineConfig.m_EnableDepthTest = true;
			entry2.m_PipelineConfig.m_EnableDepthWrite = true;
			entry2.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(TDSModel::Vertex)));
			entry2.m_FBTarget = m_FrameBuffers[RENDER_PASS::RENDER_COMPOSITION];
			BufferInfo Buffer2{};
			Buffer2.m_Data = m_Composition3DInstance.m_InstanceBuffers.data();
			Buffer2.m_Size = MAX_POSSIBLE_BATCH * sizeof(BatchData);
			Buffer2.m_Static = false;
			entry2.m_ShaderInputs.m_InputBuffers[15] = Buffer2;

			BufferInfo Buffer3{};
			Buffer3.m_Data = m_Bones.data();
			Buffer3.m_Size = MAX_BONES * sizeof(Mat4);
			Buffer3.m_Static = false;
			entry2.m_ShaderInputs.m_InputBuffers[19] = Buffer3;


			//BufferInfo Buffer4{};
			//Buffer4.m_Data = m_Composition3DInstance.m_MaterialList.data();
			//Buffer4.m_Size = MAX_INSTANCE_BUFFER * sizeof(MaterialBuffer);
			//Buffer4.m_Static = false;
			//entry2.m_ShaderInputs.m_InputBuffers[16] = Buffer4;

			m_DeferredPipelines[DEFERRED_STAGE::STAGE_3D_COMPOSITION_INSTANCE] = std::make_shared<VulkanPipeline>();
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_3D_COMPOSITION_INSTANCE]->Create(entry2);


		}


		UpdateDeferredTextures();
	}
	void DeferredController::G_BufferPassBatch(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{
		auto& textureMgr = AssetManager::GetInstance()->GetTextureFactory();

		auto GBufferPipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH];
		GBufferPipeline->SetCommandBuffer(commandBuffer);
		for (auto& meshItr : m_GBufferBatch3D.m_BatchUpdateInfo)
		{
			for (auto& updates : meshItr.second.m_MeshUpdates)
			{


				Mat4 temp{};
				if (updates.m_pTransform->GetPosition() == updates.m_pTransform->GetFakePosition() &&
					updates.m_pTransform->GetScale() == updates.m_pTransform->GetFakeScale()
					&& updates.m_pTransform->GetRotation() == updates.m_pTransform->GetFakeRotation())
				{
					temp = updates.m_pTransform->GetTransformMatrix();
				}
				else
				{

					temp = updates.m_pTransform->GetFakeTransform();
				}


				m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_EntityID = updates.m_EntityID;
				m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_IsRender = updates.m_ShowMesh;
				m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_modelMatrix = temp;
				m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_TextureID = updates.m_TextureID;
				m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_UseMeshMatID = updates.m_UsePreloadedMaterials;
				m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_UseMaterials = updates.m_UseMaterials;
				m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_MaterialID = updates.m_MaterialID;

				auto& materialBuffer = m_GBufferBatch3D.m_BatchBuffers[updates.m_MeshID].m_ComponentMaterial;
				//Materials
				{
					materialBuffer.diffuseTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_DiffuseTex);
					materialBuffer.specularTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_SpecularTex);
					materialBuffer.ambientTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_AmbientTex);
					materialBuffer.emissiveTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_EmissiveTex);
					materialBuffer.shininessTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_ShininessTex);
					materialBuffer.diffuse = updates.m_pMaterialAttribute->m_phongBlinn.m_DiffuseColor;
					materialBuffer.specular = updates.m_pMaterialAttribute->m_phongBlinn.m_SpecularColor;
					materialBuffer.emissive = updates.m_pMaterialAttribute->m_phongBlinn.m_EmissiveColor;
					materialBuffer.ambient = updates.m_pMaterialAttribute->m_phongBlinn.m_AmbientColor;
					materialBuffer.shininess = updates.m_pMaterialAttribute->m_phongBlinn.m_Shininess;
					materialBuffer.shininessStrength = updates.m_pMaterialAttribute->m_phongBlinn.m_ShininessStrength;
					materialBuffer.reflectivity = updates.m_pMaterialAttribute->m_Reflectivity;

					materialBuffer.reflectance = updates.m_pMaterialAttribute->m_Reflectance;
					materialBuffer.reflectanceTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_ReflectanceTexture);

					//NO PBR for now

					//materialBuffer.normalTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_PBRAttributes.m_NormalTex);
					materialBuffer.MaterialID = 0;
					materialBuffer.ShadingModel = updates.m_pMaterialAttribute->m_shading;
					materialBuffer.UseMaterialTextures = updates.m_UseMaterials;
				}

			}
			meshItr.second.m_MeshUpdates.clear();
		}

		GBufferPipeline->UpdateUBO(&m_SceneUBO, sizeof(SceneUniform), 5, frameIndex);
		std::uint32_t batchCnt = AssetManager::GetInstance()->GetMeshFactory().GetBatchCount();
		GBufferPipeline->UpdateUBO(m_GBufferBatch3D.m_BatchBuffers.data(), sizeof(BatchData) * batchCnt, 15, frameIndex);

		//if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayBatch)
		//{
		//	GBufferPipeline->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
		//	AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayBatch = false;
		//}

		for (auto& [meshName, meshUpdate] : m_GBufferBatch3D.m_BatchUpdateInfo)
		{
			if (meshUpdate.m_MeshBuffer == nullptr) continue;

			GBufferPipeline->BindPipeline();

			GBufferPipeline->BindVertexBuffer(*meshUpdate.m_MeshBuffer->m_VertexBuffer);
			GBufferPipeline->BindIndexBuffer(*meshUpdate.m_MeshBuffer->m_IndexBuffer);
			GBufferPipeline->BindDescriptor(frameIndex, 1);
			GBufferPipeline->BindArrayDescriptorSet(0, 1, 1);

			//GBufferPipeline->DrawIndexed(*meshUpdate.m_MeshBuffer->m_VertexBuffer, *meshUpdate.m_MeshBuffer->m_IndexBuffer, frameIndex);
			vkCmdDrawIndexedIndirect(commandBuffer, meshUpdate.m_MeshBuffer->m_IndirectBuffer->GetBuffer(), 0, meshUpdate.m_MeshBuffer->m_IndirectBuffer->getDataCount(), sizeof(VkDrawIndexedIndirectCommand));

		}
		m_GBufferBatch3D.m_BatchUpdateInfo.clear();

	}
	void DeferredController::G_BufferInstanced(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{

		auto& textureMgr = AssetManager::GetInstance()->GetTextureFactory();

		auto GBufferPipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_INSTANCE];
		unsigned int startingOffset = 0;
		unsigned int totalAnimationOffset = 0;
		bool useMaterials = false;
		for (auto& itr : m_GBufferInstance.m_instanceRenderManager.m_InstanceUpdateInfo)
		{
			auto& instanceReq = m_GBufferInstance.m_InstanceRequests[m_GBufferInstance.m_GroupIdx];
			instanceReq.m_MeshBuffer = itr.first;
			std::string ModelName{};
			for (std::uint32_t i = 0; i < itr.second.m_Index; ++i)
			{
				auto& meshUpdateData = itr.second.m_Updates[i];

				auto& InstanceInfo = instanceReq.m_RenderInstanceInfo;
				auto& instaneBuffer = m_GBufferInstance.m_InstanceBuffers[m_GBufferInstance.m_TotalInstances];
				auto& materialBuffer = instaneBuffer.m_ComponentMaterial;

				InstanceInfo.m_InstanceOffset = startingOffset;
				InstanceInfo.m_Instances = itr.second.m_Index;

				instaneBuffer.m_AnimOffset = totalAnimationOffset;
				instaneBuffer.m_MaterialID = meshUpdateData.m_MaterialID;
				instaneBuffer.m_IsRender = meshUpdateData.m_ShowMesh;
				instaneBuffer.m_TextureID = meshUpdateData.m_TextureID;
				instaneBuffer.m_EntityID = meshUpdateData.m_EntityID;
				instaneBuffer.m_modelMatrix = meshUpdateData.m_pTransform->GetTransformMatrix();
				instaneBuffer.m_IsAnimated = meshUpdateData.m_IsAnimated;
				instaneBuffer.m_UseMaterials = useMaterials = meshUpdateData.m_UseMaterials;
				instaneBuffer.m_UseMeshMatID = meshUpdateData.m_UsePreloadedMaterials;
				ModelName = meshUpdateData.m_ModelName;
				{

					materialBuffer.diffuseTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_DiffuseTex);
					materialBuffer.specularTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_SpecularTex);
					materialBuffer.ambientTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_AmbientTex);
					materialBuffer.emissiveTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_EmissiveTex);
					materialBuffer.shininessTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_ShininessTex);
					materialBuffer.diffuse = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_DiffuseColor;
					materialBuffer.specular = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_SpecularColor;
					materialBuffer.emissive = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_EmissiveColor;
					materialBuffer.ambient = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_AmbientColor;
					materialBuffer.shininess = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_Shininess;
					materialBuffer.shininessStrength = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_ShininessStrength;
					materialBuffer.reflectivity = meshUpdateData.m_pMaterialAttribute->m_Reflectivity;


					materialBuffer.reflectance = meshUpdateData.m_pMaterialAttribute->m_Reflectance;
					materialBuffer.reflectanceTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_ReflectanceTexture);

					//NO PBR for now

					//materialBuffer.normalTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_PBRAttributes.m_NormalTex);
					materialBuffer.MaterialID = 0;
					materialBuffer.ShadingModel = meshUpdateData.m_pMaterialAttribute->m_shading;
					materialBuffer.UseMaterialTextures = meshUpdateData.m_UseMaterials;


				}



				if (meshUpdateData.m_IsAnimated)
				{
					if (meshUpdateData.m_pAnimationPlayer == nullptr)
					{
						meshUpdateData.m_pAnimationPlayer = &ecs.getComponent<AnimationComponent>(meshUpdateData.m_EntityID)->m_AnimationPlayer;
					}

					auto bones = meshUpdateData.m_pAnimationPlayer->getCurrentBonesMatrices();
					for (size_t i = 0; i < bones->size() && i < MAX_BONES_PER_MESH; ++i)
					{
						m_Bones[totalAnimationOffset] = bones->at(i);
						totalAnimationOffset++;
					}


				}



				++m_GBufferInstance.m_TotalInstances;

			}
			instanceReq.m_ModelName = ModelName;
			startingOffset = m_GBufferInstance.m_TotalInstances;
			itr.second.m_Index = 0;
			++m_GBufferInstance.m_GroupIdx;
		}


		GBufferPipeline->UpdateUBO(&m_SceneUBO, sizeof(SceneUniform), 5, frameIndex);

		GBufferPipeline->UpdateUBO(m_Bones.data(), sizeof(Mat4) * totalAnimationOffset, 19, frameIndex);

		GBufferPipeline->UpdateUBO(m_GBufferInstance.m_InstanceBuffers.data(), sizeof(BatchData) * m_GBufferInstance.m_TotalInstances, 15, frameIndex);

		//if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayInstance)
		//{
		//	GBufferPipeline->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
		//	AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayInstance = false;
		//}

		GBufferPipeline->SetCommandBuffer(commandBuffer);
		for (std::uint32_t i = 0; i < m_GBufferInstance.m_GroupIdx; ++i)
		{
			auto& instanceReq = m_GBufferInstance.m_InstanceRequests[i];
			
			if (useMaterials)
				UploadMaterialsList(instanceReq.m_ModelName);

			GBufferPipeline->BindPipeline();
			GBufferPipeline->BindVertexBuffer(*instanceReq.m_MeshBuffer->m_VertexBuffer);
			GBufferPipeline->BindIndexBuffer(*instanceReq.m_MeshBuffer->m_IndexBuffer);
			GBufferPipeline->BindDescriptor(frameIndex, 1);
			GBufferPipeline->BindArrayDescriptorSet(0, 1, 1);
			GBufferPipeline->SubmitPushConstant(&instanceReq.m_RenderInstanceInfo.m_InstanceOffset, sizeof(std::uint32_t), SHADER_FLAG::VERTEX);
			GBufferPipeline->DrawInstancedIndexed(*instanceReq.m_MeshBuffer->m_VertexBuffer, *instanceReq.m_MeshBuffer->m_IndexBuffer, instanceReq.m_RenderInstanceInfo.m_Instances, frameIndex);

			instanceReq.m_RenderInstanceInfo.m_InstanceOffset = 0;
			instanceReq.m_RenderInstanceInfo.m_Instances = 0;
		}

		m_GBufferInstance.m_TotalInstances = 0;
		m_GBufferInstance.m_GroupIdx = 0;

	}
	void DeferredController::ClearBatchSubmission()
	{
		m_GBufferInstance.m_GroupIdx = 0;
		m_GBufferInstance.m_TotalInstances = 0;

	}
	void DeferredController::SubmitMesh(std::uint32_t entityID, GraphicsComponent* graphComp, Transform* transformComp, float _dt)
	{
		Mat4 temp{};


		if (transformComp->GetPosition() == transformComp->GetFakePosition() &&
			transformComp->GetScale() == transformComp->GetFakeScale()
			&& transformComp->GetRotation() == transformComp->GetFakeRotation())
		{
			temp = transformComp->GetTransformMatrix();
		}
		else
		{

			temp = transformComp->GetFakeTransform();
		}


		std::string texName = graphComp->m_TextureName;

		MeshController* pModelController = graphComp->m_MeshControllerRef.m_ResourcePtr;

		if (graphComp->m_ModelName != graphComp->m_MeshControllerRef.m_AssetName)
		{

			std::string newMeshName = "";
			MeshController* temp = AssetManager::GetInstance()->GetMeshFactory().GetMeshController(graphComp->m_ModelName, graphComp->m_MeshControllerRef);
			if (temp == nullptr)
			{
				temp = graphComp->m_MeshControllerRef.m_ResourcePtr;

			}
			else
			{
				graphComp->m_MeshControllerRef.m_AssetName = graphComp->m_ModelName;
				graphComp->m_MeshControllerRef.m_ResourcePtr = temp;
			}

			pModelController = temp;
		}


		if (pModelController == nullptr) return;

		auto entityToNodeItr = pModelController->m_EntityToNodeName.find(entityID);
		if (entityToNodeItr == pModelController->m_EntityToNodeName.end())
			pModelController->m_EntityToNodeName[entityID] = graphComp->m_MeshNodeName;

		int textureID = AssetManager::GetInstance()->GetTextureFactory().GetTextureIndex(graphComp->m_TextureName, graphComp->m_TextureReference);

		if (textureID == -1)
			textureID = 999;

		if (graphComp->m_UsedIn2D == false)
		{

			if (pModelController->m_Instancing == false)
			{
				SubmitBatch(entityID, textureID, transformComp, graphComp);
			}
			else
			{
				SubmitInstance(entityID, textureID, transformComp, graphComp);
			}
		}
		else
		{
			SubmitMeshForUI(entityID, textureID, graphComp, transformComp);
		}




	}
	void DeferredController::SubmitBatch(std::uint32_t entityID, int TextureID, Transform* transformComp, GraphicsComponent* graphComp)
	{


		MeshController* pMeshController = graphComp->m_MeshControllerRef.m_ResourcePtr;
		MeshBuffer* meshBuffer = pMeshController->GetMeshBuffer();

		//Check if batch VBO and EBO exist
		if (meshBuffer == nullptr) return;


		//Check if model is referencing a mesh
		int meshID = pMeshController->GetMeshID(graphComp->m_MeshName);

		if (meshID == -1) return;



		if (graphComp->m_UsePreloadMaterials && graphComp->m_UseMaterials)
		{
			auto& materialMgr = GraphicsManager::getInstance().GetMaterialManager();
			MaterialData* matdata = materialMgr.GetMaterialData(graphComp->GetModelName());
			if (matdata != nullptr)
			{
				int materialID = pMeshController->GetRoots().at(graphComp->m_MeshNodeName).m_MeshList[graphComp->m_MeshName].m_MaterialID;

				auto findItr = matdata->m_materialAttributes.find(materialID);

				if (findItr != matdata->m_materialAttributes.end())
				{
					graphComp->m_MaterialAttributes = findItr->second;
				}
			}
		}



		m_GBufferBatch3D.m_BatchUpdateInfo[graphComp->m_ModelName].m_MeshBuffer = pMeshController->GetMeshBuffer();
		auto& batchUpdate = m_GBufferBatch3D.m_BatchUpdateInfo[graphComp->m_ModelName].m_MeshUpdates.emplace_back();

		batchUpdate.m_EntityID = entityID;
		batchUpdate.m_pTransform = transformComp;
		batchUpdate.m_TextureID = TextureID;
		batchUpdate.m_IsAnimated = false;
		batchUpdate.m_MeshID = meshID;
		batchUpdate.m_ShowMesh = graphComp->ShowMesh();
		batchUpdate.m_RenderIn2D = graphComp->m_UsedIn2D;
		batchUpdate.m_pMaterialAttribute = &graphComp->m_MaterialAttributes;
		batchUpdate.m_UseMaterials = graphComp->m_UseMaterials;
		batchUpdate.m_UsePreloadedMaterials = graphComp->m_UsePreloadMaterials;
		batchUpdate.m_ModelName = graphComp->m_ModelName;
	}

	void DeferredController::SubmitMeshForUI(std::uint32_t entityID, int TextureID, GraphicsComponent* graphComp, Transform* transformComp)
	{
		auto pMeshController = graphComp->m_MeshControllerRef;

		if (pMeshController.m_ResourcePtr->m_Instancing == false)
		{
			MeshController* pMeshController = graphComp->m_MeshControllerRef.m_ResourcePtr;
			MeshBuffer* meshBuffer = pMeshController->GetMeshBuffer();

			if (meshBuffer == nullptr) return;

			int meshID = pMeshController->GetMeshID(graphComp->m_MeshName);

			if (meshID == -1) return;

			if (graphComp->m_UsePreloadMaterials && graphComp->m_UseMaterials)
			{
				auto& materialMgr = GraphicsManager::getInstance().GetMaterialManager();
				MaterialData* matdata = materialMgr.GetMaterialData(graphComp->GetModelName());
				if (matdata != nullptr)
				{
					int materialID = pMeshController->GetRoots().at(graphComp->m_MeshNodeName).m_MeshList[graphComp->m_MeshName].m_MaterialID;

					auto findItr = matdata->m_materialAttributes.find(materialID);

					if (findItr != matdata->m_materialAttributes.end())
					{
						graphComp->m_MaterialAttributes = findItr->second;
					}
				}
			}

			m_Composition3DBatch.m_BatchUpdateInfo[graphComp->m_ModelName].m_MeshBuffer = pMeshController->GetMeshBuffer();
			auto& batchUpdate = m_Composition3DBatch.m_BatchUpdateInfo[graphComp->m_ModelName].m_MeshUpdates.emplace_back();

			batchUpdate.m_EntityID = entityID;
			batchUpdate.m_pTransform = transformComp;
			batchUpdate.m_TextureID = TextureID;
			batchUpdate.m_IsAnimated = false;
			batchUpdate.m_MeshID = meshID;
			batchUpdate.m_ShowMesh = graphComp->ShowMesh();
			batchUpdate.m_RenderIn2D = graphComp->m_UsedIn2D;
			batchUpdate.m_pMaterialAttribute = &graphComp->m_MaterialAttributes;
			batchUpdate.m_UseMaterials = graphComp->m_UseMaterials;
			batchUpdate.m_UsePreloadedMaterials = graphComp->m_UsePreloadMaterials;
			batchUpdate.m_ModelName = graphComp->m_ModelName;
		}
		else
		{
			MeshController* pMeshController = graphComp->m_MeshControllerRef.m_ResourcePtr;
			MeshBuffer* meshBuffer = pMeshController->GetMeshBuffer();

			if (m_Composition3DInstance.m_GroupIdx >= MAX_INSTANCE_BUFFER)
			{
				TDS_ERROR("We have reached the maximum amount of instances!");
				return;
			}

			auto& instanceUpdatePack = m_Composition3DInstance.m_instanceRenderManager.m_InstanceUpdateInfo[meshBuffer];


			MeshUpdate* updateData = (instanceUpdatePack.m_Index == instanceUpdatePack.m_Updates.size()) ?
				&instanceUpdatePack.m_Updates.emplace_back() : &instanceUpdatePack.m_Updates[instanceUpdatePack.m_Index];


			updateData->m_pTransform = transformComp;
			updateData->m_IsAnimated = false;
			updateData->m_TextureID = TextureID;
			updateData->m_EntityID = entityID;
			updateData->m_MeshID = 0;
			updateData->m_ShowMesh = graphComp->ShowMesh();
			updateData->m_RenderIn2D = graphComp->m_UsedIn2D;
			updateData->m_pMaterialAttribute = &graphComp->m_MaterialAttributes;
			updateData->m_UseMaterials = graphComp->m_UseMaterials;
			updateData->m_UsePreloadedMaterials = graphComp->m_UsePreloadMaterials;
			updateData->m_ModelName = graphComp->m_ModelName;
			++instanceUpdatePack.m_Index;
		}
	}

	void DeferredController::RenderUISceneMeshBatch(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{
		auto& textureMgr = AssetManager::GetInstance()->GetTextureFactory();
		if (GraphicsManager::getInstance().IsViewingFrom2D() == false) return;
		auto GBufferPipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_3D_COMPOSITION_BATCH];
		GBufferPipeline->SetCommandBuffer(commandBuffer);
		for (auto& meshItr : m_Composition3DBatch.m_BatchUpdateInfo)
		{
			for (auto& updates : meshItr.second.m_MeshUpdates)
			{
				m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_EntityID = updates.m_EntityID;
				m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_IsRender = updates.m_ShowMesh;
				m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_modelMatrix = updates.m_pTransform->GetFakeTransform();
				m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_TextureID = updates.m_TextureID;
				m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_MaterialID = updates.m_MaterialID;
				m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_UseMaterials = updates.m_UseMaterials;
				m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_UseMeshMatID = updates.m_UsePreloadedMaterials;

				auto& materialBuffer = m_Composition3DBatch.m_BatchBuffers[updates.m_MeshID].m_ComponentMaterial;
				//Materials
				{
					materialBuffer.diffuseTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_DiffuseTex);
					materialBuffer.specularTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_SpecularTex);
					materialBuffer.ambientTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_AmbientTex);
					materialBuffer.emissiveTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_EmissiveTex);
					materialBuffer.shininessTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_phongBlinn.m_ShininessTex);
					materialBuffer.diffuse = updates.m_pMaterialAttribute->m_phongBlinn.m_DiffuseColor;
					materialBuffer.specular = updates.m_pMaterialAttribute->m_phongBlinn.m_SpecularColor;
					materialBuffer.emissive = updates.m_pMaterialAttribute->m_phongBlinn.m_EmissiveColor;
					materialBuffer.ambient = updates.m_pMaterialAttribute->m_phongBlinn.m_AmbientColor;
					materialBuffer.shininess = updates.m_pMaterialAttribute->m_phongBlinn.m_Shininess;
					materialBuffer.shininessStrength = updates.m_pMaterialAttribute->m_phongBlinn.m_ShininessStrength;
					materialBuffer.reflectivity = updates.m_pMaterialAttribute->m_Reflectivity;

					materialBuffer.reflectance = updates.m_pMaterialAttribute->m_Reflectance;
					materialBuffer.reflectanceTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_ReflectanceTexture);

					//NO PBR for now

					//materialBuffer.normalTex = textureMgr.GetTextureID(updates.m_pMaterialAttribute->m_PBRAttributes.m_NormalTex);
					materialBuffer.MaterialID = 0;
					materialBuffer.ShadingModel = updates.m_pMaterialAttribute->m_shading;
					materialBuffer.UseMaterialTextures = updates.m_UseMaterials;
				}
			}
			meshItr.second.m_MeshUpdates.clear();
		}




		GBufferPipeline->UpdateUBO(&m_SceneUBO, sizeof(SceneUniform), 5, frameIndex);
		GBufferPipeline->UpdateUBO(m_Composition3DBatch.m_BatchBuffers.data(), sizeof(BatchData) * m_Composition3DBatch.m_BatchBuffers.size(), 15, frameIndex);

		//if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayBatch)
		//{
		//	GBufferPipeline->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
		//	AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayBatch = false;
		//}

		for (auto& [meshName, meshUpdate] : m_Composition3DBatch.m_BatchUpdateInfo)
		{
			GBufferPipeline->BindPipeline();
			GBufferPipeline->BindVertexBuffer(*meshUpdate.m_MeshBuffer->m_VertexBuffer);
			GBufferPipeline->BindIndexBuffer(*meshUpdate.m_MeshBuffer->m_IndexBuffer);
			GBufferPipeline->BindDescriptor(frameIndex, 1);
			GBufferPipeline->BindArrayDescriptorSet(0, 1, 1);

			GBufferPipeline->DrawIndexed(*meshUpdate.m_MeshBuffer->m_VertexBuffer, *meshUpdate.m_MeshBuffer->m_IndexBuffer, frameIndex);
		}
		m_Composition3DBatch.m_BatchUpdateInfo.clear();
	}
	void DeferredController::SetFadeFactor(float fadeValue)
	{
		m_ScreenFadeFactor = fadeValue;
	}
	float DeferredController::GetFadeFactor()
	{
		return m_ScreenFadeFactor;
	}
	void DeferredController::RenderUISceneMeshInstance(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{
		if (GraphicsManager::getInstance().IsViewingFrom2D() == false) return;

		auto& textureMgr = AssetManager::GetInstance()->GetTextureFactory();
		bool useMaterials = false;

		auto GBufferPipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_3D_COMPOSITION_INSTANCE];
		int startingOffset = 0;
		for (auto& itr : m_Composition3DInstance.m_instanceRenderManager.m_InstanceUpdateInfo)
		{
			std::string ModelName{};
			auto& instanceReq = m_Composition3DInstance.m_InstanceRequests[m_Composition3DInstance.m_GroupIdx];
			instanceReq.m_MeshBuffer = itr.first;

			for (std::uint32_t i = 0; i < itr.second.m_Index; ++i)
			{
				auto& meshUpdateData = itr.second.m_Updates[i];
				ModelName = meshUpdateData.m_ModelName;
				auto& InstanceInfo = instanceReq.m_RenderInstanceInfo;
				auto& instaneBuffer = m_Composition3DInstance.m_InstanceBuffers[m_Composition3DInstance.m_TotalInstances];

				InstanceInfo.m_InstanceOffset = startingOffset;
				InstanceInfo.m_Instances = itr.second.m_Index;
				

				{
					instaneBuffer.m_IsRender = meshUpdateData.m_ShowMesh;
					instaneBuffer.m_TextureID = meshUpdateData.m_TextureID;
					instaneBuffer.m_EntityID = meshUpdateData.m_EntityID;
					instaneBuffer.m_modelMatrix = meshUpdateData.m_pTransform->GetTransformMatrix();
					instaneBuffer.m_UseMaterials = useMaterials = meshUpdateData.m_UseMaterials;
					instaneBuffer.m_UseMeshMatID = meshUpdateData.m_UsePreloadedMaterials;
					instaneBuffer.m_MaterialID = meshUpdateData.m_MaterialID;
					
				}


				auto& materialBuffer = instaneBuffer.m_ComponentMaterial;
				//Materials
				{
					materialBuffer.diffuseTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_DiffuseTex);
					materialBuffer.specularTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_SpecularTex);
					materialBuffer.ambientTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_AmbientTex);
					materialBuffer.emissiveTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_EmissiveTex);
					materialBuffer.shininessTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_ShininessTex);
					materialBuffer.diffuse = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_DiffuseColor;
					materialBuffer.specular = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_SpecularColor;
					materialBuffer.emissive = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_EmissiveColor;
					materialBuffer.ambient = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_AmbientColor;
					materialBuffer.shininess = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_Shininess;
					materialBuffer.shininessStrength = meshUpdateData.m_pMaterialAttribute->m_phongBlinn.m_ShininessStrength;
					materialBuffer.reflectivity = meshUpdateData.m_pMaterialAttribute->m_Reflectivity;

					materialBuffer.reflectance = meshUpdateData.m_pMaterialAttribute->m_Reflectance;
					materialBuffer.reflectanceTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_ReflectanceTexture);

					//NO PBR for now

					//materialBuffer.normalTex = textureMgr.GetTextureID(meshUpdateData.m_pMaterialAttribute->m_PBRAttributes.m_NormalTex);
					materialBuffer.MaterialID = 0;
					materialBuffer.ShadingModel = meshUpdateData.m_pMaterialAttribute->m_shading;
					materialBuffer.UseMaterialTextures = meshUpdateData.m_UseMaterials;
				}
				++m_Composition3DInstance.m_TotalInstances;

			}
			instanceReq.m_ModelName = ModelName;
			startingOffset = m_Composition3DInstance.m_TotalInstances;
			itr.second.m_Index = 0;
			++m_Composition3DInstance.m_GroupIdx;
		}

		GBufferPipeline->UpdateUBO(&m_SceneUBO, sizeof(SceneUniform), 5, frameIndex);
		GBufferPipeline->UpdateUBO(m_Composition3DInstance.m_InstanceBuffers.data(), sizeof(BatchData) * m_Composition3DInstance.m_TotalInstances, 15, frameIndex);

		//if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayInstance)
		//{
		//	GBufferPipeline->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
		//	AssetManager::GetInstance()->GetTextureFactory().m_UpdateArrayInstance = false;
		//}

		GBufferPipeline->SetCommandBuffer(commandBuffer);
		for (std::uint32_t i = 0; i < m_Composition3DInstance.m_GroupIdx; ++i)
		{
			auto& instanceReq = m_Composition3DInstance.m_InstanceRequests[i];

			if (useMaterials)
				UploadMaterialsList(instanceReq.m_ModelName);


			GBufferPipeline->BindPipeline();
			GBufferPipeline->BindVertexBuffer(*instanceReq.m_MeshBuffer->m_VertexBuffer);
			GBufferPipeline->BindIndexBuffer(*instanceReq.m_MeshBuffer->m_IndexBuffer);
			GBufferPipeline->BindDescriptor(frameIndex, 1);
			GBufferPipeline->BindArrayDescriptorSet(0, 1, 1);
			GBufferPipeline->SubmitPushConstant(&instanceReq.m_RenderInstanceInfo.m_InstanceOffset, sizeof(std::uint32_t), SHADER_FLAG::VERTEX);
			GBufferPipeline->DrawInstancedIndexed(*instanceReq.m_MeshBuffer->m_VertexBuffer, *instanceReq.m_MeshBuffer->m_IndexBuffer, instanceReq.m_RenderInstanceInfo.m_Instances, frameIndex);

			instanceReq.m_RenderInstanceInfo.m_InstanceOffset = 0;
			instanceReq.m_RenderInstanceInfo.m_Instances = 0;
		}

		m_Composition3DInstance.m_TotalInstances = 0;
		m_Composition3DInstance.m_GroupIdx = 0;
	}
	void DeferredController::G_BufferPass(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{
		//UpdateMaterialList();

		m_FrameBuffers[RENDER_PASS::RENDER_G_BUFFER]->BeginRenderPass(commandBuffer);
		{

			auto SkyBox = GraphicsManager::getInstance().GetSkyBox();
			if (GraphicsManager::getInstance().IsViewingFrom2D() == false)
				SkyBox->RenderSkyBox(commandBuffer, frameIndex);

			G_BufferPassBatch(commandBuffer, frameIndex);
			G_BufferInstanced(commandBuffer, frameIndex);


			auto& DebugRenderer = GraphicsManager::getInstance().GetDebugRenderer();
			if (CollisionSystem::m_RenderDebugDrawing)
			{
				m_LightingPushConstant.toggleDebugLight = 0;
				DebugRenderer.Render(commandBuffer, frameIndex);
			}
			else
			{
				m_LightingPushConstant.toggleDebugLight = 1;
			}

			ParticleSystem::Render();
		}
		m_FrameBuffers[RENDER_PASS::RENDER_G_BUFFER]->EndRenderPass(commandBuffer);
	}

	void DeferredController::SubmitInstance(std::uint32_t entityID, int TextureID, Transform* transformComp, GraphicsComponent* graphComp)
	{
		MeshController* pMeshController = graphComp->m_MeshControllerRef.m_ResourcePtr;
		MeshBuffer* meshBuffer = pMeshController->GetMeshBuffer();
		MaterialManager& mtrMgr = GraphicsManager::getInstance().GetMaterialManager();

		if (m_GBufferInstance.m_GroupIdx >= MAX_INSTANCE_BUFFER)
		{
			TDS_ERROR("We have reached the maximum amount of instances!");
			return;
		}


		auto& instanceUpdatePack = m_GBufferInstance.m_instanceRenderManager.m_InstanceUpdateInfo[meshBuffer];

		MeshUpdate* updateData = (instanceUpdatePack.m_Index == instanceUpdatePack.m_Updates.size()) ?
			&instanceUpdatePack.m_Updates.emplace_back() : &instanceUpdatePack.m_Updates[instanceUpdatePack.m_Index];


		updateData->m_pTransform = transformComp;
		updateData->m_IsAnimated = graphComp->m_IsAnimated;
		updateData->m_TextureID = TextureID;
		updateData->m_EntityID = entityID;
		updateData->m_MeshID = 0;
		updateData->m_ShowMesh = graphComp->ShowMesh();
		updateData->m_RenderIn2D = graphComp->m_UsedIn2D;
		updateData->m_pMaterialAttribute = &graphComp->m_MaterialAttributes;
		updateData->m_UseMaterials = graphComp->m_UseMaterials;
		updateData->m_UsePreloadedMaterials = graphComp->m_UsePreloadMaterials;
		updateData->m_ModelName = graphComp->m_ModelName;

		++instanceUpdatePack.m_Index;

	}
	std::shared_ptr<VulkanPipeline> DeferredController::GetDeferredPipeline(DEFERRED_STAGE stage)
	{
		return m_DeferredPipelines[stage];
	}
	void DeferredController::ShutDown()
	{
		for (auto& pipelines : m_DeferredPipelines)
		{
			if (pipelines != nullptr)
			{
				pipelines->ShutDown();
			}
		}

		if (m_LightSource)
			m_LightSource->ShutDown();

		for (auto& fb : m_FrameBuffers)
		{
			if (fb != nullptr)
				fb->Destroy();
		}
	}
	void DeferredController::UpdateAllTextureArrays()
	{
		if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D)
		{
			m_DeferredPipelines[STAGE_G_BUFFER_BATCH]->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
			m_DeferredPipelines[STAGE_G_BUFFER_INSTANCE]->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
			m_DeferredPipelines[STAGE_3D_COMPOSITION_INSTANCE]->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
			m_DeferredPipelines[STAGE_3D_COMPOSITION_BATCH]->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
			AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray3D = false;
		}
	}
	void DeferredController::UpdateMaterialList()
	{


		auto& textureMgr = AssetManager::GetInstance()->GetTextureFactory();
		auto& materialMgr = GraphicsManager::getInstance().GetMaterialManager();

		if (materialMgr.m_UpdateMaterialList)
		{
			unsigned int MatIndx = 0;
			for (auto& materialData : materialMgr.m_ModelToMaterials)
			{
				for (auto& materialAttribute : materialData.second.m_materialAttributes)
				{
					auto& materialBuffer = m_MaterialList[MatIndx++];

					materialBuffer.diffuseTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_DiffuseTex);
					materialBuffer.specularTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_SpecularTex);
					materialBuffer.ambientTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_AmbientTex);
					materialBuffer.emissiveTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_EmissiveTex);
					materialBuffer.shininessTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_ShininessTex);
					materialBuffer.diffuse = materialAttribute.second.m_phongBlinn.m_DiffuseColor;
					materialBuffer.specular = materialAttribute.second.m_phongBlinn.m_SpecularColor;
					materialBuffer.emissive = materialAttribute.second.m_phongBlinn.m_EmissiveColor;
					materialBuffer.ambient = materialAttribute.second.m_phongBlinn.m_AmbientColor;
					materialBuffer.shininess = materialAttribute.second.m_phongBlinn.m_Shininess;
					materialBuffer.shininessStrength = materialAttribute.second.m_phongBlinn.m_ShininessStrength;
					materialBuffer.reflectivity = materialAttribute.second.m_Reflectivity;

					materialBuffer.reflectance = materialAttribute.second.m_Reflectance;
					materialBuffer.reflectanceTex = textureMgr.GetTextureID(materialAttribute.second.m_ReflectanceTexture);

					//NO PBR for now

					//materialBuffer.normalTex = textureMgr.GetTextureID(materialAttribute.second.m_PBRAttributes.m_NormalTex);
					materialBuffer.MaterialID = materialAttribute.first;
					materialBuffer.ShadingModel = materialAttribute.second.m_shading;
					materialBuffer.UseMaterialTextures = true;
				}


			}
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH]->UpdateUBO(m_MaterialList.data(), m_MaterialList.size() * sizeof(MaterialBuffer), 16, 0);
			m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH]->UpdateUBO(m_MaterialList.data(), m_MaterialList.size() * sizeof(MaterialBuffer), 16, 1);
			materialMgr.m_UpdateMaterialList = false;
		}

	}
	void DeferredController::UploadMaterialsList(std::string_view modelName)
	{
		auto& textureMgr = AssetManager::GetInstance()->GetTextureFactory();
		auto& materialMgr = GraphicsManager::getInstance().GetMaterialManager();

		/*if (materialMgr.m_UpdateMaterialList)
		{*/
		unsigned int MatIndx = 0;

		for (auto& materialAttribute : materialMgr.m_ModelToMaterials[modelName.data()].m_materialAttributes)
		{
			auto& materialBuffer = m_MaterialList[MatIndx++];

			materialBuffer.diffuseTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_DiffuseTex);
			materialBuffer.specularTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_SpecularTex);
			materialBuffer.ambientTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_AmbientTex);
			materialBuffer.emissiveTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_EmissiveTex);
			materialBuffer.shininessTex = textureMgr.GetTextureID(materialAttribute.second.m_phongBlinn.m_ShininessTex);
			materialBuffer.diffuse = materialAttribute.second.m_phongBlinn.m_DiffuseColor;
			materialBuffer.specular = materialAttribute.second.m_phongBlinn.m_SpecularColor;
			materialBuffer.emissive = materialAttribute.second.m_phongBlinn.m_EmissiveColor;
			materialBuffer.ambient = materialAttribute.second.m_phongBlinn.m_AmbientColor;
			materialBuffer.shininess = materialAttribute.second.m_phongBlinn.m_Shininess;
			materialBuffer.shininessStrength = materialAttribute.second.m_phongBlinn.m_ShininessStrength;
			materialBuffer.reflectivity = materialAttribute.second.m_Reflectivity;

			materialBuffer.reflectance = materialAttribute.second.m_Reflectance;
			materialBuffer.reflectanceTex = textureMgr.GetTextureID(materialAttribute.second.m_ReflectanceTexture);

			//NO PBR for now

			//materialBuffer.normalTex = textureMgr.GetTextureID(materialAttribute.second.m_PBRAttributes.m_NormalTex);
			materialBuffer.MaterialID = materialAttribute.first;
			materialBuffer.ShadingModel = materialAttribute.second.m_shading;
			materialBuffer.UseMaterialTextures = true;
		}



		m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH]->UpdateUBO(m_MaterialList.data(), MatIndx * sizeof(MaterialBuffer), 16, 0);
		m_DeferredPipelines[DEFERRED_STAGE::STAGE_G_BUFFER_BATCH]->UpdateUBO(m_MaterialList.data(), MatIndx * sizeof(MaterialBuffer), 16, 1);
		//materialMgr.m_UpdateMaterialList = false;
		/*}*/
	}
	FBO* DeferredController::GetFrameBuffer(RENDER_PASS renderpassType)
	{
		return m_FrameBuffers[renderpassType];
	}
	SceneUniform& DeferredController::GetSceneUniform()
	{
		return m_SceneUBO;
	}
	void DeferredController::LightingPass(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{

		if (GraphicsManager::getInstance().IsViewingFrom2D() == true)
		{
			m_LightingPushConstant.toggleDebugLight = false;
		}
		m_SceneUBO.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
		m_SceneUBO.m_Proj = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);

		m_SceneUBO.m_Proj.m[1][1] *= -1;


		m_SceneCamera.view = m_SceneUBO.m_View;
		m_SceneCamera.projection = m_SceneUBO.m_Proj;
		m_SceneCamera.cameraPos = Vec4(GraphicsManager::getInstance().GetCamera().getPosition(), 1.f);
		auto LightingBuffer = m_FrameBuffers[RENDER_LIGTHING];
		auto lightingPipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_LIGTHING];

		lightingPipeline->SetCommandBuffer(commandBuffer);
		lightingPipeline->UpdateUBO(&m_SceneCamera, sizeof(SceneCamera), 7, frameIndex);
		lightingPipeline->UpdateUBO(m_LightUBOs.m_PointLights.data(), sizeof(Pointlight) * m_LightingPushConstant.activepointlights, 21, frameIndex);
		lightingPipeline->UpdateUBO(m_LightUBOs.m_DirectionalLight.data(), sizeof(DirectionalLight) * m_LightingPushConstant.activeDirLights, 22, frameIndex);
		lightingPipeline->UpdateUBO(m_LightUBOs.m_SpotLights.data(), sizeof(SpotLight) * m_LightingPushConstant.activeSpotLights, 23, frameIndex);

		//m_LightSource->SetCommandBuffer(commandBuffer);
		//m_LightSource->UpdateUBO(&m_SceneUBO, sizeof(SceneUniform), 5, frameIndex);
		//m_LightSource->UpdateUBO(m_LightSourceBuffers.data(), m_LightSrcInstance * sizeof(LightSourceProperties), 11, frameIndex);

		LightingBuffer->BeginRenderPass(commandBuffer);
		{


			lightingPipeline->BindPipeline();
			lightingPipeline->BindDescriptor(frameIndex, 1);
			lightingPipeline->SubmitPushConstant(&m_LightingPushConstant, sizeof(LightingPushConstant), SHADER_FLAG::FRAGMENT);
			lightingPipeline->Draw(6, frameIndex);

			/*m_LightSource->BindPipeline();
			m_LightSource->BindDescriptor(frameIndex, 1);
			m_LightSource->Draw(6, frameIndex, m_LightSrcInstance);*/



		}
		LightingBuffer->EndRenderPass(commandBuffer);
		m_LightingPushConstant.activeDirLights = 0;
		m_LightingPushConstant.activepointlights = 0;
		m_LightingPushConstant.activeSpotLights = 0;
		m_LightSrcInstance = 0;

	}
	void DeferredController::UpdateDeferredTextures()
	{
		auto G_Buffer = m_FrameBuffers[RENDER_G_BUFFER];
		auto Lighting = m_FrameBuffers[RENDER_LIGTHING];
		auto Composition = m_FrameBuffers[RENDER_COMPOSITION];
		
		auto LightingPipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_LIGTHING];
		auto CompositionPipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_COMPOSITION];



		//Update lightihng pass
		{
			auto albedo = G_Buffer->GetTargets().at(0)->getImageInfoDescriptor();
			auto positions = G_Buffer->GetTargets().at(2)->getImageInfoDescriptor();
			auto normals = G_Buffer->GetTargets().at(3)->getImageInfoDescriptor();
			auto PhongBlinnProp = G_Buffer->GetTargets().at(4)->getImageInfoDescriptor();
			auto LightCondition = G_Buffer->GetTargets().at(5)->getImageInfoDescriptor();
			

			LightingPipeline->UpdateDescriptor(albedo, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, 0);
			LightingPipeline->UpdateDescriptor(albedo, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, 0);

			LightingPipeline->UpdateDescriptor(positions, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, 0);
			LightingPipeline->UpdateDescriptor(normals, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6, 0);
			LightingPipeline->UpdateDescriptor(albedo, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, 1);
			LightingPipeline->UpdateDescriptor(positions, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, 1);
			LightingPipeline->UpdateDescriptor(normals, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6, 1);

			LightingPipeline->UpdateDescriptor(PhongBlinnProp, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8, 0);
			LightingPipeline->UpdateDescriptor(PhongBlinnProp, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 8, 1);

			LightingPipeline->UpdateDescriptor(LightCondition, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 9, 0);
			LightingPipeline->UpdateDescriptor(LightCondition, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 9, 1);
		
		
		}

		//Update composition Pass
		{
			auto albedo = Lighting->GetTargets().at(0)->getImageInfoDescriptor();
			auto GammaCondition = Lighting->GetTargets().at(1)->getImageInfoDescriptor();
			CompositionPipeline->UpdateDescriptor(albedo, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, 0);
			CompositionPipeline->UpdateDescriptor(albedo, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, 1);
			CompositionPipeline->UpdateDescriptor(GammaCondition, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, 0);
			CompositionPipeline->UpdateDescriptor(GammaCondition, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, 1);
		}

	}

	void DeferredController::CombinationPass(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{
		auto fbo = m_FrameBuffers[RENDER_PASS::RENDER_COMPOSITION];
		auto pipeline = m_DeferredPipelines[DEFERRED_STAGE::STAGE_COMPOSITION];
		auto renderer2D = GraphicsManager::getInstance().GetRenderer2D();
		auto fontrenderer = GraphicsManager::getInstance().GetFontRenderer();
		pipeline->SetCommandBuffer(commandBuffer);
		renderer2D->GetBatchList().PrepareBatch();
		fontrenderer->GetBatchList().PrepareBatch();
		renderer2D->Update(commandBuffer, frameIndex);
		fontrenderer->Update(commandBuffer, frameIndex);

		m_SceneUBO.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
		m_SceneUBO.m_Proj = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);

		m_SceneUBO.m_Proj.m[1][1] *= -1;
		fbo->BeginRenderPass(commandBuffer);
		{
			if (GraphicsManager::getInstance().IsViewingFrom2D() == false)
			{
				pipeline->BindPipeline();
				pipeline->SubmitPushConstant(&m_ScreenFadeFactor, sizeof(float), SHADER_FLAG::FRAGMENT);
				pipeline->BindDescriptor(frameIndex, 1);
				pipeline->Draw(6, frameIndex);

			}
			else
			{
				Mat4 scaleFactor = Mat4::identity();
				if (GraphicsManager::getInstance().IsNormalizedView())
				{
					scaleFactor = Mat4::Scale(Vec3(0.005f, 0.005f, 0.005f));
				}
				m_SceneUBO.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix() * scaleFactor;
				m_SceneUBO.m_Proj = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
					GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);

				m_SceneUBO.m_Proj.m[1][1] *= -1;
				RenderUISceneMeshBatch(commandBuffer, frameIndex);
				RenderUISceneMeshInstance(commandBuffer, frameIndex);
			}

			renderer2D->Draw(commandBuffer, frameIndex);
			fontrenderer->Draw(commandBuffer, frameIndex);



		}
		fbo->EndRenderPass(commandBuffer);
		renderer2D->GetBatchList().Clear();
		fontrenderer->GetBatchList().Clear();

		m_LightSrcInstance = 0;


	}
	void DeferredController::SetClearColour(Vec4 clearColor)
	{
		for (auto& fbo : m_FrameBuffers)
		{
			fbo->SetClearColour(clearColor);
		}
	}

	void DeferredController::SubmitPointLight(std::uint32_t entityID, PointLightComponent* poingLight, Transform* transformComp)
	{
		//auto& LightSrcBuffer = m_LightSourceBuffers[m_LightSrcInstance];
		//LightSrcBuffer.Position = transformComp->GetPosition();
		//LightSrcBuffer.Radius = transformComp->GetScale().x;
		//LightSrcBuffer.Color = poingLight->m_color;


		poingLight->m_Position = transformComp->GetPosition();

		//m_LightSrcInstance++;

		auto& pointLightBuffer = m_LightUBOs.m_PointLights[m_LightingPushConstant.activepointlights];
		pointLightBuffer.Position = poingLight->m_Position;
		pointLightBuffer.Color = poingLight->m_color;
		pointLightBuffer.attenuation = poingLight->m_Attenuation;
		m_LightingPushConstant.activepointlights++;
	}
	void DeferredController::SubmitDirectionalLight(std::uint32_t entityID, DirectionalLightComponent* dirLight, Transform* transformComp)
	{
		//auto& LightSrcBuffer = m_LightSourceBuffers[m_LightSrcInstance];
		//LightSrcBuffer.Position = transformComp->GetPosition();
		//LightSrcBuffer.Radius = transformComp->GetScale().x;
		//LightSrcBuffer.Color = dirLight->m_color;

		//m_LightSrcInstance++;

		auto& dirLightBuffer = m_LightUBOs.m_DirectionalLight[m_LightingPushConstant.activeDirLights];
		dirLightBuffer.direction = dirLight->m_Direction;
		dirLightBuffer.Color = dirLight->m_color;

		m_LightingPushConstant.activeDirLights++;
	}
	void DeferredController::SubmitSpotLight(std::uint32_t entityID, SpotLightComponent* spotLightComp, Transform* transformComp)
	{
		//m_SceneUBO.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
		//m_SceneUBO.m_Proj = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
		//	GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
		//m_SceneUBO.m_View.m[1][1] *= -1;
		////We need to get the direction of the camera and set it to the spot light
		////We can get the direction from the view matrix
		//Mat4 view = m_SceneUBO.m_View;
		//m_SceneUBO.m_View.m[1][1] *= -1;
		//Vec3 forwardDirection(
		//		view.m[0][2],
		//		view.m[1][2],
		//		view.m[2][2]
		//	);

		//forwardDirection.Normalize();


		//spotLightComp->m_Position = GraphicsManager::getInstance().GetCamera().getPosition();
		//spotLightComp->m_direction = forwardDirection;






		//auto& LightSrcBuffer = m_LightSourceBuffers[m_LightSrcInstance];
		//LightSrcBuffer.Position = transformComp->GetPosition();
		//LightSrcBuffer.Radius = transformComp->GetScale().x;
		//LightSrcBuffer.Color = spotLightComp->m_color;
		//m_LightSrcInstance++;

		//Vec4 positionInViewSpace = m_SceneUBO.m_View * Vec4(spotLightComp->m_Position, 1.0);


		//Vec4 directionInWorldSpace = Vec4(spotLightComp->m_direction, 0.0); 
		//Vec4 directionInViewSpace = m_SceneUBO.m_View * directionInWorldSpace;

		//directionInViewSpace.normalize();
		auto& spotLightBuffer = m_LightUBOs.m_SpotLights[m_LightingPushConstant.activeSpotLights];
		//spotLightBuffer.direction = spotLightComp->m_direction;
		//spotLightBuffer.Position = spotLightComp->m_Position;

		/*NameTag* nameTag = ecs.getComponent<NameTag>(entityID);
		if (nameTag->GetName() == "FlashLight")
		{
			spotLightComp->m_direction = GraphicsManager::getInstance().GetCamera().getForwardVector();
			spotLightComp->m_direction.Normalize();

		}*/


		spotLightBuffer.direction = spotLightComp->m_direction;
		spotLightBuffer.Position = transformComp->GetPosition();


		spotLightBuffer.attenuation = spotLightComp->m_attenuation;
		spotLightBuffer.Color = spotLightComp->m_color;
		spotLightBuffer.cutoff = spotLightComp->m_Angle;

		m_LightingPushConstant.activeSpotLights++;
	}
	void DeferredController::CreateFrameBuffers(std::uint32_t width, std::uint32_t height)
	{
		m_FrameBuffers[RENDER_G_BUFFER] = new G_Buffer(Vec2(float(width), float(height)));
		m_FrameBuffers[RENDER_G_BUFFER]->Create();

		RenderTarget*& commonDepth = m_FrameBuffers[RENDER_G_BUFFER]->GetDepthTarget();
		m_FrameBuffers[RENDER_LIGTHING] = new LightingBuffer(Vec2(float(width), float(height)));
		m_FrameBuffers[RENDER_LIGTHING]->Create();
		m_FrameBuffers[RENDER_COMPOSITION] = new CompositionPass(Vec2(float(width), float(height)));
		m_FrameBuffers[RENDER_COMPOSITION]->Create();



	}
	void DeferredController::Resize(std::uint32_t width, std::uint32_t height)
	{
		auto G_Buffer = m_FrameBuffers[RENDER_G_BUFFER];
		auto Lighting = m_FrameBuffers[RENDER_LIGTHING];
		auto Composition = m_FrameBuffers[RENDER_COMPOSITION];

		G_Buffer->Resize(width, height);
		Lighting->Resize(width, height);
		Composition->Resize(width, height);


		UpdateDeferredTextures();


	}

}