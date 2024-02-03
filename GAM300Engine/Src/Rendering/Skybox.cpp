#include "Rendering/Skybox.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "vulkanTools/Renderer.h"
#include "vulkanTools/VulkanTexture.h"
#include "Rendering/GraphicsManager.h"
#include "Rendering/Revamped/DeferredController.h"
namespace TDS
{
	void SkyBoxRenderer::Init()
	{
	
		m_SkyBoxTexture.m_VulkanTexture = new VulkanTexture();
		m_SkyBoxPipeline = std::make_shared<VulkanPipeline>();
	
		m_SkyBoxTexture.LoadTexture("../assets/textures/skybox1.dds");

	
		if (!AssetManager::GetInstance()->GetMeshFactory().GetMeshController("cube_Bin.bin", m_CubeMapModel))
		{
			TDS_ERROR("FAILED TO LOAD SKYBOX!");
			return;
		}

		m_CubeMapVertexBuffer = std::make_shared<VMABuffer>();

		auto* modelPack = m_CubeMapModel.m_ResourcePtr->GetModelPack();

		std::vector<Vec3> pos(modelPack->m_ModelHandle.m_ModelVertex.size());

		for (size_t i = 0; i < pos.size(); ++i)
		{
			pos[i] = modelPack->m_ModelHandle.m_ModelVertex[i].m_Position;
		}

		m_CubeMapVertexBuffer->CreateVertexBuffer(pos.size() * sizeof(Vec3), true, pos.data());
		m_CubeMapVertexBuffer->SetDataCnt(static_cast<uint32_t>(pos.size()));

		m_CubeMapIndexBuffer = std::make_shared<VMABuffer>();
		m_CubeMapIndexBuffer->CreateIndexBuffer(modelPack->m_ModelHandle.m_Indices.size() * sizeof(uint32_t), true, modelPack->m_ModelHandle.m_Indices.data());
		m_CubeMapIndexBuffer->SetDataCnt(static_cast<uint32_t>(modelPack->m_ModelHandle.m_Indices.size()));

		auto deferredController = GraphicsManager::getInstance().GetDeferredController();

		auto frameBuffer = deferredController->GetFrameBuffer(RENDER_G_BUFFER);
		
		PipelineCreateEntry skyboxCE{};
		skyboxCE.m_NumDescriptorSets = 1;
		skyboxCE.m_PipelineConfig.m_CullMode = VK_CULL_MODE_FRONT_BIT;
		skyboxCE.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/skybox.spv"));
		skyboxCE.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/skyboxfrag.spv"));
		VertexLayout layout{ VertexBufferElement(VAR_TYPE::VEC3, "inPos") };
		skyboxCE.m_PipelineConfig.m_EnableDepthTest = false;
		skyboxCE.m_PipelineConfig.m_EnableDepthWrite = false;
		skyboxCE.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(SkyBoxVertexData)));
		skyboxCE.m_FBTarget = frameBuffer;
		m_SkyBoxPipeline->Create(skyboxCE);
		
	}

	void SkyBoxRenderer::RenderSkyBox(VkCommandBuffer commandBuffer, uint32_t frameIndex)
	{
		if (GraphicsManager::getInstance().IsViewingFrom2D())
			return;

		m_SkylinePositions = GraphicsManager::getInstance().GetCamera().getPosition();
		Quat qRot = Quat(m_SkylineRot);
		Mat4 scaleM4 = Mat4::Scale(m_SkylineScale);
		Mat4 rotM4 = Mat4(Quat::toMat4(qRot));
		Mat4 transM4 = Mat4::Translate(m_SkylinePositions);
		Mat4 TransformMatrix;
		TransformMatrix = transM4 * rotM4 * scaleM4;

		m_SkyBoxPipeline->UpdateTexture(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, *m_SkyBoxTexture.m_VulkanTexture);
		m_SkyBoxPipeline->SetCommandBuffer(commandBuffer);
		skyboxubo.m_View = GraphicsManager::getInstance().GetCamera().GetViewMatrix() * TransformMatrix;
		skyboxubo.m_Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000.f);
		skyboxubo.m_Projection.m[1][1] *= -1;
		
		m_SkyBoxPipeline->BindPipeline();
		m_SkyBoxPipeline->BindDescriptor(frameIndex, 1, 0);
		m_SkyBoxPipeline->UpdateUBO(&skyboxubo, sizeof(SkyBoxUBO),2, frameIndex);
		m_SkyBoxPipeline->BindVertexBuffer(*m_CubeMapVertexBuffer);
		m_SkyBoxPipeline->BindIndexBuffer(*m_CubeMapIndexBuffer);
		m_SkyBoxPipeline->DrawIndexed(*m_CubeMapVertexBuffer, *m_CubeMapIndexBuffer, frameIndex);
		
	}

	DLL_API void SkyBoxRenderer::ShutDown()
	{
		m_SkyBoxPipeline->ShutDown();
		m_SkyBoxTexture.Destroy();
		m_CubeMapVertexBuffer->DestroyBuffer();
		m_CubeMapIndexBuffer->DestroyBuffer();
	}

}