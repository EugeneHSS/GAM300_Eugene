/*!*************************************************************************
****
\file Renderer.h
\author Ng Zuo Xian Amadeus
\par DP email: ng.z@digipen.edu
\par Course: CSD3400
\par Section: a
\date 22-9-2023
\brief  Contains the Renderer Class and its member functions
****************************************************************************
***/
#ifndef TDS_VULKAN_RENDERER
#define TDS_VULKAN_RENDERER

#include "vulkanInstance.h"
#include "vulkanSwapChain.h"
#include "windowswindow.h"

namespace TDS {
	class DLL_API Renderer {
	public:
		//Constructor and Destructor
		Renderer(WindowsWin& window, VulkanInstance& instance);
		~Renderer();

		//no copy constructor
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		void						ShutDown();
		void						RegisterEvent();

		//Getters
		VkRenderPass				getSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
		float						getAspectRatio() const { return m_SwapChain->extentAspectRatio(); }
		bool						isFrameInProgress() const { return m_isFrameStarted; }
		VkImageView					getImageView(uint32_t val)const { return m_SwapChain->getImageView(val); }
		std::vector	<VkImageView>	getImageViewContainer()const { return m_SwapChain->getImageViewContainer(); }
		VkCommandBuffer				getCurrentCommandBuffer() const {
			assert(m_isFrameStarted && "Cannot get commandbuffer when frame not in progress");
			return m_vCommandBuffers[m_currentFrameIndex];
		}

		int							getFrameIndex() const 
		{
			assert(m_isFrameStarted && "Cannot get Frame index when frame not in progress");
			return m_currentFrameIndex;
		}

		int							GetCurrentImage() const
		{
			return m_currentImageIndex;
		}

		//these functions tell tell when to begin and end the frame and renderpass
		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandbuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandbuffer);
		VulkanSwapChain& getSwapchain();
		//helper functions
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
	private:


		WindowsWin& m_Window;
		VulkanInstance& m_Instance;
		std::unique_ptr<VulkanSwapChain>	m_SwapChain;
		std::vector<VkCommandBuffer>		m_vCommandBuffers;

		uint32_t							m_currentImageIndex;
		int									m_currentFrameIndex{ 0 };
		bool								m_isFrameStarted{ false };
	};
}

#endif //!TDS_VULKAN_RENDERER