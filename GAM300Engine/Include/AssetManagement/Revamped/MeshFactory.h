#pragma once
#include "GraphicsResource/Revamped/ModelPack.h"
#include "AssetManagement/FactoryBase.h"
#include "GraphicsResource/Revamped/MeshController.h"
#define MODEL_PATH "../assets/models/"
#define MAX_PRELOAD_MODELS 1000
namespace TDS
{





	template <>
	class AssetFactory<MeshController>
	{
		static constexpr int MAX_MODELS = 8196;

	private:

		std::array<const char*, 6>										m_PrimitiveModels = { "capsule_bin.bin", "cube_bin.bin", "Quad1_bin.bin",
			"Quad2_bin.bin","sphere_bin.bin", "torus_bin.bin" };

		std::array<MeshController, MAX_MODELS>							m_MeshControllers;
		std::unordered_map<std::string, std::uint32_t>					m_ModelIndices;
		std::unordered_map<std::string, std::uint32_t>					m_ReferenceCnt;
		std::uint32_t													m_CurrentIndex = 0;
		std::uint32_t													m_TotalBatchMeshes = 0;

	public:

		std::array<MeshController, MAX_MODELS>& GetMeshControllers()
		{
			return m_MeshControllers;
		}
		void LoadNewModel(std::string_view path)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();

			auto& CurrController = m_MeshControllers[m_CurrentIndex];

			if (CurrController.m_ModelPack != nullptr)
				CurrController.Destroy();

			CurrController.m_ModelPack = new Modelpack();



			CurrController.m_ModelPack->LoadModelPack(path);


			if (CurrController.m_ModelPack->m_ModelHandle.m_SubMesh.size() == 1)
			{
				CurrController.LoadMeshData();
			}
			else
			{
				m_TotalBatchMeshes += CurrController.m_ModelPack->m_TotalMeshCnt;

				std::uint32_t currOffset = m_TotalBatchMeshes;

				CurrController.SetMeshOffset(currOffset);

				CurrController.LoadBatchData();
			}
			CurrController.BuildMeshTree();

			m_ModelIndices[fileName] = m_CurrentIndex;

			CurrController.m_ModelPack->m_ModelName = fileName;
			m_CurrentIndex++;
		}

		void LoadNewModelWithNewName(std::string_view path, std::string_view newName)
		{
			//std::filesystem::path FilePath(path);
			//std::string fileName = FilePath.filename().string();

			auto& CurrController = m_MeshControllers[m_CurrentIndex];

			if (CurrController.m_ModelPack != nullptr)
				CurrController.Destroy();

			CurrController.m_ModelPack = new Modelpack();



			CurrController.m_ModelPack->LoadModelPack(path);


			if (CurrController.m_ModelPack->m_ModelHandle.m_SubMesh.size() == 1)
			{
				CurrController.LoadMeshData();
			}
			else
			{
				m_TotalBatchMeshes += CurrController.m_ModelPack->m_TotalMeshCnt;

				std::uint32_t currOffset = m_TotalBatchMeshes;

				CurrController.SetMeshOffset(currOffset);

				CurrController.LoadBatchData();
			}
			CurrController.BuildMeshTree();

			m_ModelIndices[newName.data()] = m_CurrentIndex;

			CurrController.m_ModelPack->m_ModelName = newName.data();
			m_CurrentIndex++;
		}

		std::string LoadModel(std::string_view path)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();
			std::string outName = fileName;

