#include "Rendering/FontRenderer.h"
#include "vulkanTools/VulkanPipeline.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#include "vulkanTools/VulkanTexture.h"
#include "AssetManagement/FontFactory.h"
#include "AssetManagement/AssetManager.h"
#include "components/UiSprite.h"
#include "Rendering/Revamped/DeferredController.h"
namespace TDS
{
	struct FontVertex
	{
		Vec3 m_Position;
	};
	//void FontRenderer::Init()
	//{
	//	std::vector<Vec3> quad =
	//	{
	//		{-0.5f,  0.5f, 0.f},
	//		{-0.5f, -0.5f, 0.f},
	//		{ 0.5f,  0.5f, 0.f},
	//		{ 0.5f, -0.5f, 0.f}
	//	};
	//	m_VertexBuffer = std::make_shared<VMABuffer>();
	//	m_VertexBuffer->CreateVertexBuffer(quad.size() * sizeof(Vec3), true, quad.data());
	//	m_VertexBuffer->SetDataCnt((std::uint32_t)quad.size());
	//	std::vector<uint32_t> indices =
	//	{
	//		0, 1, 2,
	//		2, 1, 3
	//	};
	//	m_IndexBuffer = std::make_shared<VMABuffer>();
	//	m_IndexBuffer->CreateIndexBuffer(indices.size() * sizeof(uint32_t), true, indices.data());
	//	m_IndexBuffer->SetDataCnt((std::uint32_t)indices.size());
	//	PipelineCreateEntry entry{};
	//	entry.m_NumDescriptorSets = 1;

	//	entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../../assets/shaders/FontVertex.spv"));
	//	entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../../assets/shaders/FontFragment.spv"));
	//	VertexLayout layout =
	//		VertexLayout(
	//			{
	//			  VertexBufferElement(VAR_TYPE::VEC3, "vPosition")
	//			});
	//	entry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(Vec3)));
	//	BufferInfo info{};
	//	info.m_Data = nullptr;
	//	info.m_Size = sizeof(Instance) * MAX_CHARACTERS;
	//	info.m_Static = false;
	//	entry.m_ShaderInputs.m_InputBuffers.insert(std::make_pair(1, info));
	//	m_Pipeline = std::make_shared<VulkanPipeline>();
	//	m_Pipeline->Create(entry);


	//}
	//void FontRenderer::Update(const std::vector<EntityID>& entity, FontComponent* component)
	//{
	//	FontUBO ubo{};
	//	auto cmdBuffer = GraphicsManager::getInstance().getCommandBuffer();
	//	ubo.m_view = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
	//	ubo.m_projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
	//		GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 10.f);
	//	ubo.m_projection.m[1][1] *= -1;

	//	m_Pipeline->SetCommandBuffer(cmdBuffer);
	//	std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
	//	m_Pipeline->UpdateUBO(&ubo, sizeof(FontUBO), 2, frame);
	//	for (size_t i = 0; i < entity.size(); ++i)
	//	{

	//		double CurX = 0;
	//		double CurY = 0;
	//		if (component->m_FontInfo.m_FontAtlasTexture != nullptr)
	//			m_Pipeline->UpdateDescriptor(component->m_FontInfo.m_FontAtlasTexture->m_VulkanTexture->getInfo(), VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3);
	//		std::int32_t instanceCnt = 0;
	//		if (component->m_FontInfo.m_AtlasLayout.m_Glyphs.empty())
	//			return;
	//		for (auto& character : component[i].m_UIText)
	//		{
	//			if (character == '\n')
	//			{
	//				CurX = 0;
	//				CurY -= component[i].m_FontInfo.m_AtlasLayout.m_Metrics.m_LineHeight;
	//				continue;
	//			}

	//			Glyph& glyph = component[i].m_FontInfo.m_AtlasLayout.m_Glyphs[character];
	//			double charWidth = glyph.m_PlaneBounds.m_Right - glyph.m_PlaneBounds.m_Left;
	//			double charHeight = glyph.m_PlaneBounds.m_Top - glyph.m_PlaneBounds.m_Bottom;


	//			double UV_left = glyph.m_AtlasBounds.m_Left / component[i].m_FontInfo.m_AtlasLayout.m_Atlas.m_Width;
	//			double UV_right = glyph.m_AtlasBounds.m_Right / component[i].m_FontInfo.m_AtlasLayout.m_Atlas.m_Width;
	//			double UV_bottom = 1.0 - (glyph.m_AtlasBounds.m_Top / component[i].m_FontInfo.m_AtlasLayout.m_Atlas.m_Height);
	//			double UV_top = 1.0 - (glyph.m_AtlasBounds.m_Bottom / component[i].m_FontInfo.m_AtlasLayout.m_Atlas.m_Height);

