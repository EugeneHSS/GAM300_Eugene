#pragma once
#include <pch.h>
#include "dotnet/ImportExport.h"
#include "vulkan/vulkan.h"
#include "camera/camera.h"
#include "vulkanTools/PointLightSystem.h"
#include "DebugRenderer.h"
#include "Rendering/ObjectPicking.h"
#include "camera/Camerasystem/CameraSystem.h"


namespace TDS
{

	class WindowsWin;
	class VulkanInstance;
	class Renderer;
	class CommandManager;
	class RenderLayer;
	class RenderTarget;
	class RenderPass;
	class FrameBuffer;
	class PointLightSystem;
	class DeferredController;
	class Renderer2D;
	class MaterialManager;



	struct FullScreenVertex
	{
		Vec3 m_Position{};
		Vec2 m_UV{};
	};

	class MessageSystem;
	class SkyBoxRenderer;
	class FontRenderer;

	class DLL_API GraphicsManager
	{
	public:
		typedef std::function<void(VkCommandBuffer)> EditorRenderFunc;
		EditorRenderFunc						m_EditorRender = nullptr;
	private:
		std::shared_ptr<VulkanInstance>			m_MainVkContext = nullptr;
		std::shared_ptr<Renderer>				m_SwapchainRenderer = nullptr;
		std::shared_ptr<SkyBoxRenderer>			m_SkyBoxRenderer = nullptr;
		std::shared_ptr<CommandManager>			m_CommandManager = nullptr;
		std::shared_ptr<ObjectPick>				m_ObjectPicking = nullptr;
		std::shared_ptr<Renderer2D>				m_Renderer2D = nullptr;
		std::shared_ptr<FontRenderer>			m_FontRenderer = nullptr;
		std::shared_ptr<MaterialManager>		m_MaterialManager = nullptr;
		std::vector<RenderLayer*>				m_RenderLayer;



		
		TDSCamera* m_Camera = nullptr;
		VkCommandBuffer							currentCommand = nullptr;
		WindowsWin* m_pWindow = nullptr;
		RenderTarget* m_RenderingAttachment{ nullptr };
		RenderTarget* m_RenderingDepthAttachment{ nullptr };
		RenderTarget* m_PickAttachment{ nullptr };
		RenderPass* m_Renderpass{ nullptr };
		FrameBuffer* m_Framebuffer{ nullptr };
		std::unique_ptr<MessageSystem>			m_MessagingSystem = nullptr;
		std::shared_ptr<DeferredController>		m_DeferredController {nullptr};

		int										m_LayerID = 0;
		bool									m_RenderAllLayer = true;
		bool									m_ViewingFrom2D = false;
		bool									m_FrameHasBegin = false;
		bool									m_StartRender = true;
		bool									m_NormalizedView = false;
		Vec4									m_CurrClearColor;
		float									m_TabOffset{};
		Vec4									m_ViewportScreen{};
	public:
		inline static std::shared_ptr<GraphicsManager> m_Instance;

		GraphicsManager();
		~GraphicsManager();

		void								SetFadeFactor(float val);
		float								GetFadeFactor();
		void								SetClearColor(Vec4 clearColor);
		void								ToggleViewFrom2D(bool condition);
		bool								IsViewingFrom2D();
		void								StopAllRender();
		void								StartAllRender();
		bool								IsRenderOn();
		
		MaterialManager&					GetMaterialManager();

		inline bool							IsNormalizedView()
		{
			return							m_NormalizedView;
		}

		inline void							ToggleNormalizedView(bool condition)
		{
			m_NormalizedView = condition;
		}
		inline bool							IfFrameHasBegin()
		{
			return m_FrameHasBegin;
		}
		void								AddRenderLayer(RenderLayer* layer);
		void								Init(WindowsWin* window);
		void								InitSkyBox();
		void								InitDebugRenderers();
		void								StartFrame();
		void								DrawFrame();
		void								EndFrame();
		void								ShutDown();

		void								UpdateClearColor();
		void								ResizeEvent(std::uint32_t width, std::uint32_t height);
		void								setCamera(TDSCamera& camera);
		std::shared_ptr<VulkanInstance>		getVkInstancePtr();
		std::shared_ptr<DeferredController>	GetDeferredController();
		std::shared_ptr<Renderer2D>			GetRenderer2D();
		std::shared_ptr<FontRenderer>		GetFontRenderer();

		WindowsWin* GetWindow();

		void								SetLayerToRender(int ID);
		bool								RenderAllLayer();
		
		std::uint32_t						PickedObject();
		void								ToggleRenderAllLayer(bool condition);
		int									LayerToRender();


		void								RenderFullScreen();
		void								CreateFullScreen();
		void								UpdateFullScreen();

		std::shared_ptr<VulkanPipeline>		m_FinalQuad = nullptr;
		std::shared_ptr<VMABuffer>			m_FinalQuadVertexBuffer = nullptr;
		std::shared_ptr<VMABuffer>			m_FinalQuadIndexBuffer = nullptr;
		VkDescriptorImageInfo				m_FinalIamgeInfo{};


		TDSCamera& GetCamera();

		VkCommandBuffer& getCommandBuffer();
		Renderer& GetSwapchainRenderer();
		VulkanInstance& getVkInstance();
		CommandManager& getCommandManager();
		static GraphicsManager& getInstance();
		/*RenderTarget& getFinalImage() { return *m_RenderingAttachment; }*/

		RenderTarget& getFinalImage();
		RenderTarget& getPickImage() { return *m_PickAttachment; }
		RenderTarget& getFinalDepthAttachment() { return *m_RenderingDepthAttachment; }
		FrameBuffer& getFrameBuffer() { return *m_Framebuffer; }
		RenderPass& getRenderPass() { return *m_Renderpass; }
		std::shared_ptr<SkyBoxRenderer> GetSkyBox();
		inline DebugRenderer& GetDebugRenderer() { return *m_DebugRenderer; }
		std::unique_ptr<PointLightSystem> m_PointLightRenderer;
		std::unique_ptr<DebugRenderer> m_DebugRenderer;


		ObjectPick& getObjectPicker();

		MessageSystem& GetMessageSystem();

		WindowsWin* getWindow() const { return m_pWindow; }
		Vec4& getViewportScreen() { return m_ViewportScreen; }
		float& getOffset() { return m_TabOffset; }
	};

	DLL_API float getScreenWidth();
	DLL_API float getScreenHeight();
}