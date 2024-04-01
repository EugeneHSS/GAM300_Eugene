#include "Rendering/Renderer2D.h"
#include "vulkanTools/VulkanPipeline.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/renderPass.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/vulkanInstance.h"
#include "AssetManagement/AssetManager.h"
#include "Rendering/Revamped/DeferredController.h"
#include "vulkanTools/Renderer.h"
#include "windowswindow.h"
namespace TDS
{
	struct Renderer2DVertex
	{
		Vec3 m_Position;
		Vec2 m_UV;
	};
	struct pushConstant
	{
		Mat4 testmodel;
		unsigned int dataOffset = 0;
		float padding[3];
	};

	pushConstant pushTest;

	Renderer2D::Renderer2D()
	{
	}
	Renderer2D::~Renderer2D()
	{
	}
	void Renderer2D::Init()
	{
		std::vector<Renderer2DVertex> m_VertexData;
		m_VertexData.push_back(Renderer2DVertex(Vec3(-0.5f, -0.5f, 0.f), Vec2(0.f, 1.f)));
		m_VertexData.push_back(Renderer2DVertex(Vec3(0.5f, -0.5f, 0.f), Vec2(1.f, 1.f)));
		m_VertexData.push_back(Renderer2DVertex(Vec3(0.5f, 0.5f, 0.f), Vec2(1.f, 0.f)));
		m_VertexData.push_back(Renderer2DVertex(Vec3(-0.5f, 0.5f, 0.f), Vec2(0.f, 0.f)));


		std::vector<std::uint32_t> indexBuffer =
		{
			0, 1, 3,
			1, 2, 3
		};

		m_VertexBuffer = new VMABuffer();
		m_IndexBuffer = new VMABuffer();

		m_VertexBuffer->CreateVertexBuffer(m_VertexData.size() * sizeof(Renderer2DVertex), false, m_VertexData.data());
		m_VertexBuffer->SetDataCnt(m_VertexData.size());
		m_IndexBuffer->CreateIndexBuffer(indexBuffer.size() * sizeof(std::uint32_t), false, indexBuffer.data());
		m_IndexBuffer->SetDataCnt(indexBuffer.size());


		Vec2 Dimension = { static_cast<float>(GraphicsManager::getInstance().GetWindow()->getWidth()), static_cast<float>(GraphicsManager::getInstance().GetWindow()->getHeight()) };
		Vec3 size = { Dimension.x, Dimension.y, 1.f };
		std::vector<AttachmentInfo> attachmentInfos{};
		std::vector<RenderTarget*> attachments{};

		RenderTargetCI rendertargetCI{
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_VIEW_TYPE_2D,
			size,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			RenderTargetType::COLOR,
			false,
			VK_SAMPLE_COUNT_1_BIT
		};
		RenderTargetCI rendertargetCI2{
				VK_FORMAT_D32_SFLOAT,
				VK_IMAGE_ASPECT_DEPTH_BIT,
				VK_IMAGE_VIEW_TYPE_2D,
				size,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				RenderTargetType::DEPTH,
				false,
				VK_SAMPLE_COUNT_1_BIT
		};

		RenderTargetCI rendertargetCI3{
			VK_FORMAT_R32_UINT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_VIEW_TYPE_2D,
			size,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_LAYOUT_GENERAL,
			RenderTargetType::COLOR,
			false
		};


		std::shared_ptr<VulkanInstance> instance = GraphicsManager::getInstance().getVkInstancePtr();
		m_FrameBuffer.m_RenderingAttachment = new RenderTarget(instance, rendertargetCI);
		m_FrameBuffer.m_PickingAttachment = new RenderTarget(instance, rendertargetCI3);
		m_FrameBuffer.m_RenderingDepthAttachment = new RenderTarget(instance, rendertargetCI2);
		attachmentInfos.push_back({ m_FrameBuffer.m_RenderingAttachment, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE });
		attachmentInfos.back().clear.color = { m_FrameBuffer.ClearColor.x,  m_FrameBuffer.ClearColor.y, m_FrameBuffer.ClearColor.z, m_FrameBuffer.ClearColor.w };
		attachmentInfos.push_back({ m_FrameBuffer.m_RenderingDepthAttachment, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE });
		attachmentInfos.push_back({ m_FrameBuffer.m_PickingAttachment, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE });
		attachments.push_back(m_FrameBuffer.m_RenderingAttachment);
		attachments.push_back(m_FrameBuffer.m_RenderingDepthAttachment);
		attachments.push_back(m_FrameBuffer.m_PickingAttachment);

		m_FrameBuffer.m_Renderpass = new RenderPass(instance->getVkLogicalDevice(), attachmentInfos);
		m_FrameBuffer.m_Framebuffer = new FrameBuffer(instance->getVkLogicalDevice(), m_FrameBuffer.m_Renderpass->getRenderPass(), attachments);

		m_Pipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry entry{};
		entry.m_NumDescriptorSets = 1;

		entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/Render2DVertInstanced.spv"));
		entry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/Render2DFragInstanced.spv"));
		entry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_SRC_ALPHA;
		entry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		entry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_SRC_ALPHA;
		entry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		VertexLayout layout =
			VertexLayout(
				{
				  VertexBufferElement(VAR_TYPE::VEC3, "vPosition"),
				VertexBufferElement(VAR_TYPE::VEC2, "inTexCoord"),
				//VertexBufferElement(VAR_TYPE::VEC3, "vColor"),
				//VertexBufferElement(VAR_TYPE::VEC4, "vNormals")
				});

		m_BatchList.m_InstanceVector.resize(5000);
		BufferInfo Buffer{};
		Buffer.m_Data = m_BatchList.m_InstanceVector.data();
		Buffer.m_Size = m_BatchList.m_InstanceVector.size() * sizeof(Instance2D);
		Buffer.m_Static = false;
		entry.m_ShaderInputs.m_InputBuffers[10] = Buffer;
		entry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(Renderer2DVertex)));

		auto deferredController = GraphicsManager::getInstance().GetDeferredController();

		auto frameBuffer = deferredController->GetFrameBuffer(RENDER_COMPOSITION);
		entry.m_FBTarget = frameBuffer;
		m_Pipeline->Create(entry);



	}

	void Renderer2D::Draw(VkCommandBuffer commandBuffer, int Frame)
	{
		m_Pipeline->BindPipeline();
		if (m_BatchList.m_InstanceCnt > 0)
		{
			/*for (std::uint32_t i = 0; i < 12; ++i)
			{*/
			//if (GraphicsManager::getInstance().RenderAllLayer() == false)
			//{
			//	int layerToRender = GraphicsManager::getInstance().LayerToRender();
			//	if (layerToRender != i)
			//		continue;
			//}
			/*auto& layer = m_BatchList.m_LayerInfos[i];
			if (layer.m_Instance > 0)
			{*/
			//m_Pipeline->SubmitPushConstant(&layer.m_Offset, sizeof(std::uint32_t), SHADER_FLAG::VERTEX | SHADER_FLAG::FRAGMENT);
			pushTest.dataOffset = 0;
			/*	m_Pipeline->SubmitPushConstant(&pushTest, sizeof(pushConstant), SHADER_FLAG::VERTEX);
				m_Pipeline->BindVertexBuffer(*m_AssetModel.m_ResourcePtr->GetVertexBuffer());
				m_Pipeline->BindIndexBuffer(*m_AssetModel.m_ResourcePtr->GetIndexBuffer());
				m_Pipeline->BindDescriptor(Frame, 1);
				m_Pipeline->BindArrayDescriptorSet(0, 1, 1);
				m_Pipeline->DrawInstancedIndexed(*m_AssetModel.m_ResourcePtr->GetVertexBuffer(), *m_AssetModel.m_ResourcePtr->GetIndexBuffer(), layer.m_Instance);*/
			m_Pipeline->SubmitPushConstant(&pushTest, sizeof(pushConstant), SHADER_FLAG::VERTEX);
			m_Pipeline->BindVertexBuffer(*m_VertexBuffer);
			m_Pipeline->BindIndexBuffer(*m_IndexBuffer);
			m_Pipeline->BindDescriptor(Frame, 1);
			m_Pipeline->BindArrayDescriptorSet(0, 1, 1);
			m_Pipeline->DrawInstancedIndexed(*m_VertexBuffer, *m_IndexBuffer, m_BatchList.m_InstanceCnt);
			//layer.m_Instance = 0;
			//layer.m_Offset = 0;
			//layer.isDirty = true;
		/*}*/
	/*}*/
	//m_FrameBuffer.m_Renderpass->endRenderPass(commandBuffer);

		}
	}
	void Renderer2D::Update(VkCommandBuffer commandBuffer, int Frame)
	{

		//if (m_AssetModel.m_ResourcePtr == nullptr)
		//{
		//	std::string PlanePath = "Quad2_Bin.bin";
		//	PlanePath = MODEL_PATH + PlanePath;

		//	AssetManager::GetInstance()->LoadAsset(PlanePath, m_AssetModel);

		//	if (m_AssetModel.m_ResourcePtr->BufferIsNull())
		//		m_AssetModel.m_ResourcePtr->CreateBuffers();
		//}
		m_Pipeline->SetCommandBuffer(commandBuffer);

		m_SceneUBO.ViewingFrom2D = 1;

		if (m_SceneUBO.ViewingFrom2D == false)
		{
			m_SceneUBO.Projection = Mat4::Perspective(GraphicsManager::getInstance().GetCamera().m_Fov * Mathf::Deg2Rad,
				GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000.f);

			m_SceneUBO.Projection.m[1][1] *= -1;

		}
		else
		{

			RECT win;
			GetClientRect(GraphicsManager::getInstance().GetWindow()->getWindowHandler(), &win);
			float hlfWidth = (win.right - win.left) * 0.5f;
			float hlfHeight = (win.bottom - win.top) * 0.5f;
			m_SceneUBO.Projection = Mat4::Ortho(
				-1.f,
				1.f,
				-1.f,
				1.f,
				-20.f,
				20.f
			);
			m_SceneUBO.Projection.m[1][1] *= -1;

		}
		m_SceneUBO.View = GraphicsManager::getInstance().GetCamera().GetViewMatrix();

		m_Pipeline->UpdateUBO(&m_SceneUBO, sizeof(SceneUBO), 25, Frame);
		m_Pipeline->UpdateUBO(m_BatchList.m_InstanceVector.data(), sizeof(Instance2D) * m_BatchList.m_InstanceCnt, 10, Frame);
		if (AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray2D)
		{
			m_Pipeline->UpdateTextureArray(4, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, AssetManager::GetInstance()->GetTextureFactory().GetTextureArray());
			AssetManager::GetInstance()->GetTextureFactory().m_UpdateTextureArray2D = false;
		}
	}

	void Renderer2D::ShutDown()
	{
		m_Pipeline->ShutDown();
		delete m_IndexBuffer;
		delete m_VertexBuffer;
		delete m_FrameBuffer.m_Framebuffer;
		delete m_FrameBuffer.m_RenderingAttachment;
		delete m_FrameBuffer.m_RenderingDepthAttachment;
		delete m_FrameBuffer.m_Renderpass;
	}

	VulkanPipeline& Renderer2D::GetPipeline()
	{
		return *m_Pipeline;
	}

	void SpriteBatch::AddToBatch(void* component, Transform* transform, std::uint32_t entity)
	{
		auto Renderer2D = GraphicsManager::getInstance().GetRenderer2D();
		UISprite* componentSprite = reinterpret_cast<UISprite*>(component);

		m_InstanceInfo[m_InstanceCnt].m_LayerID = componentSprite->m_LayerID > 12 ? 12 : (std::uint32_t)componentSprite->m_LayerID;
		m_InstanceInfo[m_InstanceCnt].m_Color = &componentSprite->m_Color;
		m_InstanceInfo[m_InstanceCnt].m_Translate = transform->GetPosition();
		m_InstanceInfo[m_InstanceCnt].m_Rotate = transform->GetRotation();
		m_InstanceInfo[m_InstanceCnt].m_Scale = transform->GetScale();
		m_InstanceInfo[m_InstanceCnt].m_Transform = transform->GetTransformMatrix();
		m_InstanceInfo[m_InstanceCnt].m_ID = entity;
		if (Vec3 Scale = transform->GetScale(); Scale.x <= 0.f || Scale.y <= 0.f || Scale.z <= 0.f)
		{
		}
		else
		{
			transform->GenerateTransform();
		}

		if (componentSprite->m_IsDirty)
		{
			Renderer2D->GetRenderedSprite().m_Update = true;
			componentSprite->m_IsDirty = false;
		}

		int TextureID = AssetManager::GetInstance()->GetTextureFactory().GetTextureIndex(componentSprite->m_TextureName, componentSprite->GetReference());

		if (TextureID == -1)
		{
			m_InstanceInfo[m_InstanceCnt].m_TextureIndex = 999;
			componentSprite->GetReference().m_AssetName = "";
		}
		else
		{
			m_InstanceInfo[m_InstanceCnt].m_TextureIndex = TextureID;
			componentSprite->GetReference().m_AssetName = componentSprite->m_TextureName;
		}

		++m_InstanceCnt;


	}

	void SpriteBatch::PrepareBatch()
	{
		if (m_InstanceCnt != std::uint32_t(m_InstanceVector.size()))
			m_InstanceVector.resize(m_InstanceCnt);
		for (std::uint32_t i = 0; i < m_InstanceCnt; ++i)
		{
			/*for (std::uint32_t layer = 0; layer < 12; ++layer)
			{*/
			auto& instanceinfo = m_InstanceInfo[i];
			/*if (layer == instanceinfo.m_LayerID)
			{*/
			/*if (m_LayerInfos[layer].isDirty)
			{
				m_LayerInfos[layer].m_Offset = i;
				m_LayerInfos[layer].isDirty = false;
			}*/

			//++m_LayerInfos[layer].m_Instance;
			m_InstanceVector[i].m_LayerID.x = float(instanceinfo.m_LayerID);
			m_InstanceVector[i].m_Color = *instanceinfo.m_Color;
			m_InstanceVector[i].m_texID.x = float(instanceinfo.m_TextureIndex);
			m_InstanceVector[i].ID = instanceinfo.m_ID;

			//Vec3 rotationRadians = Vec3(instanceinfo.m_Rotate->x * static_cast<float>(3.14159265359f / 180.0f),
			//	instanceinfo.m_Rotate->y * static_cast<float>(3.14159265359f / 180.0f),
			//		instanceinfo.m_Rotate->x * static_cast<float>(3.14159265359f / 180.0f));


			//TDS::Mat4 rotX = TDS::Mat4::Rotate(TDS::Vec3(1, 0, 0), rotationRadians.x);
			//TDS::Mat4 rotY = TDS::Mat4::Rotate(TDS::Vec3(0, 1, 0), rotationRadians.y);
			//TDS::Mat4 rotZ = TDS::Mat4::Rotate(TDS::Vec3(0, 0, 1), rotationRadians.z);

			//TDS::Mat4 combinedRotation = rotZ * rotY * rotX;

			//TDS::Mat4 scaleMatrix = TDS::Mat4::Scale(*instanceinfo.m_Scale);
			//TDS::Mat4 translationMatrix = TDS::Mat4::Translate(*instanceinfo.m_Translate);
			if (GraphicsManager::getInstance().IsViewingFrom2D())
			{
				//float rotationRadians = instanceinfo.m_Rotate.z * static_cast<float>(3.14159265359 / 180.0f);
				/*TDS::Mat4 rotZ = TDS::Mat4::Rotate(TDS::Vec3(0, 0, 1), rotationRadians);*/
				//TDS::Mat4 rotZ = TDS::Mat4::identity();
				//TDS::Mat4 scaleMatrix = TDS::Mat4::Scale(TDS::Vec3(instanceinfo.m_Scale.x, instanceinfo.m_Scale.y, 0.0f));

				//TDS::Mat4 translationMatrix = TDS::Mat4::Translate(TDS::Vec3(instanceinfo.m_Translate.x, instanceinfo.m_Translate.y, 0.0f));


				//m_Instances[i].m_Model = translationMatrix * rotZ * scaleMatrix;
				m_InstanceVector[i].m_Model = instanceinfo.m_Transform;;
			}
			else
				m_InstanceVector[i].m_Model = instanceinfo.m_Transform;
			//m_Instances[i].m_Model = translationMatrix * combinedRotation * scaleMatrix;
			//m_Instances[i].m_Model = instanceinfo.m_Transform;
		/*}*/
	/*}*/
		}
		std::sort(m_InstanceVector.begin(), m_InstanceVector.end(),
			[](const Instance2D& a, const Instance2D& b) {
				return a.m_LayerID.x < b.m_LayerID.x;
			});
	}



}