	//			Mat4 temp{ 1.0f, 0.f, 0.f, 0.f,
	//						0.f, 1.f, 0.f, 0.f,
	//						0.f, 0.f, 1.f, 0.f,
	//						0.f, 0.f, 0.f, 1.f };

	//			temp = temp.Translate(Vec3(CurX, CurY, 0.f));
	//			temp = temp.Scale(Vec3(charWidth, charHeight, 1.f));
	//			m_CharacterInstance[instanceCnt].m_model = temp;
	//			m_CharacterInstance[instanceCnt].m_StartUV = Vec2(float(UV_left), float(UV_bottom));
	//			m_CharacterInstance[instanceCnt].m_EndUV = Vec2(float(UV_right), float(UV_top));



	//			CurX += glyph.m_Advance;

	//			instanceCnt++;
	//		}
	//		m_Pipeline->UpdateUBO(m_CharacterInstance.data(), m_CharacterInstance.size() * sizeof(Instance), 1, frame);


	//		FontColors fontClr{};
	//		fontClr.bgColor = { 1.f, 0.f, 0.f, 1.f };
	//		fontClr.fgColor = { 1.f, 1.f, 1.f, 1.f };
	//		fontClr.pxRange = 2.f;
	//		m_Pipeline->UpdateUBO(&fontClr, sizeof(FontColors), 4, frame);


	//		m_Pipeline->BindPipeline();
	//		m_Pipeline->BindVertexBuffer(*m_VertexBuffer);
	//		m_Pipeline->BindIndexBuffer(*m_IndexBuffer);
	//		m_Pipeline->DrawInstancedIndexed(*m_VertexBuffer, *m_IndexBuffer, instanceCnt, frame);

	//	}

	//}

	//void FontRenderer::ShutDown()
	//{
	//	m_IndexBuffer->DestroyBuffer();
	//	m_VertexBuffer->DestroyBuffer();
	//	m_Pipeline->ShutDown();
	//	m_pFrameBuffer = nullptr;
	//}

