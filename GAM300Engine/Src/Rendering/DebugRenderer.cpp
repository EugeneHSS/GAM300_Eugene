#include "Rendering/DebugRenderer.h"
#include "GraphicsResource/AssetModel.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#include "Physics/CollisionSystem.h"
#include "AssetManagement/AssetManager.h"
#include "Rendering/Revamped/DeferredController.h"
namespace TDS
{




	DebugRenderer::DebugRenderer(VulkanInstance& Instance) : m_Instance(Instance)
	{

	}

	DebugRenderer::~DebugRenderer()
	{
	}

	void DebugRenderer::DestroyPipeline()
	{
		m_Pipeline->ShutDown();
	}

	void DebugRenderer::Render(VkCommandBuffer commandBuffer, std::uint32_t frameIndex)
	{

		m_DebugInstance.BuildAllGroups();
		m_Pipeline->SetCommandBuffer(commandBuffer);
		m_SceneInfo.View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();

		m_SceneInfo.Proj = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
		m_SceneInfo.Proj.m[1][1] *= -1;

		m_Pipeline->UpdateUBO(&m_SceneInfo, sizeof(SceneInfo), 5, frameIndex);
		m_Pipeline->UpdateUBO(m_DebugInstance.m_DebugInstanceBuffer.data(), sizeof(DebugInstanceBuffer) * m_DebugInstance.m_InstanceCnt, 15, frameIndex);

		for (std::uint32_t i = 0; i < m_DebugInstance.m_TotalGroup; ++i)
		{
			auto& instanceReq = m_DebugInstance.m_DebugCommand[i];

			m_Pipeline->BindPipeline(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
			m_Pipeline->BindVertexBuffer(*instanceReq.m_pBuffer->m_VertexBuffer);
			m_Pipeline->BindIndexBuffer(*instanceReq.m_pBuffer->m_IndexBuffer);
			m_Pipeline->BindDescriptor(frameIndex, 1);
			m_Pipeline->SubmitPushConstant(&instanceReq.m_Offset, sizeof(std::uint32_t), SHADER_FLAG::VERTEX);
			m_Pipeline->DrawInstancedIndexed(*instanceReq.m_pBuffer->m_VertexBuffer, *instanceReq.m_pBuffer->m_IndexBuffer, instanceReq.m_Instance, frameIndex);
			instanceReq.m_Offset = 0;
			instanceReq.m_Instance = 0;
		}

		m_DebugInstance.m_InstanceCnt = 0;
		m_DebugInstance.m_TotalGroup = 0;




	}
	VulkanPipeline& DebugRenderer::GetPipeline() {
		return *m_Pipeline;
	}
	bool DebugRenderer::Init()
	{
		CollisionSystem::PreInit();
		PipelineCreateEntry entry{};

		entry.m_EnableDoubleBuffering = true;
		entry.m_NumDescriptorSets = 1;
		entry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		entry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		entry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		entry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		entry.m_PipelineConfig.m_CullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
		entry.m_PipelineConfig.m_PipelineDrawModes = { VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST };
		entry.m_PipelineConfig.m_EnableDepthTest = true;
		entry.m_PipelineConfig.m_EnableDepthWrite = true;

		entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/DebugVert.spv"));
		entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/DebugFrag.spv"));


		entry.m_ShaderInputs.m_InputBuffers[15].m_Size = MAX_BUFFER_SIZE * sizeof(DebugInstanceBuffer);
		entry.m_ShaderInputs.m_InputBuffers[15].m_Data = m_DebugInstance.m_DebugInstanceBuffer.data();
		entry.m_ShaderInputs.m_InputBuffers[15].m_Static = false;



		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CUBE].m_VertexBuffer = std::make_shared<VMABuffer>();
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CUBE].m_IndexBuffer = std::make_shared<VMABuffer>();

		std::vector<Vec3> CubeVertices;
		std::vector<Vec3> CapsuleVertices;
		std::vector<Vec3> SphereVertices;

		std::vector<float>& m_CubeFloat = CollisionSystem::cubeVertices;
		std::vector<float>& m_SphereVertices = CollisionSystem::sphereVertices;
		std::vector<float>& m_CapsuleVertices = CollisionSystem::capsuleVertices;

		if (m_CubeFloat.size() % 3 != 0)
		{
			TDS_ERROR("Something wrong");
			return false;
		}

		if (m_SphereVertices.size() % 3 != 0)
		{
			TDS_ERROR("Something wrong");
			return false;
		}

