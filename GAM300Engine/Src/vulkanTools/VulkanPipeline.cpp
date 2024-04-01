#include "vulkanTools/VulkanPipeline.h"
#include "vulkanTools/VulkanHelper.h"
#include "Shader/ShaderLoader.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/vulkanInstance.h"
#include "vulkanTools/Renderer.h"
#include "vulkanTools/FrameBuffer.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "vulkanTools/VulkanTexture.h"
#include "Rendering/renderPass.h"
#include "Rendering/Revamped/FrameBuffers/FrameBufferObject.h"
namespace TDS
{
	VulkanPipeline::VulkanPipeline()
	{
	}
	VulkanPipeline::~VulkanPipeline()
	{
	}
	bool VulkanPipeline::Create(PipelineCreateEntry& createEntry)
	{
		if (createEntry.m_NumDescriptorSets == 0)
		{
			TDS_ERROR("Invalid descriptor entry!\n");
			return false;
		}
		m_PipelineEntry = createEntry;


		for (auto Shader : m_PipelineEntry.m_ShaderInputs.m_Shaders)
		{
			LoadShader(Shader.second, Shader.first);
		}

		if (m_PipelineEntry.m_PipelineConfig.m_SrcClrBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			m_PipelineEntry.m_PipelineConfig.m_DstClrBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			m_PipelineEntry.m_PipelineConfig.m_SrcAlphaBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO ||
			m_PipelineEntry.m_PipelineConfig.m_DstAlphaBlend != VkBlendFactor::VK_BLEND_FACTOR_ZERO)
		{
			m_BlendingEnabled = true;
		}



		GraphicsManager& mgr = GraphicsManager::getInstance();


		/*m_RenderTarget = GraphicsManager::getInstance().GetSwapchainRenderer().getSwapChainRenderPass();*/

		CreateDescriptors(m_PipelineEntry.m_ShaderInputs, m_PipelineEntry.m_NumDescriptorSets);

		std::vector<VkDescriptorSetLayout> layouts;
		layouts.push_back(m_DescSetLayout);
		//Just check the first one
		if (!m_PipelineDescriptor[0].m_TextureOrBindless.empty())
		{
			layouts.push_back(m_ArrayTextureLayout);
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCI = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

		pipelineLayoutCI.pNext = nullptr;
		pipelineLayoutCI.setLayoutCount = std::uint32_t(layouts.size());
		pipelineLayoutCI.pSetLayouts = layouts.data();
		pipelineLayoutCI.pushConstantRangeCount = static_cast<uint32_t>(m_ShaderLoadedData.m_VkPushConstantRanges.size());
		pipelineLayoutCI.pPushConstantRanges = m_ShaderLoadedData.m_VkPushConstantRanges.data();


		VK_ASSERT(vkCreatePipelineLayout(mgr.getVkInstance().getVkLogicalDevice(), &pipelineLayoutCI, nullptr, &m_PipelineLayout), "Failed to create pipeline layout!\n");


		if (bool isComputeShader = (SHADER_FLAG::COMPUTE_SHADER & createEntry.m_ShaderInputs.m_Shaders.begin()->first) != 0)
		{
			VkComputePipelineCreateInfo computePipelineCreateInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
			computePipelineCreateInfo.layout = m_PipelineLayout;
			computePipelineCreateInfo.flags = 0;
			computePipelineCreateInfo.stage = m_ShaderLoadedData.m_VkPipelineShaderStages[0];

			VkPipelineCacheCreateInfo pipelineCacheCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };

			VK_ASSERT(vkCreatePipelineCache(mgr.getVkInstance().getVkLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &m_ComputePipeline.m_Cache), "Failed to create compute Pipeline!\n");
			VK_ASSERT(vkCreateComputePipelines(mgr.getVkInstance().getVkLogicalDevice(), m_ComputePipeline.m_Cache, 1, &computePipelineCreateInfo, nullptr, &m_ComputePipeline.m_Pipeline), "Failed to create compute Pipeline cache!\n");

		}
		else
		{
			for (auto primitiveMode : m_PipelineEntry.m_PipelineConfig.m_PipelineDrawModes)
				GeneratePipeline(primitiveMode);

		}
		DestroyModules();

		return true;

	}

	void VulkanPipeline::GeneratePipeline(VkPrimitiveTopology drawMode)
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssemblyState.topology = drawMode;
		inputAssemblyState.primitiveRestartEnable = m_PipelineEntry.m_PipelineConfig.m_EnablePrimitiveRestart;

		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceFeatures(GraphicsManager::getInstance().getVkInstance().getVkPhysicalDevice(), &features);

		VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizationState.polygonMode = m_PipelineEntry.m_PipelineConfig.m_PolygonMode;
		rasterizationState.cullMode = m_PipelineEntry.m_PipelineConfig.m_CullMode;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.depthBiasEnable = m_PipelineEntry.m_PipelineConfig.m_EnableDepthBiased;
		rasterizationState.lineWidth = 1.0f;

		std::uint32_t count = 0;
		std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentState;

		if (m_PipelineEntry.m_FBTarget == nullptr && m_PipelineEntry.m_UseSwapchain == false)
		{
			TDS_ERROR("Something is wrong here? You are not rendering to swapchain but also dont have an framebuffer object?");
			__debugbreak();
		}



		if (m_PipelineEntry.m_UseSwapchain == false)
		{

			m_PipelineEntry.m_FBTarget->GetBlendAttachments(blendAttachmentState, m_PipelineEntry);
			m_RenderTarget = m_PipelineEntry.m_FBTarget->GetRenderPass()->getRenderPass();
			count = 2;

			/*blendAttachmentState.resize(count);
			for (int i{ 0 }; i < blendAttachmentState.size(); ++i)
			{
				blendAttachmentState[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				if (i == 1)
				{
					blendAttachmentState[i].blendEnable = VK_FALSE;
				}
				else
				{
					blendAttachmentState[i].blendEnable = m_BlendingEnabled ? VK_TRUE : VK_FALSE;
				}
				blendAttachmentState[i].srcColorBlendFactor = m_PipelineEntry.m_PipelineConfig.m_SrcClrBlend;
				blendAttachmentState[i].dstColorBlendFactor = m_PipelineEntry.m_PipelineConfig.m_DstClrBlend;
				blendAttachmentState[i].colorBlendOp = m_PipelineEntry.m_PipelineConfig.m_ColorBlend;
				blendAttachmentState[i].srcAlphaBlendFactor = m_PipelineEntry.m_PipelineConfig.m_SrcAlphaBlend;
				blendAttachmentState[i].dstAlphaBlendFactor = m_PipelineEntry.m_PipelineConfig.m_DstAlphaBlend;
				blendAttachmentState[i].alphaBlendOp = m_PipelineEntry.m_PipelineConfig.m_AlphaBlend;

			}*/
		}
		else
		{


			count = 1;
			blendAttachmentState.resize(count);
			for (int i{ 0 }; i < blendAttachmentState.size(); ++i)
			{
				blendAttachmentState[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				blendAttachmentState[i].blendEnable = m_BlendingEnabled ? VK_TRUE : VK_FALSE;
				blendAttachmentState[i].srcColorBlendFactor = m_PipelineEntry.m_PipelineConfig.m_SrcClrBlend;
				blendAttachmentState[i].dstColorBlendFactor = m_PipelineEntry.m_PipelineConfig.m_DstClrBlend;
				blendAttachmentState[i].colorBlendOp = m_PipelineEntry.m_PipelineConfig.m_ColorBlend;
				blendAttachmentState[i].srcAlphaBlendFactor = m_PipelineEntry.m_PipelineConfig.m_SrcAlphaBlend;
				blendAttachmentState[i].dstAlphaBlendFactor = m_PipelineEntry.m_PipelineConfig.m_DstAlphaBlend;
				blendAttachmentState[i].alphaBlendOp = m_PipelineEntry.m_PipelineConfig.m_AlphaBlend;

			}
		}
		if (m_RenderTarget == nullptr)
		{
			TDS_ERROR("U have no renderpass?");
			__debugbreak();

		}

		VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlendState.attachmentCount = static_cast<uint32_t>(blendAttachmentState.size());
		colorBlendState.pAttachments = blendAttachmentState.data();


		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;


		std::vector<VkDynamicState> dynamicStateEnables;

		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
		if (m_PipelineEntry.m_PipelineConfig.m_EnableDepthBiased)
			dynamicStateEnables.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);


		VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicState.pDynamicStates = dynamicStateEnables.data();
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

		VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depthStencilState.depthTestEnable = m_PipelineEntry.m_PipelineConfig.m_EnableDepthTest ? VK_TRUE : VK_FALSE;
		depthStencilState.depthWriteEnable = m_PipelineEntry.m_PipelineConfig.m_EnableDepthWrite ? VK_TRUE : VK_FALSE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.depthBoundsTestEnable = m_PipelineEntry.m_PipelineConfig.m_EnableDepthBiased ? VK_TRUE : VK_FALSE;
		depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.minDepthBounds = m_PipelineEntry.m_PipelineConfig.m_MinDepth;
		depthStencilState.maxDepthBounds = m_PipelineEntry.m_PipelineConfig.m_MaxDepth;

		//Set multisampling state, lemme know if you guys are doing multi samlping from your frame buffer and I will set this accordingly.
		VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		//VkSampleCountFlagBits MSAAbits = m_PipelineEntry.m_FBTarget[GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex()]->getFBEntryInfo().m_SamplingCnt;
		//if (MSAAbits != VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT)
		//{
		//	multisampleState.rasterizationSamples = MSAAbits;
		//	multisampleState.sampleShadingEnable = VK_TRUE;
		//	multisampleState.minSampleShading = 0.2f;
		//	multisampleState.pSampleMask = nullptr;
		//}

		std::uint32_t numInputVertex = std::uint32_t(m_PipelineEntry.m_ShaderInputs.m_InputVertex.size());
		m_inputBindings.resize(numInputVertex);


		std::uint32_t location = 0;
		std::uint32_t bindingIndex = 0;




		for (; bindingIndex < numInputVertex; ++bindingIndex)
		{
			m_inputBindings[bindingIndex].binding = bindingIndex;
			m_inputBindings[bindingIndex].stride = m_PipelineEntry.m_ShaderInputs.m_InputVertex[bindingIndex].m_Stride;
			m_inputBindings[bindingIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			if (m_PipelineEntry.m_ShaderInputs.m_InputVertex[bindingIndex].m_InstanceInput)
				m_inputBindings[bindingIndex].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

			for (auto& elem : m_PipelineEntry.m_ShaderInputs.m_InputVertex[bindingIndex].m_Layout.m_MemberElements)
			{
				if (elem.m_DataType == VAR_TYPE::MAT4 || elem.m_DataType == VAR_TYPE::MAT3)
				{
					std::uint32_t Prevoffset = (location == 0) ? 0 : m_InputAttributes[location - 1].offset;
					GenerateMatrixInputAttribute(m_InputAttributes, Prevoffset, location, bindingIndex, elem);
				}
				else
					m_InputAttributes.push_back(GenerateVectorInputAttribute(location, bindingIndex, elem));
			}
		}


		VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };


		vertexInputStateInfo.vertexBindingDescriptionCount = std::uint32_t(m_inputBindings.size());
		vertexInputStateInfo.pVertexBindingDescriptions = m_inputBindings.data();
		vertexInputStateInfo.vertexAttributeDescriptionCount = std::uint32_t(m_InputAttributes.size());
		vertexInputStateInfo.pVertexAttributeDescriptions = m_InputAttributes.data();


