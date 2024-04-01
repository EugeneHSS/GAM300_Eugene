#pragma once
#include "BufferInfo.h"
#include "dotnet/ImportExport.h"
namespace TDS
{
	//Global Buffer Pool IS only IF YOUR BUFFER IS SHARED THROUGHOUT ALL SHADERS, if not please dont use this

	class GlobalBufferPool
	{
	private:
		using BufferBindingMap = std::unordered_map<std::uint32_t, std::vector<std::shared_ptr<UBO>>>;;
		using BufferBindingName = std::unordered_map<std::string, std::uint32_t>;
		std::vector<BufferBindingMap>											m_SharedBufferBindings;
		std::vector<BufferBindingName>											m_SharedBindingNames;
	public:
		inline static std::shared_ptr<GlobalBufferPool> m_Instance = nullptr;
		static DLL_API std::shared_ptr<GlobalBufferPool>						GetInstance()
		{
			if (m_Instance == nullptr)
			{
				m_Instance = std::make_shared<GlobalBufferPool>();
				m_Instance->m_SharedBufferBindings.resize(5);
				m_Instance->m_SharedBindingNames.resize(5);
			}
			return m_Instance;
		}
		DLL_API std::shared_ptr<UBO>											GetBuffer(std::uint32_t binding, std::uint32_t descIndex = 0);
		DLL_API std::vector<std::shared_ptr<UBO>>* GetBufferContainer(std::uint32_t binding, std::uint32_t descIndex = 0);
		void DLL_API															AddToGlobalPool(size_t size, std::uint32_t binding, VkBufferUsageFlags usage,
			std::string_view BufferName, void* data = nullptr, std::uint32_t descIndex = 0);
		//void DLL_API															AddToGlobalPoolByName(std::string_view BufferName, void* data = nullptr);
		DLL_API bool															BindingExist(std::uint32_t binding, std::uint32_t descIndex = 0);
		DLL_API bool															BindingExist(std::string_view name, std::uint32_t descIndex = 0);
		DLL_API std::uint32_t													GetBinding(std::string_view name, std::uint32_t descIndex = 0);
		void DLL_API															Destroy();
	};
}