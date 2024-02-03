#pragma once
namespace TDS
{
	template <typename T>
	struct TypeReference
	{
		std::string			m_AssetName;
		T*					m_ResourcePtr = nullptr;

	};

}