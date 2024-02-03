#include "Rendering/ObjectPicking.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#include "Rendering/Revamped/DeferredController.h"
#include "Rendering/Revamped/FrameBuffers/G_Buffer.h"
namespace TDS
{
	
	ObjectPick::ObjectPick(std::shared_ptr<VulkanInstance> inst,Vec3 resolution)
	{
		CreatePickObj(inst, resolution);


		//m_DepthSync.addImageBarrier(SYNCATTACHMENT::DEPTH, m_PickDepthAttachment->getImage(), m_PickDepthAttachment->getImageSubResourceRange(), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL);


		m_PickPipeline = std::make_shared<VulkanPipeline>();
		m_PickPipeline->SetRenderTarget(m_PickRenderPass->getRenderPass());
		PipelineCreateEntry ObjectPickEntry{};
		ObjectPickEntry.m_NumDescriptorSets = 1;
		ObjectPickEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		ObjectPickEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		ObjectPickEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ObjectPickEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ObjectPickEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::COMPUTE_SHADER, "../assets/shaders/objectpicking.spv"));


		m_PickBuffer = std::make_shared<VMABuffer>();
		m_PickBuffer->CreateBuffer(sizeof(PickStruct), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU,&pickStr);

		m_PickPipeline->Create(ObjectPickEntry);

	}
	ObjectPick::~ObjectPick()
	{

	}
	void ObjectPick::Shutdown()
	{
		m_PickPipeline->ShutDown();
		m_PickBuffer->DestroyBuffer();
		delete m_PickRenderPass;
		delete m_PickRenderTarget;
		delete m_PickDepthAttachment;
		delete m_PickFrameBuffer;
	}
	void ObjectPick::Update(VkCommandBuffer commandBuffer, uint32_t frameIndex, Vec2 mousePosition)
	{

		auto GBuffer = GraphicsManager::getInstance().GetDeferredController()->GetFrameBuffer(RENDER_PASS::RENDER_G_BUFFER);
		auto pickTarget = GBuffer->GetTargets().at(1);
		Synchronization m_ReadSync2{ SYNCTYPE::GRAPHIC2COMPUTE,VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		m_ReadSync2.addImageBarrier(SYNCATTACHMENT::COLOR, pickTarget->getImage(), pickTarget->getImageSubResourceRange(),
			VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);
		m_ReadSync2.syncBarrier(commandBuffer);
		float mX = GraphicsManager::getInstance().getViewportScreen().x;
		float mY = GraphicsManager::getInstance().getViewportScreen().y;
		float mWidth = GraphicsManager::getInstance().getViewportScreen().z;
		float mHeight = GraphicsManager::getInstance().getViewportScreen().w;

		float xClipped = (mousePosition.x - mX) / (mWidth);
		float yClipped = ((mousePosition.y - mY) - (GraphicsManager::getInstance().getOffset() - mHeight)) / (mHeight);
		 
		IDReadCompute idreadPush{};
		idreadPush.mouseRelativeCoord = { xClipped, yClipped };
		//std::cout << idreadPush.mouseRelativeCoord.y << std::endl;

		pickubo.View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
		pickubo.Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 10.f);
		pickubo.Projection.m[1][1] *= -1;
		

		VkDescriptorImageInfo pickAttachment = pickTarget->getImageInfoDescriptor();
		
		m_PickPipeline->SetCommandBuffer(commandBuffer);
		m_PickPipeline->BindComputePipeline();
		m_PickPipeline->SubmitPushConstant(&idreadPush, sizeof(IDReadCompute), SHADER_FLAG::COMPUTE_SHADER);
		m_PickPipeline->BindDescriptor(frameIndex, 1, 0, true);
		m_PickPipeline->UpdateUBO(&pickStr, sizeof(PickStruct), 9, frameIndex, 0, true);

		m_PickPipeline->UpdateDescriptor(pickAttachment, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
		
		m_PickPipeline->UpdateUBO(&pickubo, sizeof(PickUBO),5, frameIndex);
		
		m_PickPipeline->DispatchCompute(1, 1, 1);
	}
	void ObjectPick::resize(Vec3 newSize)
	{
		m_PickFrameBuffer->resize(newSize, m_PickRenderPass->getRenderPass());
	
	}
	void ObjectPick::beginRenderPass(VkCommandBuffer commandbuffer)
	{
		m_PickRenderPass->beginRenderPass(commandbuffer, m_PickFrameBuffer);
	}
	void ObjectPick::endRenderPass(VkCommandBuffer commandbuffer)
	{
		m_PickRenderPass->endRenderPass(commandbuffer);
	}
	void ObjectPick::CreatePickObj(std::shared_ptr<VulkanInstance> inst,Vec3 resolution)
	{
		m_PickRenderTarget = new RenderTarget(inst,
			{
				VK_FORMAT_R32_UINT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_VIEW_TYPE_2D,
				resolution,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_GENERAL,
				RenderTargetType::COLOR,
				false
			});


		std::vector<AttachmentInfo> attachmentInfos{};

		attachmentInfos.push_back({m_PickRenderTarget, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE });

		m_PickRenderPass = new RenderPass(inst->getVkLogicalDevice(), attachmentInfos);
		m_PickFrameBuffer = new FrameBuffer(inst->getVkLogicalDevice(), m_PickRenderPass->getRenderPass(), { m_PickRenderTarget/*, m_PickDepthAttachment*/ });

	}
}