		if (m_CapsuleVertices.size() % 3 != 0)
		{
			TDS_ERROR("Something wrong");
			return false;
		}
		for (size_t i = 0; i < m_CubeFloat.size(); i += 3)
		{
			CubeVertices.emplace_back(Vec3(m_CubeFloat[i], m_CubeFloat[i + 1], m_CubeFloat[i + 2]));
		}
		for (size_t i = 0; i < m_SphereVertices.size(); i += 3)
		{
			SphereVertices.emplace_back(Vec3(m_SphereVertices[i], m_SphereVertices[i + 1], m_SphereVertices[i + 2]));
		}
		for (size_t i = 0; i < m_CapsuleVertices.size(); i += 3)
		{
			CapsuleVertices.emplace_back(Vec3(m_CapsuleVertices[i], m_CapsuleVertices[i + 1], m_CapsuleVertices[i + 2]));
		}

		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CUBE].m_VertexBuffer->MappedStaging(CubeVertices.size() * sizeof(Vec3), VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_Instance, CubeVertices.data());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CUBE].m_VertexBuffer->SetDataCnt(CubeVertices.size());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CUBE].m_IndexBuffer->MappedStaging(CollisionSystem::cubeIndices.size() * sizeof(std::uint32_t), VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_Instance, CollisionSystem::cubeIndices.data());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CUBE].m_IndexBuffer->SetDataCnt(CollisionSystem::cubeIndices.size());

		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_SPHERE].m_VertexBuffer = std::make_shared<VMABuffer>();
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_SPHERE].m_IndexBuffer = std::make_shared<VMABuffer>();

		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_SPHERE].m_VertexBuffer->MappedStaging(SphereVertices.size() * sizeof(Vec3), VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_Instance, SphereVertices.data());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_SPHERE].m_VertexBuffer->SetDataCnt(SphereVertices.size());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_SPHERE].m_IndexBuffer->MappedStaging(CollisionSystem::sphereIndices.size() * sizeof(std::uint32_t), VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_Instance, CollisionSystem::sphereIndices.data());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_SPHERE].m_IndexBuffer->SetDataCnt(CollisionSystem::sphereIndices.size());

		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CAPSULE].m_VertexBuffer = std::make_shared<VMABuffer>();
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CAPSULE].m_IndexBuffer = std::make_shared<VMABuffer>();

		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CAPSULE].m_VertexBuffer->MappedStaging(CapsuleVertices.size() * sizeof(Vec3), VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_Instance, CapsuleVertices.data());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CAPSULE].m_VertexBuffer->SetDataCnt(CapsuleVertices.size());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CAPSULE].m_IndexBuffer->MappedStaging(CollisionSystem::capsuleIndices.size() * sizeof(std::uint32_t), VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_Instance, CollisionSystem::capsuleIndices.data());
		m_DebugInstance.m_DebugDrawing[DEBUG_RENDER_CAPSULE].m_IndexBuffer->SetDataCnt(CollisionSystem::capsuleIndices.size());

		VertexLayout layout = VertexLayout
		(
			{
			  VertexBufferElement(VAR_TYPE::VEC3, "vPosition")
			}
		);

		entry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(Vec3)));
		m_Pipeline = std::make_unique<VulkanPipeline>();
		auto deferredController = GraphicsManager::getInstance().GetDeferredController();

		auto frameBuffer = deferredController->GetFrameBuffer(RENDER_G_BUFFER);
		entry.m_FBTarget = frameBuffer;
		return m_Pipeline->Create(entry);
	}
	void DebugRenderer::ToggleDebug(bool condition)
	{
		m_DebugToggle = condition;
	}
	bool DebugRenderer::GetDebugToggle() const
	{
		return m_DebugToggle;
	}
	void DebugRenderer::BuildUpdates()
	{

	}
	void DebugInstance::SubmitToDebugGroup(Transform* transform, EntityID id)
	{
		BoxCollider* Box = ecs.getComponent<BoxCollider>(id);
		SphereCollider* Sphere = ecs.getComponent<SphereCollider>(id);
		CapsuleCollider* Capsule = ecs.getComponent<CapsuleCollider>(id);



		if (Box)
		{
			auto& debugInstance = m_Instances[DEBUG_RENDER_CUBE];

			DebugInstanceInfo* updateInfo = (debugInstance.m_Index == debugInstance.m_DebugInstanceInfo.size()) ?
				&debugInstance.m_DebugInstanceInfo.emplace_back() : &debugInstance.m_DebugInstanceInfo[debugInstance.m_Index];

			updateInfo->m_Colour = Vec4(0.f, 1.f, 0.f, 1.f);
			updateInfo->m_Transform = transform;

			++debugInstance.m_Index;
		}

		else if (Sphere)
		{
			auto& debugInstance = m_Instances[DEBUG_RENDER_SPHERE];

			DebugInstanceInfo* updateInfo = (debugInstance.m_Index == debugInstance.m_DebugInstanceInfo.size()) ?
				&debugInstance.m_DebugInstanceInfo.emplace_back() : &debugInstance.m_DebugInstanceInfo[debugInstance.m_Index];

			updateInfo->m_Colour = Vec4(0.f, 1.f, 0.f, 1.f);
			updateInfo->m_Transform = transform;

			++debugInstance.m_Index;

		}

		else if (Capsule)
		{
			auto& debugInstance = m_Instances[DEBUG_RENDER_CAPSULE];

			DebugInstanceInfo* updateInfo = (debugInstance.m_Index == debugInstance.m_DebugInstanceInfo.size()) ?
				&debugInstance.m_DebugInstanceInfo.emplace_back() : &debugInstance.m_DebugInstanceInfo[debugInstance.m_Index];

			updateInfo->m_Colour = Vec4(0.f, 1.f, 0.f, 1.f);
			updateInfo->m_Transform = transform;

			++debugInstance.m_Index;
		}
	}
	void DebugInstance::BuildAllGroups()
	{
		int startOffset = 0;
		for (auto& itr : m_Instances)
		{
			auto& instanceReq = m_DebugCommand[m_TotalGroup];
			instanceReq.m_pBuffer = &m_DebugDrawing[itr.first];

			for (std::uint32_t i = 0; i < itr.second.m_DebugInstanceInfo.size(); ++i)
			{

				auto& meshUpdateData = itr.second.m_DebugInstanceInfo[i];


				auto& instaneBuffer = m_DebugInstanceBuffer[m_InstanceCnt];

				instanceReq.m_Offset = startOffset;
				instanceReq.m_Instance = itr.second.m_Index;

				{

					instaneBuffer.m_WorldTransform = meshUpdateData.m_Transform->GetTransformMatrix();
					instaneBuffer.m_Colour = meshUpdateData.m_Colour;
				}

				++m_InstanceCnt;

			}
			startOffset = m_InstanceCnt;
			itr.second.m_Index = 0;
			++m_TotalGroup;
		}


	}


}
