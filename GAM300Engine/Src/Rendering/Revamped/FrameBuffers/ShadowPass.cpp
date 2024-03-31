#include "Rendering/Revamped/FrameBuffers/ShadowPass.h"
#include "Rendering/GraphicsManager.h"
namespace TDS
{
	ShadowPass::ShadowPass(Vec2 DimensionInput)
	{
		m_Resizeable = true;
		m_AutoSync = true;
		m_TargetSwapchain = false;
		m_SampleCnt = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		m_FrameBufferDimension = DimensionInput;
		m_FBOType = FBO_TYPE::SHADOW;


		m_ShadowDepth = RenderTargetCI(VK_FORMAT_D32_SFLOAT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			VK_IMAGE_VIEW_TYPE_2D,
			m_FrameBufferDimension,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			RenderTargetType::DEPTH,
			false,
			VK_SAMPLE_COUNT_1_BIT);
	}
	bool ShadowPass::Create()
	{
		GraphicsManager& mgr = GraphicsManager::getInstance();
		std::shared_ptr<VulkanInstance> instance = mgr.getVkInstancePtr();

		m_DepthTarget = new RenderTarget(instance, m_ShadowDepth);

		std::vector<AttachmentInfo> attachInfo;
		auto& depthAttach = attachInfo.emplace_back();
		depthAttach.loadOP = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttach.framebufferAttachment = m_DepthTarget;
		depthAttach.storeOP = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttach.stencilLoadOP = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttach.stencilStoreOP = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		m_RenderTargets.emplace_back(m_DepthTarget);

		m_VKRenderPass = new RenderPass(instance->getVkLogicalDevice(), attachInfo);
		m_FrameBufferHdl = new FrameBuffer(instance->getVkLogicalDevice(), m_VKRenderPass->getRenderPass(), m_RenderTargets);

		return true;
	}
	bool ShadowPass::Resize(std::uint32_t width, std::uint32_t height)
	{
		m_FrameBufferHdl->resize(Vec3(width, height, 1.f), m_VKRenderPass->getRenderPass());
		return true;
	}

	void ShadowPass::Destroy()
	{
		if (m_VKRenderPass)
			delete m_VKRenderPass;
		if (m_FrameBufferHdl)
			delete m_FrameBufferHdl;
		if (m_DepthTarget)
			delete m_DepthTarget;
		for (auto& target : m_RenderTargets)
		{
			if (target)
				delete target;
		}
	}
	void ShadowPass::GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& createEntry)
	{
		if (createEntry.m_PipelineConfig.m_SrcClrBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			createEntry.m_PipelineConfig.m_DstClrBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			createEntry.m_PipelineConfig.m_SrcAlphaBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			createEntry.m_PipelineConfig.m_DstAlphaBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO)
			blendCont[0].blendEnable = VK_TRUE;
		else
			blendCont[0].blendEnable = VK_FALSE;

		blendCont[0].colorWriteMask = 0xf;
		blendCont[0].srcColorBlendFactor = createEntry.m_PipelineConfig.m_SrcClrBlend;
		blendCont[0].dstColorBlendFactor = createEntry.m_PipelineConfig.m_DstClrBlend;
		blendCont[0].colorBlendOp = createEntry.m_PipelineConfig.m_ColorBlend;
		blendCont[0].srcAlphaBlendFactor = createEntry.m_PipelineConfig.m_SrcAlphaBlend;
		blendCont[0].dstAlphaBlendFactor = createEntry.m_PipelineConfig.m_DstAlphaBlend;
		blendCont[0].alphaBlendOp = createEntry.m_PipelineConfig.m_AlphaBlend;
	}
}