#pragma once
#include "camera/SphereFrustum.h"
#include "ResourceManagement/ResourceRef.h"
#include "GraphicsResource/AssetModel.h"
#include "Rendering/Batch2D.h"

namespace TDS
{
	struct InstanceInfo2D
	{
		Vec3			m_Scale;
		Vec3			m_Rotate;
		Vec3			m_Translate;
		Mat4			m_Transform{ 1.f };
		Vec4* m_Color = nullptr;
		std::uint32_t	m_LayerID;
		std::uint32_t	m_TextureIndex = 999;
		uint32_t		m_ID{};
	};

	struct alignas(16) SceneUBO
	{
		Mat4 Projection;
		Mat4 View;
		unsigned int ViewingFrom2D = 0;
		float padding[3];
	};

	struct alignas(16) Instance2D
	{
		Mat4 m_Model = Mat4::identity();
		Vec4 m_Color;
		Vec4 m_texID = { 999.f, 0.f, 0.f, 0.f };
		Vec4 m_LayerID = { 0.f, 0.f, 0.f, 0.f };
		uint32_t ID{};
	};

	class Transform;
	class UISprite;

	struct SpriteBatch : Batch2D
	{
		std::vector<Instance2D>				m_InstanceVector;
		//std::array<Instance2D, 12000>		m_Instances;
		std::array<InstanceInfo2D, 5000>	m_InstanceInfo;
		virtual void DLL_API				AddToBatch(void* componentSprite, Transform* transform, std::uint32_t entity);
		virtual void DLL_API				PrepareBatch();
	};


	class RenderTarget;
	class RenderPass;
	class FrameBuffer;
	class VulkanPipeline;

	struct OffScreenFrameBuffer
	{
		RenderTarget* m_RenderingAttachment{ nullptr };
		RenderTarget* m_PickingAttachment{ nullptr };
		RenderTarget* m_RenderingDepthAttachment{ nullptr };
		RenderPass* m_Renderpass{ nullptr };
		FrameBuffer* m_Framebuffer{ nullptr };
		Vec4 ClearColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	struct RenderedSprite
	{
		bool					m_Update = false;
		VkDescriptorImageInfo	m_ImageInfo{};

	};

	class Renderer2D
	{
	public:
		DLL_API Renderer2D();
		DLL_API ~Renderer2D();
		void DLL_API Init();
		void DLL_API Draw(VkCommandBuffer commandBuffer, int Frame);
		void DLL_API Update(VkCommandBuffer commandBuffer, int Frame);
		inline SpriteBatch& GetBatchList()
		{
			return m_BatchList;
		}
		inline RenderedSprite& GetRenderedSprite()
		{
			return m_SpriteTexture;
		}
		void DLL_API ShutDown();

		DLL_API VulkanPipeline& GetPipeline();
	private:
		std::shared_ptr<VulkanPipeline> m_Pipeline = nullptr;
		SpriteBatch m_BatchList{};
		RenderedSprite m_SpriteTexture{};
		OffScreenFrameBuffer m_FrameBuffer; //Not using for now
		VMABuffer* m_VertexBuffer = nullptr;
		VMABuffer* m_IndexBuffer = nullptr;
		SceneUBO m_SceneUBO{}; //Probably not in used.
		Vec4	m_ClearColor;


	};

}