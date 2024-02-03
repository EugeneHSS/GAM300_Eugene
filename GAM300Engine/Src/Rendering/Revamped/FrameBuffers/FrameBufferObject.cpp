#include "Rendering/Revamped/FrameBuffers/FrameBufferObject.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/vulkanInstance.h"
#include "Rendering/renderPass.h"
#include "vulkanTools/FrameBuffer.h"

namespace TDS
{
	FBO::~FBO()
	{
	}
	void FBO::SetClearColour(const Vec4& color)
	{
		for (auto& clearAttachment : m_ClearAttachments)
		{
			clearAttachment.clearValue = { { color.x,  color.y,  color.z,  color.w } };
		}
	}
	void FBO::BeginRenderPass(VkCommandBuffer commandBuffer)
	{
		auto& clearVal = m_VKRenderPass->getClearValues();

		for (size_t i = 0; i < m_ClearAttachments.size(); ++i)
		{
			std::uint32_t index = m_ClearAttachments[i].colorAttachment;
			clearVal[index] = m_ClearAttachments[i].clearValue;
		}
		m_VKRenderPass->beginRenderPass(commandBuffer, m_FrameBufferHdl);
	}
	void FBO::EndRenderPass(VkCommandBuffer commandBuffer)
	{
		m_VKRenderPass->endRenderPass(commandBuffer);
	}

	VkSampleCountFlagBits FBO::QueryMaxSamplingCnt()
	{
		VkSampleCountFlagBits sampleCount;
		GraphicsManager& mgr = GraphicsManager::getInstance();
		VkSampleCountFlags count = mgr.getVkInstance().GetDeviceProperties().limits.framebufferColorSampleCounts & mgr.getVkInstance().GetDeviceProperties().limits.framebufferDepthSampleCounts;

		if (count & VK_SAMPLE_COUNT_64_BIT)
			sampleCount = VK_SAMPLE_COUNT_64_BIT;
		else if (count & VK_SAMPLE_COUNT_32_BIT)
			sampleCount = VK_SAMPLE_COUNT_32_BIT;
		else if (count & VK_SAMPLE_COUNT_16_BIT)
			sampleCount = VK_SAMPLE_COUNT_16_BIT;
		else if (count & VK_SAMPLE_COUNT_8_BIT)
			sampleCount = VK_SAMPLE_COUNT_8_BIT;
		else if (count & VK_SAMPLE_COUNT_4_BIT)
			sampleCount = VK_SAMPLE_COUNT_4_BIT;
		else if (count & VK_SAMPLE_COUNT_2_BIT)
			sampleCount = VK_SAMPLE_COUNT_2_BIT;
		else
			sampleCount = VK_SAMPLE_COUNT_1_BIT;

		mgr.getVkInstance().GetDeviceFeatures().sampleRateShading = VK_TRUE;
		return sampleCount;
	}
	void FBO::SetDepthTarget(RenderTarget*& depthTarget)
	{
		m_DepthTarget = depthTarget;
	}
	void FBO::SetResizeable(bool condition)
	{
		m_Resizeable = condition;
	}
	void FBO::SetAutoSync(bool condition)
	{
		m_AutoSync = condition;
	}
	void FBO::SetTargetSwapchain(bool condition)
	{
		m_TargetSwapchain = condition;
	}
	void FBO::SetSampleCount(VkSampleCountFlagBits bits)
	{
		m_SampleCnt = bits;
	}
	bool FBO::GetResizeable()
	{
		return m_Resizeable;
	}
	bool FBO::GetAutoSync()
	{
		return m_AutoSync;
	}
	bool FBO::GetTargetSwapchain()
	{
		return m_TargetSwapchain;
	}
	Vec2 FBO::GetFrameBufferDimension()
	{
		return m_FrameBufferDimension;
	}
	VkSampleCountFlagBits FBO::GetSampleCount()
	{
		return m_SampleCnt;
	}
	RenderTarget*& FBO::GetDepthTarget()
	{
		return m_DepthTarget;
	}
	std::vector<RenderTarget*>& FBO::GetTargets()
	{
		return m_RenderTargets;
	}
	std::vector<RenderTargetCI>& FBO::GetRenderAttachmentInfo()
	{
		return m_RenderAttachmentInfo;
	}
	FrameBuffer* FBO::GetFrameBufferHandle()
	{
		return m_FrameBufferHdl;
	}
	RenderPass* FBO::GetRenderPass()
	{
		return m_VKRenderPass;
	}
	std::vector<VkClearAttachment>& FBO::GetClearAttachments()
	{
		return m_ClearAttachments;
	}
}