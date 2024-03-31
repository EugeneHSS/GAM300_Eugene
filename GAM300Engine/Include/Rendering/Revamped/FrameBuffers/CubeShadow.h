#pragma once
#include "FrameBufferObject.h"
namespace TDS
{

	struct AttachmentImage
	{
		VkImage							m_Image = nullptr;
		VkImageView						m_DepthView = nullptr;
		VmaAllocation					m_Allocation = nullptr;
	};

	class VulkanTexture;

	class CubeShadow : public FBO
	{
	private:
		VkFormat							m_ValidDepth{};
		VkRenderPass						m_Renderpass = nullptr;
		std::shared_ptr<VulkanTexture>		m_CubeMapImage = nullptr;
		AttachmentImage						m_DepthImage{};
		std::array<VkFramebuffer, 6>		m_CubeMapFrameBuffers{};


		void	PrepareRenderPass(VkRenderPass& renderpass);
		void	PrepareCubeMapTexture();
	public:
		CubeShadow(Vec2 DimensionInput);

		bool	Create();
		bool	Resize(std::uint32_t width, std::uint32_t height);
		void	Destroy();

		CREATE_GETTOR(DepthImage, AttachmentImage)

		void	GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& createEntry);
		virtual std::shared_ptr<VulkanTexture>				GetImageTexture() override;

	};

}