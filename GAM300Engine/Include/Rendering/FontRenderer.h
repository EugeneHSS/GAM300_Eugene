#pragma once
#include "MathCommon.h"
#include "GraphicsResource/FontInfo.h"
#include "vulkanTools/VMATypes/VMABuffer.h"
#include <ecs/ecs.h>
#include <dotnet/ImportExport.h>
#include "Rendering/Batch2D.h"
namespace TDS
{
	struct alignas(16) FontSceneUBO
	{
		Mat4 Projection;
		Mat4 View;
		unsigned int ViewingFrom2D = 0;
		float padding[3]; //Setting padding[0] to use pxrange
	};

	struct FontColorInfo
	{
		Vec4 m_Color;
		Vec4 bgColor;
		Vec4 fgColor;
	};

	struct alignas(16) InstanceFont
	{
		Mat4			m_Model;
		Vec4			m_Color;
		Vec4			bgColor;
		Vec4			fgColor;
		Vec4			m_UVstartEnd = { 0.f, 0.f, 0.f, 0.f };
		Vec4			m_texID = { 999.f, 0.f, 0.f, 0.f };
	};
	struct InstanceFontInfo
	{
		Mat4			m_Transform{ 1.f };
		FontColorInfo   m_FontColor{};
		Vec4			m_StartEnd;
		std::uint32_t	m_LayerID;
		std::uint32_t	m_TextureIndex = 999;
	};

	struct FontBatch : Batch2D
	{
		std::array<InstanceFont, 10000>				m_Instances;
		std::array<InstanceFontInfo, 10000>			m_InstanceInfo;
		virtual void DLL_API						AddToBatch(void* componentSprite, Transform* transform, std::uint32_t entity);
		virtual void DLL_API						PrepareBatch();
		FontSceneUBO								m_SceneUpdate{};
	};


	constexpr std::uint32_t MAX_CHARACTERS = 5000;
	class VulkanPipeline;
	class FrameBuffer;
	class Transform;
	class FontRenderer
	{
	private:

		std::shared_ptr<VulkanPipeline> m_Pipeline = nullptr;
		std::shared_ptr<FrameBuffer>	m_pFrameBuffer = nullptr;
		std::array<InstanceFont, MAX_CHARACTERS> m_CharacterInstance;
		VMABuffer* m_VertexBuffer = nullptr;
		VMABuffer* m_IndexBuffer = nullptr;
		FontBatch m_FontBatch{};
	public:
		bool m_UpdateInstance = true;
	public:
		DLL_API void Init();
		DLL_API void ShutDown();
		void DLL_API Draw(VkCommandBuffer commandBuffer, int Frame);
		void DLL_API Update(VkCommandBuffer commandBuffer, int Frame);
		DLL_API VulkanPipeline& GetPipeline();
		inline FontBatch& GetBatchList()
		{
			return m_FontBatch;
		}



	};
}