		VkGraphicsPipelineCreateInfo graphicspipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };


		graphicspipelineCreateInfo.layout = m_PipelineLayout;
		graphicspipelineCreateInfo.renderPass = m_RenderTarget;
		graphicspipelineCreateInfo.stageCount = std::uint32_t(m_ShaderLoadedData.m_VkPipelineShaderStages.size());
		graphicspipelineCreateInfo.pStages = m_ShaderLoadedData.m_VkPipelineShaderStages.data();
		graphicspipelineCreateInfo.pVertexInputState = &vertexInputStateInfo;
		graphicspipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		graphicspipelineCreateInfo.pColorBlendState = &colorBlendState;
		graphicspipelineCreateInfo.pRasterizationState = &rasterizationState;
		graphicspipelineCreateInfo.pMultisampleState = &multisampleState;
		graphicspipelineCreateInfo.pViewportState = &viewportState;
		graphicspipelineCreateInfo.pDepthStencilState = &depthStencilState;
		graphicspipelineCreateInfo.pDynamicState = &dynamicState;


		VkPipelineCacheCreateInfo pipelineCacheCI = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };

		VkPipelineCache pipelinecache = nullptr;
		VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		VK_ASSERT(vkCreatePipelineCache(device, &pipelineCacheCI, nullptr, &pipelinecache), "Failed to create pipeline cache!");
		m_Caches[drawMode] = pipelinecache;

		VK_ASSERT(vkCreateGraphicsPipelines(device, m_Caches[drawMode], 1, &graphicspipelineCreateInfo, 0, &m_Pipelines[drawMode]),
			"Failed to create graphics pipeline!");

		m_PipelineEntry.m_PipelineName += "_pipeline.cached";
		SavePipelineCache(m_PipelineEntry.m_PipelineName, drawMode);

		m_InputAttributes.clear();
		m_inputBindings.clear();

	}
	void VulkanPipeline::SetClearColor(iColor clearColor)
	{
		(clearColor);

	}



	void VulkanPipeline::ShutDown()
	{
		VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		for (auto& cache : m_Caches)
			vkDestroyPipelineCache(device, cache.second, 0);

		m_Caches.clear();
		for (auto& pipeline : m_Pipelines)
			vkDestroyPipeline(device, pipeline.second, 0);

		m_Pipelines.clear();
		if (m_PipelineLayout)
		{
			vkDestroyPipelineLayout(device, m_PipelineLayout, 0);
			m_PipelineLayout = nullptr;
		}
		FreeDescriptors();
		if (m_DescriptorPool)
		{
			vkDestroyDescriptorPool(device, m_DescriptorPool, 0);
			m_DescriptorPool = 0;
		}
		m_RenderTarget = nullptr;

	}
	void VulkanPipeline::BindComputePipeline()
	{
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline.m_Pipeline);
	}
	bool VulkanPipeline::LoadCachedPieline(std::string_view fileName, VkPrimitiveTopology drawMode)
	{
		return false;
	}
	void VulkanPipeline::DispatchCompute(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountz)
	{
		vkCmdDispatch(m_CommandBuffer, groupCountX, groupCountY, groupCountz);
	}

	bool VulkanPipeline::SavePipelineCache(std::string_view fileName, VkPrimitiveTopology drawMode)
	{
		VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		std::ofstream file(fileName.data(), std::ios::binary);
		if (file.is_open())
		{
			size_t size = 0;

			vkGetPipelineCacheData(device, m_Caches[drawMode], &size, nullptr);
			std::vector<char> data(size);
			vkGetPipelineCacheData(device, m_Caches[drawMode], &size, data.data());

			file.write(data.data(), size);
			if (!file.good())
			{
				TDS_ERROR("Failed to save pipeline cache!");
				return false;
			}

			return true;
		}
		TDS_ERROR("Invalid Directory... Failed to save pipeline cache!");
		return false;
	}

	void VulkanPipeline::CreateDescriptors(ShaderInputs& shaderInputs, std::uint32_t numDescriptorSets)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		ShaderMetaData& reflectedMeta = ShaderLoader::GetInstance()->getReflectedLookUp();
		for (auto& shader : shaderInputs.m_Shaders)
		{
			std::filesystem::path full_path(shader.second.data());
			std::size_t size = reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_UniformBuffers.size();
			if (!reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_UniformBuffers.empty())
			{
				std::uint32_t numUniform{0}, numStorageBuffer{ 0 };

				for (auto& uniformPair : reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_UniformBuffers)
				{
					if (uniformPair.second.m_BufferType == BUFFER_TYPE::UNIFORM)
						++numUniform;
					else
						++numStorageBuffer;
				}

				if (numUniform)
				{
					VkDescriptorPoolSize poolSize = {};
					if (m_PipelineEntry.m_EnableDoubleBuffering)
						poolSize.descriptorCount = numUniform * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
					else
						poolSize.descriptorCount = numUniform;
					poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					poolSizes.push_back(poolSize);
				}

				if (numStorageBuffer)
				{
					VkDescriptorPoolSize poolSize = {};
					if (m_PipelineEntry.m_EnableDoubleBuffering)
						poolSize.descriptorCount = numStorageBuffer * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
					else
						poolSize.descriptorCount = numStorageBuffer;
					poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					poolSizes.push_back(poolSize);
				}

			}

			if (!reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_ImageSamplers.empty())
			{
				std::uint32_t NumSamplers = 0;
				for (auto& sampler : reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_ImageSamplers)
				{
					NumSamplers += sampler.second.m_ArraySize == 0 ? 1 : sampler.second.m_ArraySize;
				}
				VkDescriptorPoolSize poolSize{};
				if (m_PipelineEntry.m_EnableDoubleBuffering)
					poolSize.descriptorCount = NumSamplers /** VulkanSwapChain::MAX_FRAMES_IN_FLIGHT*/;
				else
					poolSize.descriptorCount = NumSamplers;
				poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;


				poolSizes.push_back(poolSize);
			}

			if (!reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_StorageImages.empty())
			{
				std::uint32_t storageImagesSamplers = 0;
				for (auto& storage : reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_StorageImages)
				{
					storageImagesSamplers += storage.second.m_ArraySize == 0 ? 1 : storage.second.m_ArraySize;
				}
				VkDescriptorPoolSize poolSize{};
				if (m_PipelineEntry.m_EnableDoubleBuffering)
					poolSize.descriptorCount = storageImagesSamplers/* * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT*/;
				else
					poolSize.descriptorCount = storageImagesSamplers;
				poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				poolSizes.push_back(poolSize);
			}
		}
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};

		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = nullptr;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		if (m_PipelineEntry.m_EnableDoubleBuffering)
			descriptorPoolInfo.maxSets = 2 * numDescriptorSets * VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
		else
			descriptorPoolInfo.maxSets = numDescriptorSets;
		VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		if (m_DescriptorPool)
		{
			vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
		}
		VK_ASSERT(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool), "Failed to create descriptor Pool!");


		m_PipelineDescriptor.resize(numDescriptorSets);
		std::uint32_t index = 0;
		for (auto& descriptor : m_PipelineDescriptor)
		{
			CreateDescriptorSet(shaderInputs, descriptor, index);
			CreateUniformBuffers(shaderInputs, descriptor, index);
			CreateSamplerDescriptors(shaderInputs, descriptor);
			++index;
		}



	}

	void VulkanPipeline::Draw(std::uint32_t vertexCnt, std::uint32_t frameIndex, std::uint32_t instanceCnt, std::uint32_t firstVertex, std::uint32_t firstInstance)
	{
		(frameIndex);
		/*vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_PipelineDescriptor.m_DescriptorSets[frameIndex], 0, nullptr);*/
		vkCmdDraw(m_CommandBuffer, vertexCnt, instanceCnt, firstVertex, firstInstance);
	}
	void VulkanPipeline::DrawIndexed(VMABuffer& vertexBuffer, VMABuffer& indexBuffer, std::uint32_t frameIndex)
	{
		(vertexBuffer);
		(frameIndex);
		/*vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_PipelineDescriptor.m_DescriptorSets[frameIndex], 0, nullptr);*/
		vkCmdDrawIndexed(m_CommandBuffer, indexBuffer.getDataCount(), 1, 0, 0, 1);
	}

	void VulkanPipeline::DrawInstanced(VMABuffer& vertexBuffer, std::uint32_t instance, std::uint32_t frameIndex)
	{
		(vertexBuffer);
		(frameIndex);
		/*vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_PipelineDescriptor.m_DescriptorSets[frameIndex], 0, nullptr);*/
		vkCmdDraw(m_CommandBuffer, vertexBuffer.getDataCount(), instance, 0, 0);
	}

	void VulkanPipeline::DrawInstancedIndexed(VMABuffer& vertexBuffer, VMABuffer& indexBuffer, std::uint32_t instance, std::uint32_t frameIndex)
	{
		(vertexBuffer);
		vkCmdDrawIndexed(m_CommandBuffer, indexBuffer.getDataCount(), instance, 0, 0, 0);
	}

	void VulkanPipeline::SubmitPushConstant(void* data, size_t size, std::int32_t flags)
	{
		VkShaderStageFlags stage = GetShaderFlag(flags);
		vkCmdPushConstants(m_CommandBuffer, m_PipelineLayout, stage, 0, std::uint32_t(size), data);
	}
	void VulkanPipeline::UpdateUBO(void* data, size_t size, std::uint32_t binding, std::uint32_t frameIndex, std::uint32_t offset, bool readOnly, std::uint32_t DescIndex)
	{
		if (GlobalBufferPool::GetInstance()->BindingExist(binding))
		{
			std::vector<std::shared_ptr<UBO>>* pUBOs = GlobalBufferPool::GetInstance()->GetBufferContainer(binding, DescIndex);
			if (pUBOs)
			{
				if (readOnly == false)
					pUBOs->at(frameIndex)->m_Buffer->MapData(data, size, offset);
				else
					pUBOs->at(frameIndex)->m_Buffer->ReadData(data, size, offset);
				return;
			}
		}
		auto findItr = m_PipelineDescriptor[DescIndex].m_UpdateBufferFrames.find(binding);
		if (findItr != m_PipelineDescriptor[DescIndex].m_UpdateBufferFrames.end())
		{
			if (readOnly == false)
				findItr->second.at(frameIndex)->m_Buffer->MapData(data, size, offset);
			else
				findItr->second.at(frameIndex)->m_Buffer->ReadData(data, size, offset);
			return;
		}
		TDS_WARN("Buffer binding: {} , doesnt exist!", binding);



	}
	//void VulkanPipeline::UpdateTextureArray(std::uint32_t binding, VkDescriptorType descriptorType, std::vector<VulkanTexture*>& texture)
	//{
	//	std::int32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
	//	bool invalid = false;
	//	auto findItr = m_PipelineDescriptor.m_WriteSetFrames.find(binding);
	//	if (findItr != m_PipelineDescriptor.m_WriteSetFrames.end())
	//	{
	//		VkWriteDescriptorSet write{};
	//		std::uint32_t ArrayCnt = std::uint32_t(texture.size());
	//		std::vector<VkDescriptorImageInfo> Infos(ArrayCnt);
	//		for (std::uint32_t i = 0; i < ArrayCnt; ++i)
	//		{
	//			if (!texture[i]->GetImage())
	//			{
	//				TDS_WARN("Invalid texture is found!\n");
	//				invalid = true;
	//				break;
	//			}
	//			Infos[i] = texture[i]->getInfo();

	//		}

	//		if (invalid)
	//		{
	//			TDS_INFO("Replacing with default texture...\n");
	//			for (auto& info : Infos)
	//			{
	//				//I will replace all textures with default texture...
	//				info = DefaultTextures::GetDefaultTexture()->getInfo();
	//			}
	//		}

	//		findItr->second.dstSet = m_PipelineDescriptor.m_DescriptorSets[frame];
	//		findItr->second.descriptorType = descriptorType;
	//		findItr->second.dstBinding = binding;
	//		findItr->second.dstArrayElement = 0;
	//		findItr->second.descriptorCount = static_cast<uint32_t>(Infos.size());
	//		findItr->second.pImageInfo = Infos.data();

	//		vkUpdateDescriptorSets(GraphicsManager::getInstance().getInstance().getVkInstance().getVkLogicalDevice()
	//			, 1, &findItr->second, 0, 0);
	//	}
	//}
	void VulkanPipeline::UpdateTextureArray(std::uint32_t binding, VkDescriptorType descriptorType, std::vector<Texture*>& texture, std::uint32_t DescIndex)
	{
		std::int32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		bool invalid = false;
		auto findItr = m_PipelineDescriptor[DescIndex].m_WriteSetFrames.find(binding);
		if (findItr != m_PipelineDescriptor[DescIndex].m_WriteSetFrames.end())
		{
			VkWriteDescriptorSet write{};
			std::uint32_t ArrayCnt = std::uint32_t(texture.size());
			std::vector<VkDescriptorImageInfo> Infos(ArrayCnt);
			for (std::uint32_t i = 0; i < ArrayCnt; ++i)
			{
				if (texture[i]->m_VulkanTexture == nullptr)
				{
					TDS_WARN("Invalid texture is found!\n");
					Infos[i] = DefaultTextures::GetDefaultTexture()->getInfo();
				}
				else
				{
					Infos[i] = texture[i]->m_VulkanTexture->getInfo();
				}

			}

			findItr->second.dstSet = m_PipelineDescriptor[DescIndex].m_DescriptorSets[frame];
			findItr->second.descriptorType = descriptorType;
			findItr->second.dstBinding = binding;
			findItr->second.dstArrayElement = 0;
			findItr->second.descriptorCount = static_cast<uint32_t>(Infos.size());
			findItr->second.pImageInfo = Infos.data();

			vkUpdateDescriptorSets(GraphicsManager::getInstance().getInstance().getVkInstance().getVkLogicalDevice()
				, 1, &findItr->second, 0, 0);
		}
	}
	void VulkanPipeline::UpdateTextureArray(std::uint32_t binding, VkDescriptorType descriptorType, std::array<Texture, 1000>& texture, std::uint32_t DescIndex)
	{
		std::int32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		bool invalid = false;
		auto findItr = m_PipelineDescriptor[DescIndex].m_WriteSetFrames.find(binding);
		if (findItr != m_PipelineDescriptor[DescIndex].m_WriteSetFrames.end())
		{
			VkWriteDescriptorSet write{};
			std::uint32_t ArrayCnt = std::uint32_t(texture.size());
			std::array<VkDescriptorImageInfo, 1000> Infos;
			for (std::uint32_t i = 0; i < ArrayCnt; ++i)
			{
				if (texture[i].m_VulkanTexture == nullptr)
				{
					Infos[i] = DefaultTextures::GetDefaultTexture()->getInfo();
				}
				else
				{
					Infos[i] = texture[i].m_VulkanTexture->getInfo();
				}

			}

			for (int i = 0; i < 1; ++i)
			{
				findItr->second.dstSet = m_PipelineDescriptor[DescIndex].m_TextureOrBindless[i];
				findItr->second.descriptorType = descriptorType;
				findItr->second.dstBinding = binding;
				findItr->second.dstArrayElement = 0;
				findItr->second.descriptorCount = static_cast<uint32_t>(Infos.size());
				findItr->second.pImageInfo = Infos.data();

				vkUpdateDescriptorSets(GraphicsManager::getInstance().getInstance().getVkInstance().getVkLogicalDevice()
					, 1, &findItr->second, 0, 0);
			}

		}
	}
	void VulkanPipeline::UpdateTexture(std::uint32_t binding, VkDescriptorType descriptorType, VulkanTexture& texture)
	{
		UpdateDescriptor(texture.getInfo(), descriptorType, binding);
	}
	void VulkanPipeline::BindPipeline(VkPrimitiveTopology drawMode)
	{
		vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipelines[drawMode]);
	}
	void VulkanPipeline::BindDescriptor(std::int32_t frame, std::uint32_t numOfSet, std::uint32_t firstSet, bool Compute, std::uint32_t DescIndex)
	{
		VkPipelineBindPoint bindingPoint = Compute ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS;
		vkCmdBindDescriptorSets(m_CommandBuffer, bindingPoint, m_PipelineLayout, firstSet, numOfSet, &m_PipelineDescriptor[DescIndex].m_DescriptorSets[frame], 0, nullptr);
	}
	void VulkanPipeline::BindAllDescriptors(std::int32_t frame)
	{
	}
	void VulkanPipeline::BindDescriptorSet(VkDescriptorSet descriptorSet)
	{
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
	}

	void VulkanPipeline::BindArrayDescriptorSet(std::uint32_t FrameIndex, std::uint32_t numOfSet, std::uint32_t firstSet, std::uint32_t DescIndex)
	{
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, firstSet, numOfSet, &m_PipelineDescriptor[DescIndex].m_TextureOrBindless[FrameIndex], 0, nullptr);
	}

	void VulkanPipeline::BindVertexBuffer(VMABuffer& vertexBuffer)
	{

		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &vertexBuffer.GetBuffer(), offsets);

	}
	void VulkanPipeline::BindIndexBuffer(VMABuffer& IndexBuffer)
	{
		vkCmdBindIndexBuffer(m_CommandBuffer, IndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
	void VulkanPipeline::LoadShader(std::string_view shaderPath, SHADER_FLAG shaderFlag)
	{
		GraphicsManager& mgr = GraphicsManager::getInstance();
		std::vector<std::uint32_t> binaries;
		ShaderLoader::LoadShader(binaries, shaderPath);
		VkShaderStageFlagBits vkStage = ShaderFlagsToVkStage(shaderFlag);


		VkShaderModule module = nullptr;

		VkShaderModuleCreateInfo shaderModuleCI = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		shaderModuleCI.codeSize = binaries.size() * sizeof(uint32_t);
		shaderModuleCI.pCode = binaries.data();

		VK_ASSERT(vkCreateShaderModule(mgr.getVkInstance().getVkLogicalDevice(), &shaderModuleCI, nullptr, &module), "Failed to Create Shader Module");

		VkPipelineShaderStageCreateInfo pipelineShaderStageCI{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		pipelineShaderStageCI.stage = vkStage;
		pipelineShaderStageCI.pName = "main";
		pipelineShaderStageCI.module = module;

		m_ShaderLoadedData.m_ShaderModules[shaderFlag] = module;
		m_ShaderLoadedData.m_VkPipelineShaderStages.push_back(pipelineShaderStageCI);
		std::filesystem::path full_path(shaderPath.data());
		ShaderData& reflected = ShaderLoader::GetInstance()->getReflectedLookUp().m_ShaderDatas[full_path.filename().string()];

		if (reflected.m_ReflectedData.m_PushConstant.m_Size > 0)
		{
			VkPushConstantRange range = {};
			range.offset = reflected.m_ReflectedData.m_PushConstant.m_Offset;
			range.size = reflected.m_ReflectedData.m_PushConstant.m_Size;
			range.stageFlags = ShaderFlagsToVkStage(shaderFlag);

			m_ShaderLoadedData.m_VkPushConstantRanges.push_back(range);
		}
	}
	VkPipeline& VulkanPipeline::GetPipeline(VkPrimitiveTopology drawMode)
	{
		return m_Pipelines[drawMode];
	}
	VkPipelineLayout& VulkanPipeline::GetLayout()
	{
		return m_PipelineLayout;
	}
	VkDescriptorPool& VulkanPipeline::GetDescriptorPool()
	{
		return m_DescriptorPool;
	}
	bool VulkanPipeline::IsBlendEnabled()
	{
		return m_BlendingEnabled;
	}

	VulkanPipelineDescriptor& VulkanPipeline::GetPipelineDescriptor(std::uint32_t DescIndex)
	{
		return m_PipelineDescriptor[DescIndex];
	}
	PipelineCreateEntry& VulkanPipeline::GetCreateEntry()
	{
		return m_PipelineEntry;
	}
	VkCommandBuffer& VulkanPipeline::GetCommandBuffer() {
		return m_CommandBuffer;
	}

	std::uint32_t VulkanPipeline::GetBufferBinding(std::string_view bufferName, std::uint32_t descIndex)
	{
		if (GlobalBufferPool::GetInstance()->BindingExist(bufferName))
			return GlobalBufferPool::GetInstance()->GetBinding(bufferName.data());


		auto itr = m_PipelineDescriptor[descIndex].m_LocalBufferNames.find(bufferName.data());
		if (itr != m_PipelineDescriptor[descIndex].m_LocalBufferNames.end())
			return itr->second;



	}
	void VulkanPipeline::SetCommandBuffer(VkCommandBuffer& buffer)
	{
		m_CommandBuffer = buffer;
	}
	void VulkanPipeline::FreeDescriptors()
	{
		VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();
		


		if (m_DescSetLayout)
		{
			vkDestroyDescriptorSetLayout(device, m_DescSetLayout, nullptr);
			m_DescSetLayout = nullptr;
		}
		if (m_ArrayTextureLayout)
		{
			vkDestroyDescriptorSetLayout(device, m_ArrayTextureLayout, nullptr);
			m_ArrayTextureLayout = nullptr;
		}

		for (auto& desc : m_PipelineDescriptor)
		{
			desc.m_UpdateBufferFrames.clear();
			desc.m_StaticBuffers.clear();
		}
	}
	void VulkanPipeline::CreateDescriptorSet(ShaderInputs& shaderInput, VulkanPipelineDescriptor& descriptor, std::uint32_t descIndex)
	{
		bool UseTextureArrayLayout = false;
		std::vector<VkDescriptorSetLayoutBinding> layouts;
		std::vector< VkDescriptorSetLayoutBinding> textureArrayLayout;
		std::set<std::uint32_t> binded;
		for (auto& shader : shaderInput.m_Shaders)
		{
			std::filesystem::path full_path(shader.second.data());
			ShaderMetaData& metaData = ShaderLoader::GetInstance()->getReflectedLookUp();
			for (auto& uniforms : metaData.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_UniformBuffers)
			{
				bool Global = GlobalBufferPool::GetInstance()->BindingExist(uniforms.second.m_BindingPoint, descIndex);

				if (binded.find(uniforms.second.m_BindingPoint) != binded.end())
				{
					continue;
				}

				VkDescriptorSetLayoutBinding layoutBinding = {};
				layoutBinding.binding = uniforms.second.m_BindingPoint;
				layoutBinding.descriptorType = uniforms.second.m_BufferType == BUFFER_TYPE::UNIFORM ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = Global ? VkShaderStageFlagBits::VK_SHADER_STAGE_ALL : ShaderFlagsToVkStage(shader.first);
				layouts.push_back(layoutBinding);
				binded.insert(uniforms.second.m_BindingPoint);
			}

			for (auto& sampler : metaData.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_ImageSamplers)
			{
				VkDescriptorSetLayoutBinding layoutBinding = {};
				layoutBinding.binding = sampler.second.m_BindingPoint;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.stageFlags = ShaderFlagsToVkStage(shader.first);
				layoutBinding.descriptorCount = 1;
				if (sampler.second.m_ArraySize > 1)
				{
					UseTextureArrayLayout = true;
					layoutBinding.descriptorCount = sampler.second.m_ArraySize;
					textureArrayLayout.push_back(layoutBinding);
				}
				else
				{
					layouts.push_back(layoutBinding);
				}

			}
			for (auto& storageIMage : metaData.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_StorageImages)
			{

				VkDescriptorSetLayoutBinding layoutBinding = {};

				layoutBinding.binding = storageIMage.second.m_BindingPoint;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				layoutBinding.descriptorCount = storageIMage.second.m_ArraySize == 0 ? 1 : storageIMage.second.m_ArraySize;
				layoutBinding.stageFlags = ShaderFlagsToVkStage(shader.first);
				if (storageIMage.second.m_ArraySize > 1)
				{
					UseTextureArrayLayout = true;
					layoutBinding.descriptorCount = storageIMage.second.m_ArraySize;
					textureArrayLayout.push_back(layoutBinding);
				}
				else
				{
					layouts.push_back(layoutBinding);
				}

			}

		}
		std::vector<VkDescriptorSetLayout> TextureArrayLayouts;
		std::vector<VkDescriptorSetLayout> DescSetlayouts;
		VkDescriptorSetLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutInfo.bindingCount = static_cast<uint32_t>(layouts.size());
		layoutInfo.pBindings = layouts.data();
		VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();

		if (m_DescSetLayout == 0)
			VK_ASSERT(vkCreateDescriptorSetLayout(device, &layoutInfo, 0, &m_DescSetLayout), "Failed to create Descriptor Layout!");

		DescSetlayouts.push_back(m_DescSetLayout);
		if (UseTextureArrayLayout)
		{
			VkDescriptorSetLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			layoutInfo.bindingCount = static_cast<uint32_t>(textureArrayLayout.size());
			layoutInfo.pBindings = textureArrayLayout.data();
			VkDevice device = GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice();

			if (m_ArrayTextureLayout == 0)
				VK_ASSERT(vkCreateDescriptorSetLayout(device, &layoutInfo, 0, &m_ArrayTextureLayout), "Failed to create Descriptor Layout!");
			TextureArrayLayouts.push_back(m_ArrayTextureLayout);
		}

		VkDescriptorSetAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };

		allocateInfo.descriptorPool = m_DescriptorPool;
		allocateInfo.descriptorSetCount = std::uint32_t(DescSetlayouts.size());
		allocateInfo.pSetLayouts = DescSetlayouts.data();
		if (m_PipelineEntry.m_EnableDoubleBuffering)
		{
			descriptor.m_DescriptorSets.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
			if (UseTextureArrayLayout)
				descriptor.m_TextureOrBindless.resize(1);
		}
		else
		{
			descriptor.m_DescriptorSets.resize(1);
			if (UseTextureArrayLayout)
				descriptor.m_TextureOrBindless.resize(1);
		}
		for (auto& descSet : descriptor.m_DescriptorSets)
		{
			VkResult result = vkAllocateDescriptorSets(device, &allocateInfo, &descSet);
			VK_ASSERT(result, "Failed to allocate decsriptor set!");
		}
		for (auto& Set : descriptor.m_TextureOrBindless)
		{
			allocateInfo.descriptorPool = m_DescriptorPool;
			allocateInfo.descriptorSetCount = std::uint32_t(TextureArrayLayouts.size());
			allocateInfo.pSetLayouts = TextureArrayLayouts.data();
			VkResult result = vkAllocateDescriptorSets(device, &allocateInfo, &Set);
			VK_ASSERT(result, "Failed to allocate decsriptor set!");
		}

	}
	void VulkanPipeline::CreateUniformBuffers(ShaderInputs& shaderInput, VulkanPipelineDescriptor& descriptor, std::uint32_t descIndex)
	{
		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		ShaderMetaData& reflectedMeta = ShaderLoader::GetInstance()->getReflectedLookUp();
		for (auto& shader : shaderInput.m_Shaders)
		{
			std::filesystem::path full_path(shader.second.data());
			for (auto& uniform : reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_UniformBuffers)
			{
				bool GlobalBuffer = false;
				bool StaticBuffer = false;
				VkDescriptorType Desctype = uniform.second.m_BufferType == BUFFER_TYPE::UNIFORM ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				VkBufferUsageFlags usageFlags = uniform.second.m_BufferType == BUFFER_TYPE::UNIFORM ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
				size_t size = uniform.second.m_Size;
				const auto& itr = descriptor.m_UpdateBufferFrames.find(uniform.second.m_BindingPoint);
				if (itr == descriptor.m_UpdateBufferFrames.end())
				{
					//Before adding, we check if its a global buffer.
					//You should specify what is your global buffers before creating your pipelines. If not,
					//If not they will default to local buffers. Then u gonna waste memory and rendering cycles
					if (GlobalBufferPool::GetInstance()->BindingExist(uniform.second.m_BindingPoint, descIndex))
					{
						//TDS_INFO("This is a global buffer!\n");
						GlobalBuffer = true;

					}
					else
					{
						auto findUniform = m_PipelineEntry.m_ShaderInputs.m_InputBuffers.find(uniform.second.m_BindingPoint);
						if (findUniform != m_PipelineEntry.m_ShaderInputs.m_InputBuffers.end())
						{
							if (findUniform->second.m_Static && findUniform->second.m_Data != nullptr)
							{
								if (m_PipelineEntry.m_EnableDoubleBuffering)
									descriptor.m_StaticBuffers[uniform.second.m_BindingPoint].resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
								else
									descriptor.m_StaticBuffers[uniform.second.m_BindingPoint].resize(1);
								StaticBuffer = true;



								for (auto& buffers : descriptor.m_StaticBuffers[uniform.second.m_BindingPoint])
								{
									buffers = std::make_shared<UBO>();
									buffers->m_Buffer = std::make_shared<VMABuffer>();
									if (findUniform->second.m_Data != nullptr)
									{
										usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
										buffers->m_Buffer->MappedStaging(findUniform->second.m_Size, usageFlags, instance, findUniform->second.m_Data);
									}
									buffers->m_BufferInfo.buffer = buffers->m_Buffer->GetBuffer();
									buffers->m_BufferInfo.offset = 0;
									buffers->m_BufferInfo.range = buffers->m_Buffer->GetBufferSize();

								}
							}
							else
							{
								if (m_PipelineEntry.m_EnableDoubleBuffering)
									descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint].resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
								else
									descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint].resize(1);

								for (auto& buffers : descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint])
								{
									buffers = std::make_shared<UBO>();
									buffers->m_Buffer = std::make_shared<VMABuffer>();
									buffers->m_Buffer->CreateBuffer(findUniform->second.m_Size, usageFlags, VMA_MEMORY_USAGE_CPU_TO_GPU);
									buffers->m_BufferInfo.buffer = buffers->m_Buffer->GetBuffer();
									buffers->m_BufferInfo.offset = 0;
									buffers->m_BufferInfo.range = buffers->m_Buffer->GetBufferSize();


								}
							}

						}
						else
						{
							//I will assume to be non static
							if (m_PipelineEntry.m_EnableDoubleBuffering)
								descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint].resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
							else
								descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint].resize(1);

							for (auto& buffers : descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint])
							{
								buffers = std::make_shared<UBO>();
								buffers->m_Buffer = std::make_shared<VMABuffer>();
								buffers->m_Buffer->CreateBuffer(uniform.second.m_Size, usageFlags, VMA_MEMORY_USAGE_CPU_TO_GPU);
								buffers->m_BufferInfo.buffer = buffers->m_Buffer->GetBuffer();
								buffers->m_BufferInfo.offset = 0;
								buffers->m_BufferInfo.range = uniform.second.m_Size;
							}
						}
						descriptor.m_LocalBufferNames[uniform.second.m_InstanceName] = uniform.second.m_BindingPoint;
					}
				}
				VkWriteDescriptorSet writeSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
				if (GlobalBuffer == false)
				{
					if (StaticBuffer)
					{
						for (std::uint32_t i = 0; i < std::uint32_t(descriptor.m_StaticBuffers[uniform.second.m_BindingPoint].size()); ++i)
						{
							writeSet.dstSet = descriptor.m_DescriptorSets[i];
							writeSet.dstBinding = uniform.second.m_BindingPoint;
							writeSet.dstArrayElement = 0;
							writeSet.descriptorType = Desctype;
							writeSet.descriptorCount = 1;
							writeSet.pBufferInfo = &descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint][i]->m_BufferInfo;

							descriptor.m_WriteSetFrames[uniform.second.m_BindingPoint] = writeSet;
							vkUpdateDescriptorSets(instance.getVkLogicalDevice(), 1, &writeSet, 0, nullptr);
						}
					}
					else
					{
						for (std::uint32_t i = 0; i < std::uint32_t(descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint].size()); ++i)
						{
							writeSet.dstSet = descriptor.m_DescriptorSets[i];
							writeSet.dstBinding = uniform.second.m_BindingPoint;
							writeSet.dstArrayElement = 0;
							writeSet.descriptorType = Desctype;
							writeSet.descriptorCount = 1;
							writeSet.pBufferInfo = &descriptor.m_UpdateBufferFrames[uniform.second.m_BindingPoint][i]->m_BufferInfo;

							descriptor.m_WriteSetFrames[uniform.second.m_BindingPoint] = writeSet;
							vkUpdateDescriptorSets(instance.getVkLogicalDevice(), 1, &writeSet, 0, nullptr);
						}

					}
				}
				else
				{
					std::uint32_t GlobalFrameSize = (std::uint32_t)GlobalBufferPool::GetInstance()->GetBufferContainer(uniform.second.m_BindingPoint, descIndex)->size();
					for (std::uint32_t i = 0; i < GlobalFrameSize; ++i)
					{

						VkDescriptorBufferInfo bufferInfo = GlobalBufferPool::GetInstance()->GetBufferContainer(uniform.second.m_BindingPoint, descIndex)->at(i)->m_BufferInfo;

						writeSet.dstSet = descriptor.m_DescriptorSets[i];
						writeSet.dstBinding = uniform.second.m_BindingPoint;
						writeSet.dstArrayElement = 0;
						writeSet.descriptorType = Desctype;
						writeSet.descriptorCount = 1;
						writeSet.pBufferInfo = &bufferInfo;

						descriptor.m_WriteSetFrames[uniform.second.m_BindingPoint] = writeSet;
						vkUpdateDescriptorSets(instance.getVkLogicalDevice(), 1, &writeSet, 0, nullptr);
					}

				}


			}
		}
	}
	void VulkanPipeline::CreateSamplerDescriptors(ShaderInputs& shaderInput, VulkanPipelineDescriptor& descriptor)
	{

		VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
		ShaderMetaData& reflectedMeta = ShaderLoader::GetInstance()->getReflectedLookUp();
		for (auto& shader : shaderInput.m_Shaders)
		{
			std::filesystem::path full_path(shader.second.data());
			descriptor.m_ImageInfo = DefaultTextures::GetDefaultTexture()->getInfo();

			for (auto& samplers : reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_ImageSamplers)
			{

				VkWriteDescriptorSet writeSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };


				if (samplers.second.m_Dimension == 1 && samplers.second.m_ArraySize > 1)
				{
					std::vector<VkDescriptorImageInfo> infos(samplers.second.m_ArraySize);
					for (auto& desc : descriptor.m_TextureOrBindless)
					{
						writeSet.dstSet = desc;
						writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						writeSet.dstBinding = samplers.second.m_BindingPoint;
						writeSet.dstArrayElement = 0;
						for (std::uint32_t i = 0; i < samplers.second.m_ArraySize; ++i)
							infos[i] = descriptor.m_ImageInfo;
						writeSet.descriptorCount = std::uint32_t(infos.size());
						writeSet.pImageInfo = infos.data();
						vkUpdateDescriptorSets(instance.getVkLogicalDevice(), 1, &writeSet, 0, nullptr);
					}
					descriptor.m_WriteSetFrames[samplers.second.m_BindingPoint] = writeSet;
					descriptor.m_LocalBufferNames[samplers.second.m_Name] = samplers.second.m_BindingPoint;
				}
				else
				{

					for (auto& desc : descriptor.m_DescriptorSets)
					{
						if (samplers.second.m_Dimension == 3)
						{
							writeSet.dstSet = desc;
							writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							writeSet.dstBinding = samplers.second.m_BindingPoint;
							writeSet.dstArrayElement = 0;
							writeSet.descriptorCount = 1;
							writeSet.pImageInfo = &DefaultTextures::GetDefaultCubeTexture()->getInfo();

						}
						else if (samplers.second.m_ArraySize == 1 && samplers.second.m_Dimension == 1)
						{

							writeSet.dstSet = desc;
							writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							writeSet.dstBinding = samplers.second.m_BindingPoint;
							writeSet.dstArrayElement = 0;
							writeSet.descriptorCount = 1;
							writeSet.pImageInfo = &descriptor.m_ImageInfo;


						}
						else
						{
							writeSet.dstSet = desc;
							writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							writeSet.dstBinding = samplers.second.m_BindingPoint;
							writeSet.dstArrayElement = 0;
							writeSet.descriptorCount = 1;
							writeSet.pImageInfo = &descriptor.m_ImageInfo;
						}
						vkUpdateDescriptorSets(instance.getVkLogicalDevice(), 1, &writeSet, 0, nullptr);
					}
					descriptor.m_WriteSetFrames[samplers.second.m_BindingPoint] = writeSet;
					descriptor.m_LocalBufferNames[samplers.second.m_Name] = samplers.second.m_BindingPoint;
				}

			}

			descriptor.m_ImageInfo = DefaultTextures::GetDefaultStorage()->getInfo();
			for (auto& storage : reflectedMeta.m_ShaderDatas[full_path.filename().string()].m_ReflectedData.m_StorageImages)
			{
				VkWriteDescriptorSet writeSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
				for (auto& desc : descriptor.m_DescriptorSets)
				{
					writeSet.dstSet = desc;
					writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					writeSet.dstBinding = storage.second.m_BindingPoint;
					writeSet.dstArrayElement = 0;
					std::vector<VkDescriptorImageInfo> infos;
					if (storage.second.m_ArraySize > 0)
					{
						infos.resize(storage.second.m_ArraySize);
						for (uint32_t i = 0; i < storage.second.m_ArraySize; ++i)
							infos[i] = descriptor.m_ImageInfo;
						writeSet.descriptorCount = static_cast<uint32_t>(infos.size());
						writeSet.pImageInfo = infos.data();

					}
					else
					{
						writeSet.descriptorCount = 1;
						writeSet.pImageInfo = &descriptor.m_ImageInfo;
					}

					vkUpdateDescriptorSets(instance.getVkLogicalDevice(), 1, &writeSet, 0, nullptr);
				}
				descriptor.m_WriteSetFrames[storage.second.m_BindingPoint] = writeSet;
				descriptor.m_LocalBufferNames[storage.second.m_Name] = storage.second.m_BindingPoint;
			}
		}

	}
	void VulkanPipeline::UpdateDescriptor(VkDescriptorImageInfo& imageInfo, VkDescriptorType type, std::uint32_t bindingPoint)
	{
		std::int32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
		auto findItr = m_PipelineDescriptor[0].m_WriteSetFrames.find(bindingPoint);
		if (findItr != m_PipelineDescriptor[0].m_WriteSetFrames.end())
		{
			findItr->second.dstSet = m_PipelineDescriptor[0].m_DescriptorSets[frame];
			findItr->second.descriptorType = type;
			findItr->second.dstBinding = bindingPoint;
			findItr->second.dstArrayElement = 0;
			findItr->second.descriptorCount = 1;
			findItr->second.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(GraphicsManager::getInstance().getInstance().getVkInstance().getVkLogicalDevice()
				, 1, &findItr->second, 0, 0);
		}
	}
	void VulkanPipeline::UpdateDescriptor(VkDescriptorImageInfo& imageInfo, VkDescriptorType type, std::uint32_t bindingPoint, std::uint32_t frame, std::uint32_t DescIndex)
	{
		auto findItr = m_PipelineDescriptor[DescIndex].m_WriteSetFrames.find(bindingPoint);
		if (findItr != m_PipelineDescriptor[DescIndex].m_WriteSetFrames.end())
		{
			findItr->second.dstSet = m_PipelineDescriptor[DescIndex].m_DescriptorSets[frame];
			findItr->second.descriptorType = type;
			findItr->second.dstBinding = bindingPoint;
			findItr->second.dstArrayElement = 0;
			findItr->second.descriptorCount = 1;
			findItr->second.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(GraphicsManager::getInstance().getInstance().getVkInstance().getVkLogicalDevice()
				, 1, &findItr->second, 0, 0);
		}
	}
	VkDescriptorSetLayout VulkanPipeline::GetLayout() const
	{
		return m_DescSetLayout;
	}
	const std::vector<VkDescriptorSet>& VulkanPipeline::GetDescriptorSets(std::uint32_t descIndex) const
	{
		return m_PipelineDescriptor[descIndex].m_DescriptorSets;
	}
	void VulkanPipeline::SetRenderTarget(VkRenderPass renderTarget)
	{
		m_RenderTarget = renderTarget;
	}
	void VulkanPipeline::GenerateMatrixInputAttribute(std::vector<VkVertexInputAttributeDescription>& inputAttris, std::uint32_t& prevOffset, std::uint32_t& location, std::uint32_t& binding, VertexBufferElement& inputElem)
	{
		std::uint32_t cnt = inputElem.m_DataType == VAR_TYPE::MAT4 ? 4 : 3;
		std::uint32_t offset = prevOffset;
		for (std::uint32_t i = 0; i < cnt; ++i)
		{
			offset += cnt * sizeof(float);
			VkVertexInputAttributeDescription inputAttribute{};

			inputAttribute.binding = binding;
			inputAttribute.location = location;
			inputAttribute.format = ConvertToVkFormat(inputElem.m_DataType);
			inputAttribute.offset = offset;
			inputAttris.push_back(inputAttribute);

			++location;

		}
	}
	VkVertexInputAttributeDescription VulkanPipeline::GenerateVectorInputAttribute(std::uint32_t& location, std::uint32_t& binding, VertexBufferElement& inputElem)
	{
		VkVertexInputAttributeDescription inputAttribute{};
		inputAttribute.binding = binding;
		inputAttribute.location = location;
		inputAttribute.format = ConvertToVkFormat(inputElem.m_DataType);
		inputAttribute.offset = inputElem.m_offset;

		++location;
		return inputAttribute;
	}

	void VulkanPipeline::DestroyModules()
	{
		for (auto& module : m_ShaderLoadedData.m_ShaderModules)
		{
			VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
			vkDestroyShaderModule(instance.getVkLogicalDevice(), module.second, 0);
		}
		m_ShaderLoadedData.m_ShaderModules.clear();
	}
	VkShaderStageFlags GetShaderFlag(std::int32_t flags)
	{

		VkShaderStageFlags flag = 0;

		if ((flags & (int)SHADER_FLAG::VERTEX))
		{
			flag |= ShaderFlagsToVkStage(SHADER_FLAG::VERTEX);
		}

		if ((flags & (int)SHADER_FLAG::FRAGMENT))
		{
			flag |= ShaderFlagsToVkStage(SHADER_FLAG::FRAGMENT);
		}
		if ((flags & (int)SHADER_FLAG::COMPUTE_SHADER))
		{
			flag |= ShaderFlagsToVkStage(SHADER_FLAG::COMPUTE_SHADER);
		}
		return flag;
	}
}