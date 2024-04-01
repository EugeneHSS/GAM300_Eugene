#include "Rendering/Revamped/FrameBuffers/CubeShadow.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/VulkanTexture.h"

namespace TDS
{
	void CubeShadow::PrepareRenderPass(VkRenderPass& renderpass)
	{
		GraphicsManager& mgr = GraphicsManager::getInstance();

		std::array<VkAttachmentDescription, 2> offScrrenAttachmentDesc;

		offScrrenAttachmentDesc[0].format = VK_FORMAT_R32_SFLOAT;
		offScrrenAttachmentDesc[0].samples = VK_SAMPLE_COUNT_1_BIT;
		offScrrenAttachmentDesc[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		offScrrenAttachmentDesc[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		offScrrenAttachmentDesc[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		offScrrenAttachmentDesc[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		offScrrenAttachmentDesc[0].initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		offScrrenAttachmentDesc[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		offScrrenAttachmentDesc[1].format = m_ValidDepth;
		offScrrenAttachmentDesc[1].samples = VK_SAMPLE_COUNT_1_BIT;
		offScrrenAttachmentDesc[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		offScrrenAttachmentDesc[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		offScrrenAttachmentDesc[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		offScrrenAttachmentDesc[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		offScrrenAttachmentDesc[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		offScrrenAttachmentDesc[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference{};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorReference;
		subpass.pDepthStencilAttachment = &depthReference;

		VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassCreateInfo.attachmentCount = std::uint32_t(offScrrenAttachmentDesc.size());
		renderPassCreateInfo.pAttachments = offScrrenAttachmentDesc.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;

		VK_ASSERT(vkCreateRenderPass(mgr.getVkInstance().getVkLogicalDevice(), &renderPassCreateInfo, 0, &m_Renderpass), "Failed to create omni renderpass");
	}
	void CubeShadow::PrepareCubeMapTexture()
	{
		m_CubeMapImage = VulkanTexture::CreateShadowCubeMapTexture((std::uint32_t)m_FrameBufferDimension.x, (std::uint32_t)m_FrameBufferDimension.y, VK_FORMAT_R32_SFLOAT);
	}
	CubeShadow::CubeShadow(Vec2 DimensionInput)
	{
		m_Resizeable = true;
		m_AutoSync = true;
		m_TargetSwapchain = false;
		m_SampleCnt = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		m_FrameBufferDimension = DimensionInput;
		m_FBOType = FBO_TYPE::OMNI_SHADOW;

		GraphicsManager& mgr = GraphicsManager::getInstance();

		m_ValidDepth = mgr.getVkInstance().CheckForValidDepthStencil({ VK_FORMAT_D32_SFLOAT,
			 VK_FORMAT_D32_SFLOAT_S8_UINT,
			 VK_FORMAT_D24_UNORM_S8_UINT,
			 VK_FORMAT_D16_UNORM_S8_UINT,
			 VK_FORMAT_D16_UNORM });
	}
	bool CubeShadow::Create()
	{
		GraphicsManager& mgr = GraphicsManager::getInstance();
		CommandManager& cmdMgr = mgr.getCommandManager();

		VkImageCreateInfo imageCI = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageCI.imageType = VK_IMAGE_TYPE_2D;
		imageCI.format = VK_FORMAT_R32_SFLOAT;
		imageCI.extent = VkExtent3D{ std::uint32_t(m_FrameBufferDimension.x), std::uint32_t(m_FrameBufferDimension.y), 1 };
		imageCI.mipLevels = 1;
		imageCI.arrayLayers = 1;
		imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;

		imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkImageViewCreateInfo depthViewCI = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		depthViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthViewCI.format = m_ValidDepth;

		depthViewCI.flags = 0;
		depthViewCI.subresourceRange = {};
		depthViewCI.subresourceRange.aspectMask =
			(depthViewCI.format >= VK_FORMAT_D16_UNORM_S8_UINT) ?
			(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : (VK_IMAGE_ASPECT_DEPTH_BIT);

		depthViewCI.subresourceRange.baseMipLevel = 0;
		depthViewCI.subresourceRange.levelCount = 1;
		depthViewCI.subresourceRange.baseArrayLayer = 0;
		depthViewCI.subresourceRange.layerCount = 1;

		m_DepthImage.m_Allocation = GraphicsAllocator::Allocate(&imageCI, VMA_MEMORY_USAGE_GPU_ONLY, m_DepthImage.m_Image);

		CommandBufferInfo cmdBufferInfo{};
		cmdMgr.CreateSingleUseCommandBuffer(cmdBufferInfo);
		{
			VulkanTexture::setImageLayout(cmdBufferInfo.m_CommandBuffer.m_CmdBuffer, m_DepthImage.m_Image, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		}
		cmdMgr.EndExecution(cmdBufferInfo);
		depthViewCI.image = m_DepthImage.m_Image;
		VK_ASSERT(vkCreateImageView(mgr.getVkInstance().getVkLogicalDevice(), &depthViewCI, 0, &m_DepthImage.m_DepthView), "Failed to create OmniShadow depth view");

		std::vector<VkImageView> attachments(2);
		attachments[1] = m_DepthImage.m_DepthView;

		PrepareRenderPass(m_Renderpass);

		VkFramebufferCreateInfo frameBufferCI = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		frameBufferCI.renderPass = m_Renderpass;
		frameBufferCI.attachmentCount = 2;
		frameBufferCI.pAttachments = attachments.data();
		frameBufferCI.width = std::uint32_t(m_FrameBufferDimension.x);
		frameBufferCI.height = std::uint32_t(m_FrameBufferDimension.y);
		frameBufferCI.layers = 1;
		std::vector<VkImageView>& shadowMapImageViews = m_CubeMapImage->GetImageViews();
		for (unsigned int i = 0; i < 6; ++i)
		{
			attachments[0] = shadowMapImageViews[i];
			VK_ASSERT(vkCreateFramebuffer(mgr.getVkInstance().getVkLogicalDevice(), &frameBufferCI, nullptr, &m_CubeMapFrameBuffers[i]), "Failed to create each shadow layer");
		}
		return true;
	}
	bool CubeShadow::Resize(std::uint32_t width, std::uint32_t height)
	{
		Destroy();
		PrepareRenderPass(m_Renderpass);
		m_FrameBufferDimension = Vec2(float(width), float(height));
		return Create();
	}
	
	void CubeShadow::Destroy()
	{
		GraphicsManager& mgr = GraphicsManager::getInstance();

		for (auto& fb : m_CubeMapFrameBuffers)
		{
			vkDestroyFramebuffer(mgr.getVkInstance().getVkLogicalDevice(), fb, nullptr);
			fb = nullptr;
		}

		if (m_Renderpass)
		{
			vkDestroyRenderPass(mgr.getVkInstance().getVkLogicalDevice(), m_Renderpass, 0);
			m_Renderpass = nullptr;
		}


		if (m_DepthImage.m_DepthView)
		{
			vkDestroyImageView(mgr.getVkInstance().getVkLogicalDevice(), m_DepthImage.m_DepthView, nullptr);
			m_DepthImage.m_DepthView = nullptr;
		}

		if (m_DepthImage.m_Image)
		{
			GraphicsAllocator::FreeBuffer(m_DepthImage.m_Image, m_DepthImage.m_Allocation);
			m_DepthImage.m_Image = nullptr;
			m_DepthImage.m_Allocation = nullptr;
		}

		if (m_CubeMapImage)
		{
			m_CubeMapImage->Destroy();
			m_CubeMapImage = nullptr;
		}
	}


	void CubeShadow::GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& createEntry)
	{
		blendCont.resize(1);

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
	}
	std::shared_ptr<VulkanTexture> CubeShadow::GetImageTexture()
	{
		return m_CubeMapImage;
	}
}