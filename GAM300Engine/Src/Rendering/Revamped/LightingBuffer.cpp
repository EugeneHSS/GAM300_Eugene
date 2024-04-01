#include "Rendering/Revamped/FrameBuffers/LightingBuffer.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/VulkanPipeline.h"

namespace TDS
{
	LightingBuffer::LightingBuffer(Vec2 DimensionInput)
	{
		m_Resizeable = true;
		m_AutoSync = true;
		m_TargetSwapchain = false;
		m_SampleCnt = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		m_FrameBufferDimension = DimensionInput;
		m_FBOType = FBO_TYPE::G_BUFFER;
		Vec3 Dim = Vec3(DimensionInput.x, DimensionInput.y, 1.f);
		m_RenderAttachmentInfo.emplace_back(
			RenderTargetCI(VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_VIEW_TYPE_2D,
				Dim,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
				VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				RenderTargetType::COLOR,
				false,
				VK_SAMPLE_COUNT_1_BIT)

		);

		m_RenderAttachmentInfo.emplace_back(
			RenderTargetCI(VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_VIEW_TYPE_2D,
				Dim,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
				VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				RenderTargetType::COLOR,
				false,
				VK_SAMPLE_COUNT_1_BIT)

		);


	}
	LightingBuffer::~LightingBuffer()
	{
	}
	bool LightingBuffer::Create()
	{
		if (m_RenderAttachmentInfo.size() > 1 && m_TargetSwapchain
			|| m_RenderAttachmentInfo.empty())
			return false;

		GraphicsManager& mgr = GraphicsManager::getInstance();

		std::shared_ptr<VulkanInstance> instance = mgr.getVkInstancePtr();
		m_RenderTargets.resize(m_RenderAttachmentInfo.size());
		std::vector<AttachmentInfo> attachmentInfos{};


		std::uint32_t attachmentCnt = (std::uint32_t)m_RenderAttachmentInfo.size();
		std::uint32_t totalBufferSize = attachmentCnt + 1;
		attachmentInfos.resize(totalBufferSize);
		m_ClearAttachments.resize(totalBufferSize);
		unsigned i = 0;
		for (; i < attachmentCnt; ++i)
		{
			m_RenderTargets[i] = new RenderTarget(instance, m_RenderAttachmentInfo[i]);
			attachmentInfos[i].framebufferAttachment = m_RenderTargets[i];
			attachmentInfos[i].loadOP = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachmentInfos[i].storeOP = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentInfos[i].stencilLoadOP = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentInfos[i].stencilStoreOP = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		VkFormat depthFormat = mgr.getVkInstance().CheckForValidDepthStencil(
			{ VK_FORMAT_D32_SFLOAT,
			 VK_FORMAT_D32_SFLOAT_S8_UINT,
			 VK_FORMAT_D24_UNORM_S8_UINT,
			 VK_FORMAT_D16_UNORM_S8_UINT,
			 VK_FORMAT_D16_UNORM }
		);


		RenderTargetCI depthInfo = RenderTargetCI(depthFormat,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			VK_IMAGE_VIEW_TYPE_2D,
			Vec3(m_FrameBufferDimension.x, m_FrameBufferDimension.y, 1.f),
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			RenderTargetType::DEPTH,
			false,
			VK_SAMPLE_COUNT_1_BIT);

		if (m_DepthTarget == nullptr)
			m_DepthTarget = new RenderTarget(instance, depthInfo);

		attachmentInfos[i].framebufferAttachment = m_DepthTarget;
		attachmentInfos[i].loadOP = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentInfos[i].storeOP = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentInfos[i].stencilLoadOP = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentInfos[i].stencilStoreOP = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		m_RenderTargets.emplace_back(m_DepthTarget);


		m_VKRenderPass = new RenderPass(instance->getVkLogicalDevice(), attachmentInfos);
		m_FrameBufferHdl = new FrameBuffer(instance->getVkLogicalDevice(), m_VKRenderPass->getRenderPass(), m_RenderTargets);

		//Do depth first
		auto& clearValues = m_VKRenderPass->getClearValues();

		m_ClearAttachments[i].clearValue = clearValues[i];
		m_ClearAttachments[i].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

		//Go thru all color
		for (size_t j = 0; j < attachmentCnt; ++j)
		{
			m_ClearAttachments[j].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			m_ClearAttachments[j].clearValue = clearValues[j];
			m_ClearAttachments[j].colorAttachment = j;
		}

		return true;
	}
	bool LightingBuffer::Resize(std::uint32_t width, std::uint32_t height)
	{
		m_FrameBufferHdl->resize(Vec3(width, height, 1.f), m_VKRenderPass->getRenderPass());
		return true;
	}
	void LightingBuffer::Destroy()
	{
		if (m_VKRenderPass)
			delete m_VKRenderPass;
		if (m_FrameBufferHdl)
			delete m_FrameBufferHdl;

		delete m_RenderTargets[0];

	}
	void LightingBuffer::GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& createEntry)
	{
		blendCont.resize(2);

		if (createEntry.m_PipelineConfig.m_SrcClrBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			createEntry.m_PipelineConfig.m_DstClrBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			createEntry.m_PipelineConfig.m_SrcAlphaBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			createEntry.m_PipelineConfig.m_DstAlphaBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO)
			blendCont[0].blendEnable = VK_TRUE;
		else
			blendCont[0].blendEnable = VK_FALSE;

		blendCont[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendCont[0].srcColorBlendFactor = createEntry.m_PipelineConfig.m_SrcClrBlend;
		blendCont[0].dstColorBlendFactor = createEntry.m_PipelineConfig.m_DstClrBlend;
		blendCont[0].colorBlendOp = createEntry.m_PipelineConfig.m_ColorBlend;
		blendCont[0].srcAlphaBlendFactor = createEntry.m_PipelineConfig.m_SrcAlphaBlend;
		blendCont[0].dstAlphaBlendFactor = createEntry.m_PipelineConfig.m_DstAlphaBlend;
		blendCont[0].alphaBlendOp = createEntry.m_PipelineConfig.m_AlphaBlend;


		blendCont[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendCont[1].blendEnable = VK_FALSE;
		blendCont[1].srcColorBlendFactor = createEntry.m_PipelineConfig.m_SrcClrBlend;
		blendCont[1].dstColorBlendFactor = createEntry.m_PipelineConfig.m_DstClrBlend;
		blendCont[1].colorBlendOp = createEntry.m_PipelineConfig.m_ColorBlend;
		blendCont[1].srcAlphaBlendFactor = createEntry.m_PipelineConfig.m_SrcAlphaBlend;
		blendCont[1].dstAlphaBlendFactor = createEntry.m_PipelineConfig.m_DstAlphaBlend;
		blendCont[1].alphaBlendOp = createEntry.m_PipelineConfig.m_AlphaBlend;

	}
}
