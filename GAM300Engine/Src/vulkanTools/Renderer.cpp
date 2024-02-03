/*!*************************************************************************
****
\file Renderer.cpp
\author Ng Zuo Xian Amadeus
\par DP email: ng.z@digipen.edu
\par Course: CSD3400
\par Section: a
\date 22-9-2023
\brief  Function definitions of the Renderer Class
****************************************************************************
***/
#include <vulkanTools/Renderer.h>
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/CommandManager.h"
#include "Rendering/GraphicsManager.h"
#include "MessagingSystem/MessageSystem.h"
namespace TDS {

	//Renderer class constructor
	Renderer::Renderer(WindowsWin& window, VulkanInstance& instance) : m_Window(window), m_Instance(instance)
	{
		recreateSwapChain();
		createCommandBuffers();
		RegisterEvent();
		
	}

	//Renderer class destructor
	Renderer::~Renderer()
	{
		//ShutDown();
	}

	void Renderer::ShutDown()
	{
		freeCommandBuffers();
		m_SwapChain->ShutDown();
	}

	void Renderer::RegisterEvent()
	{
		REGISTER_MESSAGE("Resize Swapchain", this, &Renderer::recreateSwapChain);
	}

	//recreates the data from using the previous swapchain
	void Renderer::recreateSwapChain() {
		vkDeviceWaitIdle(m_Instance.getVkLogicalDevice());

		VkExtent2D swapchainextent;
		if (m_Window.getHeight() == 0 || m_Window.getWidth() == 0)
		{
			swapchainextent.width = m_SwapChain->getSwapChainExtent().width;
			swapchainextent.height = m_SwapChain->getSwapChainExtent().height;
		}
		else
		{
			swapchainextent.width = m_Window.getWidth();
			swapchainextent.height = m_Window.getHeight();
		}
		if (m_SwapChain == nullptr)
			m_SwapChain = std::make_unique<VulkanSwapChain>(m_Instance, swapchainextent);
		else {
			CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();
			std::shared_ptr<VulkanSwapChain> oldswapchain = std::move(m_SwapChain);
			m_SwapChain = std::make_unique<VulkanSwapChain>(m_Instance, swapchainextent, oldswapchain);

			if (!oldswapchain->compareSwapFormat(*m_SwapChain.get()))
				throw std::runtime_error("Swap Chain image/depth format has changed");
			cmdMgr.ResetPool(POOLTYPE::GRAPHICS);
			oldswapchain->ShutDown();
		}
	}

	//create commandbuffer for Rendering
	void Renderer::createCommandBuffers() {
		m_vCommandBuffers.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();
		if (cmdMgr.CreateCommandBatch(m_vCommandBuffers, POOLTYPE::GRAPHICS) == false)
			throw std::runtime_error("failed to allocate command buffers");

		//VkCommandBufferAllocateInfo AllocInfo{};
		//AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		//AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		//AllocInfo.commandPool = m_Instance.getCommandPool();
		//AllocInfo.commandBufferCount = static_cast<uint32_t>(m_vCommandBuffers.size());

		//if (vkAllocateCommandBuffers(m_Instance.getVkLogicalDevice(), &AllocInfo, m_vCommandBuffers.data()) != VK_SUCCESS)
		//	throw std::runtime_error("failed to allocate command buffers");
	}

	//free the command buffers
	void Renderer::freeCommandBuffers()
	{
		CommandManager& cmdMgr = GraphicsManager::getInstance().getCommandManager();
		cmdMgr.DestroyBatch(m_vCommandBuffers, POOLTYPE::GRAPHICS);
		/*	vkFreeCommandBuffers(m_Instance.getVkLogicalDevice(), m_Instance.getCommandPool(), static_cast<uint32_t>(m_vCommandBuffers.size()), m_vCommandBuffers.data());*/
		m_vCommandBuffers.clear();
	}

	//Start recording of the frame
	VkCommandBuffer Renderer::BeginFrame() {
		assert(!m_isFrameStarted && "cant call beginframe while already in progress");

		auto result = m_SwapChain->acquireNextImage(&m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire swap chain image");

		m_isFrameStarted = true;

		auto commandbuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandbuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer");
		return commandbuffer;
	}

	//end recording of the frame
	void Renderer::EndFrame() {
		assert(m_isFrameStarted && "cant call endframe while frame is not in progress");
		auto commandbuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandbuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");

		auto result = m_SwapChain->SubmitCommandBuffers(&commandbuffer, &m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized()) {
			m_Window.resetResizeFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to present swap chain image");

		m_isFrameStarted = false;
		m_currentFrameIndex = (m_currentFrameIndex + 1) % VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	//start the renderpass of the swapchain
	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandbuffer) {
		assert(m_isFrameStarted && "cant call beginswapchainrenderpass if frame is not in progress");
		assert(commandbuffer == getCurrentCommandBuffer() && "cant begin renderpass on commandbuffer from different frame");

		VkRenderPassBeginInfo renderpassinfo{};
		renderpassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpassinfo.renderPass = m_SwapChain->getRenderPass();
		renderpassinfo.framebuffer = m_SwapChain->getFrameBuffer(m_currentImageIndex);

		renderpassinfo.renderArea.offset = { 0,0 };
		renderpassinfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f,0.01f,0.01f,1.f };
		clearValues[1].depthStencil = { 1.f,0 };
		renderpassinfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderpassinfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandbuffer, &renderpassinfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		VkRect2D scissor{ {0,0}, m_SwapChain->getSwapChainExtent() };

		vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandbuffer, 0, 1, &scissor);
	}

	//stop the renderpass of the swapchain
	void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandbuffer) {
		assert(m_isFrameStarted && "cant call endswapchainrenderpass if frame is not in progress");
		assert(commandbuffer == getCurrentCommandBuffer() && "cant end renderpass on commandbuffer from a different frame");
		vkCmdEndRenderPass(commandbuffer);
	}
	VulkanSwapChain& Renderer::getSwapchain()
	{
		return *m_SwapChain;
	}
}