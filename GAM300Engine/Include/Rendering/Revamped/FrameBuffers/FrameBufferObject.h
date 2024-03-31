#pragma once
#include "vulkanTools/VulkanHelper.h"
#include "Rendering/RenderTarget.h"

namespace TDS
{
	enum class FBO_TYPE
	{
		G_BUFFER = 0,
		POINT_LIGHT,
		SPOT_LIGHT,
		SHADOW,
		OMNI_SHADOW,
		COMPOSITION,
	};

	struct FrameBufferObjectCreateInfo
	{
		bool					m_Resizeable;
		bool					m_AutoSync;
		bool					m_TargetSwapchain;
		Vec2					m_FrameBufferDimension;
		VkSampleCountFlagBits	m_SampleCnt;
	};
	struct PipelineCreateEntry;

	class FrameBuffer;
	class VulkanTexture;
	class RenderPass;

	class FBO
	{
	public:
		using BlendContainer = std::vector<VkPipelineColorBlendAttachmentState>;
	protected:
		bool												m_Resizeable = true;
		bool												m_AutoSync = true;
		bool												m_TargetSwapchain = false;
		Vec2												m_FrameBufferDimension = { 0.f,0.f };
		VkSampleCountFlagBits								m_SampleCnt = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		FrameBuffer* m_FrameBufferHdl = nullptr;
		RenderPass* m_VKRenderPass = nullptr;
		RenderTarget* m_DepthTarget = nullptr;

		std::vector<RenderTarget*>							m_RenderTargets{};
		std::vector<RenderTargetCI>							m_RenderAttachmentInfo{};
		std::vector<VkClearAttachment>						m_ClearAttachments;
		FBO_TYPE											m_FBOType = FBO_TYPE::G_BUFFER;

	public:
		FBO() = default;
		virtual												~FBO();

		virtual bool										Create() = 0;
		virtual	bool										Resize(std::uint32_t width, std::uint32_t height) = 0;
		virtual void										SetClearColour(const Vec4& color);
		virtual void										Destroy() = 0;

		virtual void										GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& pipelineEntry) = 0;
		virtual void										BeginRenderPass(VkCommandBuffer commandBuffer);
		virtual void										EndRenderPass(VkCommandBuffer commandBuffer);
		VkSampleCountFlagBits								QueryMaxSamplingCnt();

		virtual std::shared_ptr<VulkanTexture>				GetImageTexture() { return nullptr; }
		void												SetDepthTarget(RenderTarget*& depthTarget);
		void												SetResizeable(bool condition);
		void												SetAutoSync(bool condition);
		void												SetTargetSwapchain(bool condition);
		void												SetSampleCount(VkSampleCountFlagBits bits);
		bool												GetResizeable();
		bool												GetAutoSync();
		bool												GetTargetSwapchain();

		Vec2												GetFrameBufferDimension();
		VkSampleCountFlagBits								GetSampleCount();
		RenderTarget*& GetDepthTarget();
		std::vector<RenderTarget*>& GetTargets();
		std::vector<RenderTargetCI>& GetRenderAttachmentInfo();
		FrameBuffer* GetFrameBufferHandle();
		RenderPass* GetRenderPass();
		std::vector<VkClearAttachment>& GetClearAttachments();



	};
}