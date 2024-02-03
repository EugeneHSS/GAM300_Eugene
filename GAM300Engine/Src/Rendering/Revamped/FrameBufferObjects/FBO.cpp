#include "Rendering/Revamped/FrameBufferObjects/FBO.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/vulkanInstance.h"
namespace TDS
{
	void FrameBufferObject::BeginRenderPass(VkCommandBuffer commandBuffer)
	{
		m_VKRenderPass->beginRenderPass(commandBuffer, m_FrameBufferHdl);
	}
	void FrameBufferObject::EndRenderPass(VkCommandBuffer commandBuffer)
	{
		m_VKRenderPass->endRenderPass(commandBuffer);
	}

	void FrameBufferObject::SetClearColour(const Vec4& color)
	{
		if (m_VKRenderPass)
		{
			auto& ClearValues = m_VKRenderPass->getClearValues();
			for (auto clearVal : ClearValues)
				clearVal.color = { {color.x, color.y, color.z, color.w} };
		}
	}
	void FrameBufferObject::Destroy()
	{
		if (DestroyFunc)
			DestroyFunc();

	}
	VkSampleCountFlagBits FrameBufferObject::QueryMaxSamplingCnt()
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
}