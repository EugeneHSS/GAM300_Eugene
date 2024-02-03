#ifndef RENDERPASS
#define RENDERPASS

#include "vulkanTools/FrameBuffer.h"
#include "Rendering/RenderTarget.h"
#include "dotnet/ImportExport.h"
namespace TDS {
	
	struct AttachmentClearColor
	{
		VkClearColorValue color{ {0.0f,0.0f,0.0f,1.f} };
		VkClearDepthStencilValue depth{ 1.f, 0 };
	};
	struct AttachmentInfo
	{
		RenderTarget* framebufferAttachment;
		VkAttachmentLoadOp loadOP;
		VkAttachmentStoreOp storeOP;
		VkAttachmentLoadOp stencilLoadOP;
		VkAttachmentStoreOp stencilStoreOP;
		AttachmentClearColor clear = AttachmentClearColor{};
	};


	class RenderPass
	{
	public:
		DLL_API RenderPass(VkDevice device, const std::vector<AttachmentInfo>& attachments);
		DLL_API ~RenderPass();
		//no move and copy ctor
		RenderPass(const RenderPass&) = delete;
		RenderPass& operator=(const RenderPass&) = delete;
		RenderPass(RenderPass&&) = delete;
		RenderPass& operator=(RenderPass&&) = delete;

		DLL_API void beginRenderPass(VkCommandBuffer commandBuffer, FrameBuffer* framebuffer, uint32_t viewports = 1);
		DLL_API void endRenderPass(VkCommandBuffer commandBuffer);

		DLL_API void setViewportSize(uint32_t w, uint32_t h) { m_Width = w; m_Height = h; }

		DLL_API VkRenderPass& getRenderPass() { return m_RenderPass; }

		DLL_API std::vector<VkClearValue>& getClearValues() { return m_clearValues; }
	private:
		VkRenderPass m_RenderPass{};
		uint32_t m_Width{};
		uint32_t m_Height{};

		const VkDevice& m_Device;

		std::vector<VkClearValue> m_clearValues{};
	};




}//END namespace



#endif // !RENDERPASS

