#pragma once
#include "GraphicsResource/Revamped/MaterialAttributes.h"
#include "vulkanTools/VulkanPipeline.h"
#include "MacroHelper.h"
namespace TDS
{

	

	class MaterialManager
	{
		public:
			std::unordered_map<MaterialAttributes::SHADING_TYPE, std::shared_ptr<VulkanPipeline>> m_MaterialShaders;
			std::unordered_map<std::string, MaterialData>	m_ModelToMaterials;
			bool											m_UpdateMaterialList = false;
		public:
			MaterialManager() = default;
			~MaterialManager() = default;

			DLL_API void InsertMaterialPipeline(MaterialAttributes::SHADING_TYPE shadingType, std::shared_ptr<VulkanPipeline> pipeline);
			DLL_API void PreloadMaterialInfos();

			DLL_API VulkanPipeline& GetMaterialPipeline(MaterialAttributes::SHADING_TYPE shading);

			DLL_API MaterialData* GetMaterialData(std::string_view ModelName);

			DLL_API void LoadMaterial(std::string_view MaterialPath);
			
			void ShutDown();



	};



}