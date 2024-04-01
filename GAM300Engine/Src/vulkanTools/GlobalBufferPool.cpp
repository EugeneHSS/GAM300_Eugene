#include "vulkanTools/GlobalBufferPool.h"
#include "Rendering/GraphicsManager.h"
#include "Shader/ShaderLoader.h"
#include "vulkanTools/VulkanHelper.h"
#include "vulkanTools/Renderer.h"
namespace TDS
{
	std::shared_ptr<UBO> GlobalBufferPool::GetBuffer(std::uint32_t binding, std::uint32_t descIndex)
	{
		auto find = m_SharedBufferBindings[descIndex].find(binding);
		if (find == m_SharedBufferBindings[descIndex].end())
			return nullptr;
		return find->second[0];
	}
	std::vector<std::shared_ptr<UBO>>* GlobalBufferPool::GetBufferContainer(std::uint32_t binding, std::uint32_t descIndex)
	{
		auto find = m_SharedBufferBindings[descIndex].find(binding);
		if (find == m_SharedBufferBindings[descIndex].end())
			return nullptr;
		return &find->second;
	}
	void GlobalBufferPool::AddToGlobalPool(size_t size, std::uint32_t binding, VkBufferUsageFlags usage, std::string_view BufferName, void* data, std::uint32_t descIndex)
	{

		auto find = m_SharedBufferBindings[descIndex].find(binding);
		if (find == m_SharedBufferBindings[descIndex].end())
		{
			std::shared_ptr<UBO> newBuffer = std::make_shared<UBO>();
			newBuffer->m_Buffer = std::make_shared<VMABuffer>();

			if (data)
			{
				VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
				usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				newBuffer->m_Buffer->MappedStaging(size, usage, instance, data);
			}
			else
			{
				newBuffer->m_Buffer->CreateBuffer(size, usage, VMA_MEMORY_USAGE_CPU_TO_GPU);
			}
			newBuffer->m_BufferInfo.buffer = newBuffer->m_Buffer->GetBuffer();
			newBuffer->m_BufferInfo.offset = 0;
			newBuffer->m_BufferInfo.range = size;
			m_SharedBufferBindings[descIndex][binding].resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

			for (std::uint32_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
				m_SharedBufferBindings[descIndex][binding][i] = newBuffer;

			m_SharedBindingNames[descIndex][BufferName.data()] = binding;
		}
		else
		{
			TDS_ERROR("Global Buffer binding {} already exist!", binding);
		}
	}

	//void GlobalBufferPool::AddToGlobalPoolByName(std::string_view BufferName, void* data)
	//{
	//	Uniform& UniformReflecteddata = ShaderLoader::GetInstance()->getReflectedLookUp().m_ShaderDatas.begin()->second.m_ReflectedData.m_UniformBuffers[BufferName.data()];

	//	VkBufferUsageFlags usage = UniformReflecteddata.m_BufferType == BUFFER_TYPE::UNIFORM ? VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	//	auto find = m_SharedBufferBindings.find(UniformReflecteddata.m_BindingPoint);
	//	size_t size = UniformReflecteddata.m_Size;
	//	if (find == m_SharedBufferBindings.end())
	//	{
	//		std::shared_ptr<UBO> newBuffer = std::make_shared<UBO>();

	//		if (data)
	//		{
	//			VulkanInstance& instance = GraphicsManager::getInstance().getVkInstance();
	//			usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	//			newBuffer->m_Buffer->MappedStaging(size, usage, instance, data);
	//		}
	//		else
	//		{
	//			newBuffer->m_Buffer->CreateBuffer(size, usage, VMA_MEMORY_USAGE_CPU_TO_GPU);
	//		}
	//		newBuffer->m_BufferInfo.buffer = newBuffer->m_Buffer->GetBuffer();
	//		newBuffer->m_BufferInfo.offset = 0;
	//		newBuffer->m_BufferInfo.range = size;
	//		m_SharedBufferBindings[UniformReflecteddata.m_BindingPoint].resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

	//		for (std::uint32_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
	//			m_SharedBufferBindings[UniformReflecteddata.m_BindingPoint][i] = newBuffer;

	//		m_SharedBindingNames[BufferName.data()] = UniformReflecteddata.m_BindingPoint;
	//	}
	//	else
	//	{
	//		TDS_ERROR("Global Buffer binding %d already exist!", UniformReflecteddata.m_BindingPoint);
	//	}
	//}

	bool GlobalBufferPool::BindingExist(std::uint32_t binding, std::uint32_t descIndex)
	{
		m_SharedBufferBindings[descIndex];
		return m_SharedBufferBindings[descIndex].find(binding) != m_SharedBufferBindings[descIndex].end();
	}

	bool GlobalBufferPool::BindingExist(std::string_view name, std::uint32_t descIndex)
	{
		return m_SharedBindingNames[descIndex].find(name.data()) != m_SharedBindingNames[descIndex].end();
	}

	DLL_API std::uint32_t GlobalBufferPool::GetBinding(std::string_view name, std::uint32_t descIndex)
	{
		return m_SharedBindingNames[descIndex][name.data()];
	}

	void DLL_API GlobalBufferPool::Destroy()
	{
		m_SharedBufferBindings.clear();
		m_SharedBindingNames.clear();
	}


}