			auto itr = m_ModelIndices.find(fileName);
			if (itr != m_ModelIndices.end())
			{

				int instance = m_ReferenceCnt[fileName];
				int index = itr->second;
				auto& CurrController = m_MeshControllers[m_ModelIndices[fileName]];


				if (instance == 0)
				{

					CurrController.Destroy();
					CurrController.m_ModelPack = new Modelpack();
					CurrController.m_ModelPack->LoadModelPack(path);


					if (CurrController.m_ModelPack->m_ModelHandle.m_SubMesh.size() == 1)
					{
						CurrController.LoadMeshData();
					}
					else
					{

						m_TotalBatchMeshes += CurrController.m_ModelPack->m_TotalMeshCnt;

						std::uint32_t currOffset = m_TotalBatchMeshes;
						CurrController.SetMeshOffset(currOffset);

						CurrController.LoadBatchData();

						CurrController.BuildMeshTree();
					}
					CurrController.m_ModelPack->m_ModelName = fileName;
				}
				else if (CurrController.m_Instancing == false)
				{

					UniqueUID uid;
					outName += std::to_string(uid.GetID());
					LoadNewModelWithNewName(path, outName);
					fileName = outName;
				}
				else
				{
					TDS_INFO("This model is being used by {} entities, load failure", instance);
				}


			}
			else
			{

				LoadNewModel(path);

			}
			return fileName;
		}

		void PreloadDefaultPrimitives()
		{
			for (auto& primitive : m_PrimitiveModels)
			{
				;
				std::string Path = MODEL_PATH;
				Path += primitive;
				std::string primitiveName = std::filesystem::path(Path).filename().string();
				LoadModel(Path);

			}
		}

		//MeshController* GetMeshController(std::string_view modelName, TypeReference<MeshController>& model)
		//{
		//	auto itr = m_ModelIndices.find(modelName.data());
		//	if (itr != m_ModelIndices.end())
		//	{
		//		if (model.m_ResourcePtr != nullptr && model.m_ResourcePtr->m_Instancing == false)
		//		{

		//			UniqueUID uid;
		//			std::string newName = itr->first + "_" + std::to_string(uid.GetID());
		//			std::string Path = MODEL_PATH;
		//			Path += itr->first;
		//			LoadNewModelWithNewName(Path, newName);
		//			model.m_AssetName = modelName;
		//			model.m_ResourcePtr = &m_MeshControllers[m_CurrentIndex];

		//		}
		//		else
		//		{
		//			UnloadReference(model);

		//			model.m_AssetName = modelName;
		//			model.m_ResourcePtr = &m_MeshControllers[itr->second];
		//			++m_ReferenceCnt[modelName.data()];
		//			return model.m_ResourcePtr;
		//		}
		//	}

		//	return nullptr;
		//	TDS_WARN("Model doesnt exist!");
		//}
		MeshController* GetMeshController(std::string_view modelName, TypeReference<MeshController>& model)
		{
			auto itr = m_ModelIndices.find(modelName.data());
			if (itr != m_ModelIndices.end())
			{
				UnloadReference(model);

				model.m_AssetName = modelName;
				model.m_ResourcePtr = &m_MeshControllers[itr->second];
				++m_ReferenceCnt[modelName.data()];
				return model.m_ResourcePtr;

			}
			else
			{
				auto GetBaseName = [](std::string_view baseName)
				{
					auto it = std::find_if(baseName.begin(), baseName.end(), [](unsigned char c) { return std::isdigit(c); });
					return std::string(baseName.begin(), it);
				};

				std::string BaseName = GetBaseName(modelName);

				auto itr = m_ModelIndices.find(BaseName);

				if (itr != m_ModelIndices.end())
				{
					UnloadReference(model);
					std::string Path = MODEL_PATH;
					Path += itr->first;
					LoadNewModelWithNewName(Path, modelName);
					model.m_AssetName = modelName;
					model.m_ResourcePtr = &m_MeshControllers[m_CurrentIndex];
				}



			}
			return nullptr;
			TDS_WARN("Model doesnt exist!");
		}
		void Load(std::string_view path, TypeReference<MeshController>& modelRef)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();
			if (modelRef.m_ResourcePtr)
				UnloadReference(modelRef);

			if (GetMeshController(fileName, modelRef) == nullptr)
				LoadModel(path);

			modelRef.m_ResourcePtr = GetMeshController(fileName, modelRef);
		}
		void Preload()
		{
			std::filesystem::path dir = MODEL_PATH;

			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
			{
				std::cout << "Invalid directory" << std::endl;
				return;
			}
			std::uint32_t numPreLoadedModels = 0;
			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				if (numPreLoadedModels >= MAX_PRELOAD_MODELS)
					break;


				const std::filesystem::path& path = entry.path();

				if (path.extension() == ".bin")
				{
					std::string pathInString = entry.path().string();
					LoadModel(pathInString);
					++numPreLoadedModels;
				}
			}
		}
		void UnloadReference(TypeReference<MeshController>& modelRef)
		{

			if (modelRef.m_ResourcePtr == nullptr)
				return;


			modelRef.m_ResourcePtr = nullptr;

			--m_ReferenceCnt[modelRef.m_AssetName];

			if (m_ReferenceCnt[modelRef.m_AssetName] == 0)
			{
				m_MeshControllers[m_ModelIndices[modelRef.m_AssetName]].Destroy();
			}
			modelRef.m_AssetName = "";

		}
		void DestroyAllMesh()
		{
			for (auto& model : m_MeshControllers)
			{
				model.Destroy();
			}
			m_ModelIndices.clear();
		}


	};
}