	void FontRenderer::Init()
	{
		std::vector<FontVertex> m_VertexData;
		m_VertexData.push_back(FontVertex(Vec3(-0.5f, -0.5f, 0.f)));
		m_VertexData.push_back(FontVertex(Vec3(0.5f, -0.5f, 0.f)));
		m_VertexData.push_back(FontVertex(Vec3(0.5f, 0.5f, 0.f)));
		m_VertexData.push_back(FontVertex(Vec3(-0.5f, 0.5f, 0.f)));

		std::vector<std::uint32_t> indexBuffer =
		{
			0, 1, 3,
			1, 2, 3
		};

		m_VertexBuffer = new VMABuffer();
		m_IndexBuffer = new VMABuffer();

		m_VertexBuffer->CreateVertexBuffer(m_VertexData.size() * sizeof(FontVertex), false, m_VertexData.data());
		m_VertexBuffer->SetDataCnt(m_VertexData.size());
		m_IndexBuffer->CreateIndexBuffer(indexBuffer.size() * sizeof(std::uint32_t), false, indexBuffer.data());
		m_IndexBuffer->SetDataCnt(indexBuffer.size());

		m_Pipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry entry{};
		entry.m_NumDescriptorSets = 1;

		entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/FontVertex.spv"));
		entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/FontFragment.spv"));
		entry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		entry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		entry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		entry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		VertexLayout layout =
			VertexLayout(
				{
				  VertexBufferElement(VAR_TYPE::VEC3, "vPosition")
				});

		BufferInfo Buffer{};
		Buffer.m_Data = m_FontBatch.m_Instances.data();
		Buffer.m_Size = m_FontBatch.m_Instances.size() * sizeof(InstanceFont);
		Buffer.m_Static = false;
		entry.m_ShaderInputs.m_InputBuffers[10] = Buffer;
		entry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(FontVertex)));
		auto deferredController = GraphicsManager::getInstance().GetDeferredController();

		auto frameBuffer = deferredController->GetFrameBuffer(RENDER_COMPOSITION);
		entry.m_FBTarget = frameBuffer;
		m_Pipeline->Create(entry);
	}

	void FontRenderer::ShutDown()
	{
		m_Pipeline->ShutDown();
		delete m_IndexBuffer;
		delete m_VertexBuffer;

	}

	struct PushConstantFont
	{
		unsigned int Offset = 0;
	};

	void FontRenderer::Draw(VkCommandBuffer commandBuffer, int Frame)
	{
		PushConstantFont push{};
		m_Pipeline->BindPipeline();
		if (m_FontBatch.m_InstanceCnt > 0)
		{
			for (std::uint32_t i = 0; i < 12; ++i)
			{
				auto& layer = m_FontBatch.m_LayerInfos[i];
				if (layer.m_Instance > 0)
				{
					push.Offset = layer.m_Offset;
					m_Pipeline->SubmitPushConstant(&push, sizeof(PushConstantFont), SHADER_FLAG::VERTEX);
					m_Pipeline->BindVertexBuffer(*m_VertexBuffer);
					m_Pipeline->BindIndexBuffer(*m_IndexBuffer);
					m_Pipeline->BindDescriptor(Frame, 1);
					m_Pipeline->BindArrayDescriptorSet(0, 1, 1);
					m_Pipeline->DrawInstancedIndexed(*m_VertexBuffer, *m_IndexBuffer, layer.m_Instance);
					layer.m_Instance = 0;
					layer.m_Offset = 0;
					layer.isDirty = true;
				}
			}
		}
	}

	void FontRenderer::Update(VkCommandBuffer commandBuffer, int Frame)
	{
		m_Pipeline->SetCommandBuffer(commandBuffer);

		m_FontBatch.m_SceneUpdate.ViewingFrom2D = 1;
		if (m_FontBatch.m_SceneUpdate.ViewingFrom2D == false)
		{
			m_FontBatch.m_SceneUpdate.Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
				GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000.f);

			m_FontBatch.m_SceneUpdate.Projection.m[1][1] *= -1;
		}
		else
		{
			m_FontBatch.m_SceneUpdate.Projection = Mat4::Ortho(
				-1.f,
				1.f,
				-1.f,
				1.f,
				-20.f,
				20.f
			);
			m_FontBatch.m_SceneUpdate.Projection.m[1][1] *= -1;
		}
		m_FontBatch.m_SceneUpdate.View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();
		m_Pipeline->UpdateUBO(&m_FontBatch.m_SceneUpdate, sizeof(FontSceneUBO), 25, Frame);
		m_Pipeline->UpdateUBO(m_FontBatch.m_Instances.data(), sizeof(InstanceFont) * m_FontBatch.m_InstanceCnt, 10, Frame);
		auto& fontfactory = AssetManager::GetInstance()->GetFontFactory();
		if (fontfactory.m_UpdateFontTextures)
		{
			m_Pipeline->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, fontfactory.GetFontAtlasTextures());
			fontfactory.m_UpdateFontTextures = false;
		}
	}

	VulkanPipeline& FontRenderer::GetPipeline()
	{
		 return *m_Pipeline;
	}




	void FontBatch::AddToBatch(void* componentSprite, Transform* transform, std::uint32_t entity)
	{

		std::uint32_t WindowWidth = GraphicsManager::getInstance().GetWindow()->getWidth();
		std::uint32_t WindowHeight = GraphicsManager::getInstance().GetWindow()->getHeight();

		UISprite* UiSprite = reinterpret_cast<UISprite*>(componentSprite);
		if (UiSprite->m_LayerID == -1)
			return;
		AssetFactory<FontAtlas>& factoryRef = AssetManager::GetInstance()->GetFontFactory();
		if (UiSprite->m_TextureName != UiSprite->GetReference().m_AssetName)
			UiSprite->m_TextureName = UiSprite->GetReference().m_AssetName;



		int TextureID = factoryRef.GetFontAtlasIndex(UiSprite->m_FontName, UiSprite->GetFontReference());

		if (TextureID == -1)
		{
			m_InstanceInfo[m_InstanceCnt].m_TextureIndex = 999;
			m_InstanceInfo[m_InstanceCnt].m_StartEnd = { 0.f, 0.f, 0.f, 0.f };
			return;
		}
		FontAtlas& atlasinfo = factoryRef.GetFontAtlas(TextureID);
		double baselineOffset = atlasinfo.m_Metrics.m_Ascender; // Ascender for baseline
		Vec2 CurrOffset = 0.f;
		char prevChar = '\0'; // Keep track of the previous character for kerning
		for (auto& eachchar : UiSprite->m_Message)
		{


			m_InstanceInfo[m_InstanceCnt].m_LayerID = UiSprite->m_LayerID > 12 ? 12 : (std::uint32_t)UiSprite->m_LayerID;
			m_InstanceInfo[m_InstanceCnt].m_FontColor.m_Color = UiSprite->m_Color;
			m_InstanceInfo[m_InstanceCnt].m_FontColor.bgColor = UiSprite->m_BackGroundColour;
			m_InstanceInfo[m_InstanceCnt].m_FontColor.fgColor = UiSprite->m_ForeGroundColour;



			m_InstanceInfo[m_InstanceCnt].m_TextureIndex = TextureID;
			Glyph& glyph = atlasinfo.m_Glyphs[eachchar];
			if (eachchar == '\n')
			{
				CurrOffset.x = 0.f;
				CurrOffset.y -= atlasinfo.m_Metrics.m_LineHeight;

				continue;
			}
			if (Vec3 Scale = transform->GetScale(); Scale.x <= 0.f || Scale.y <= 0.f || Scale.z <= 0.f)
			{
			}
			else
			{
				transform->GenerateTransform();
			}
			Mat4 temp = transform->GetTransformMatrix();
			double charWidth = (glyph.m_PlaneBounds.m_Right - glyph.m_PlaneBounds.m_Left);
			double charHeight = (glyph.m_PlaneBounds.m_Top - glyph.m_PlaneBounds.m_Bottom);


			double UV_left = glyph.m_AtlasBounds.m_Left / atlasinfo.m_Atlas.m_Width;
			double UV_right = glyph.m_AtlasBounds.m_Right / atlasinfo.m_Atlas.m_Width;
			double UV_bottom = glyph.m_AtlasBounds.m_Bottom / atlasinfo.m_Atlas.m_Height;
			double UV_top = glyph.m_AtlasBounds.m_Top / atlasinfo.m_Atlas.m_Height;

			m_InstanceInfo[m_InstanceCnt].m_StartEnd = { float(UV_left), float(UV_bottom), float(UV_right), float(UV_top) };
			Mat4 internalTransform = Mat4::identity();

			Vec3 finalOffset{};
			finalOffset.x = float(CurrOffset.x + (charWidth * 0.5));

			finalOffset.z = 0.f;

			if (eachchar == 'g' || eachchar == 'j' || eachchar == 'p' || eachchar == 'q' || eachchar == 'y')
			{
				finalOffset.y = float(CurrOffset.y + (charHeight * 0.25));
			}
			else if (eachchar == '\"' || eachchar == '\'')
			{
				finalOffset.y = float(CurrOffset.y + (charHeight + (charHeight * 0.75)));
			}
			else
			{
				finalOffset.y = float(CurrOffset.y + (charHeight * 0.5));
			}

			internalTransform = internalTransform.Translate(finalOffset)
				* internalTransform.Scale(Vec3(float(charWidth), float(charHeight), 0.f));


			m_InstanceInfo[m_InstanceCnt].m_Transform = temp * internalTransform;

			CurrOffset.x += float(glyph.m_Advance);
			++m_InstanceCnt;

		}

	}

	void DLL_API FontBatch::PrepareBatch()
	{

		for (std::uint32_t i = 0; i < m_InstanceCnt; ++i)
		{
			for (std::uint32_t layer = 0; layer < 12; ++layer)
			{
				if (GraphicsManager::getInstance().RenderAllLayer() == false)
				{
					if (GraphicsManager::getInstance().LayerToRender() != i)
						continue;
				}
				auto& instanceinfo = m_InstanceInfo[i];
				if (layer == instanceinfo.m_LayerID)
				{
					if (m_LayerInfos[layer].isDirty)
					{
						m_LayerInfos[layer].m_Offset = i;
						m_LayerInfos[layer].isDirty = false;
					}

					++m_LayerInfos[layer].m_Instance;
					m_Instances[i].bgColor = instanceinfo.m_FontColor.bgColor;
					m_Instances[i].fgColor = instanceinfo.m_FontColor.fgColor;
					m_Instances[i].m_Color = instanceinfo.m_FontColor.m_Color;
					m_Instances[i].m_texID.x = static_cast<float>(instanceinfo.m_TextureIndex);
					m_Instances[i].m_Model = instanceinfo.m_Transform;;
					m_Instances[i].m_UVstartEnd = instanceinfo.m_StartEnd;


				}
			}
		}
	}



}