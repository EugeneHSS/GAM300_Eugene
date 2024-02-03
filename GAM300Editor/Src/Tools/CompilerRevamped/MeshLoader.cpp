#include "Tools/CompilerRevamped/MeshLoader.h"
#include "meshoptimizer-master/src/meshoptimizer.h"
#include "Logger/Logger.h"
#include "Identifier/UniqueID.h"
#include "Rendering/GraphicsManager.h"
#include "GraphicsResource/GeomSerializer.h"
namespace TDS
{

	struct RawMeshData
	{
		std::string						m_ParentNode = "";
		std::string						m_NodeName = "";
		std::string						m_MeshName = "";
		std::vector<TDSModel::Vertex>	m_Vertices;
		std::vector<MeshLoader::GeomData::lod>				m_Lod;
		Vec3							m_ScenePos;
		Vec3							m_SceneRotate;
		Vec3							m_SceneScale;
		std::vector<std::uint32_t>		m_Indices;
		int								iMatertialInstance;
	};


	MeshLoader::MeshLoader()
	{
	}
	MeshLoader::~MeshLoader()
	{
	}

	struct dummy
	{
		Vec3 test;
	};

	void ListNodes(const aiNode* pNode, int level = 0)
	{
		if (pNode == nullptr) return;

		std::cout << std::string(level, ' ') << pNode->mName.C_Str() << std::endl;

		for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
		{
			ListNodes(pNode->mChildren[i], level + 1);
		}
	}

	void ListAllNodeNames(const aiScene* scene)
	{
		if (scene != nullptr && scene->mRootNode != nullptr)
		{
			ListNodes(scene->mRootNode);
		}
	}

	void ListMeshNames(const aiScene* scene) {
		if (scene == nullptr) {
			std::cout << "No scene loaded." << std::endl;
			return;
		}

		std::cout << "Listing Mesh Names:" << std::endl;
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];
			std::cout << "Mesh " << i << ": " << mesh->mName.C_Str() << std::endl;
		}
	}
	static int totalmesh = 0;
	static std::set<std::string> uniqueNodesWithMeshes;
	static int totalNodesWithMeshes = 0;

	void PrintIndentation(int level) {
		for (int i = 0; i < level; ++i) {
			std::cout << " ";
		}
	}

	void ListMeshesInEveryNode(const aiNode* node, const aiScene* scene, int level = 0) {
		if (node == nullptr || scene == nullptr) {
			std::cout << "Node or scene is null." << std::endl;
			return;
		}

		bool hasMesh = false;

		// Print current node name with indentation
		PrintIndentation(level * 2); // 2 spaces per level for indentation
		std::cout << "Node: " << node->mName.C_Str();

		// List all meshes in this node
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			unsigned int meshIndex = node->mMeshes[i];
			if (meshIndex < scene->mNumMeshes) {
				const aiMesh* mesh = scene->mMeshes[meshIndex];
				std::cout << "\n";
				PrintIndentation(level * 2 + 2); // Additional indentation for mesh
				std::cout << "Mesh " << meshIndex << ": " << mesh->mName.C_Str();
				hasMesh = true;
			}
			else {
				std::cout << "\n";
				PrintIndentation(level * 2 + 2);
				std::cout << "Mesh index out of range.";
			}
		}

		if (hasMesh) {
			std::cout << "\n";
			if (uniqueNodesWithMeshes.insert(node->mName.C_Str()).second) {
				++totalNodesWithMeshes;
			}
		}
		else {
			std::cout << " (No Meshes)\n";
		}

		// Recursively call this function for all child nodes
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			ListMeshesInEveryNode(node->mChildren[i], scene, level + 1);
		}
	}


	void MeshLoader::RunCompiler(Request& request)
	{
		AssimpSceneInfo* currSceneInfo = nullptr;
		for (auto& AssimpScene : m_AssimpLoaders)
		{
			if (AssimpScene.m_NotAvail == false)
			{
				currSceneInfo = &AssimpScene;
				break;
			}
		}

		Assimp::Importer importer{};
		importer.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true);

		std::string filePath = "../assets/models/" + request.m_FileName;
		std::uint32_t flags = 0;

		flags =
			aiProcess_Triangulate
			| aiProcess_LimitBoneWeights
			| aiProcess_GenUVCoords
			| aiProcess_TransformUVCoords
			| aiProcess_FindInstances
			| aiProcess_CalcTangentSpace
			| aiProcess_GenBoundingBoxes
			| aiProcess_GenSmoothNormals
			| aiProcess_RemoveRedundantMaterials
			| aiProcess_FindInvalidData
			| aiProcess_FlipUVs;

		currSceneInfo->m_Scene = importer.ReadFile(filePath, flags);

		if (currSceneInfo->m_Scene == nullptr)
		{
			TDS_LOGGER("Failed to read model : {}", filePath);
		}

		if (currSceneInfo->MeshValidityCheck(request) == false)
			return;


		std::vector<RawMeshData> assimpData;
		ImportMeshData(request, *currSceneInfo, assimpData);
		MergeMesh(request, assimpData);
		CreateLODs(request, assimpData);
		//OptimizeMesh(assimpData);
		GeomData data{};
		CreateFinalGeom(assimpData, data);
		data.ConvertToTDSModel(request.m_Output);
		//std::ofstream fileStream(request.m_OutFile.data(), std::ios::binary);
		//if (!fileStream)
		//{
		//	return;
		//}
		//SerializeGeom(request.m_Output, fileStream);


		request.m_Output.Serialize(request.m_OutFile);
		//






	}

	void MeshLoader::AddRequests(const std::string& fileName, const std::string& outFIle, GeomDescriptor& desc)
	{
		m_RequestLoad.emplace_back(Request(fileName, outFIle, desc));
	}
	bool MeshLoader::AssimpLoad(Request& request, AssimpSceneInfo& assimping)
	{
		Assimp::Importer importer{};
		importer.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true);

		std::string filePath = "../assets/models/" + request.m_FileName;
		std::uint32_t flags = 0;

		flags =
			aiProcess_Triangulate
			| aiProcess_LimitBoneWeights
			| aiProcess_GenUVCoords
			| aiProcess_TransformUVCoords
			| aiProcess_FindInstances
			| aiProcess_CalcTangentSpace
			| aiProcess_GenSmoothNormals
			| aiProcess_GenBoundingBoxes
			| aiProcess_JoinIdenticalVertices
			| aiProcess_RemoveRedundantMaterials
			| aiProcess_FindInvalidData
			| aiProcess_FlipUVs;

		assimping.m_Scene = importer.ReadFile(filePath, flags);

		if (assimping.m_Scene == nullptr)
		{
			TDS_LOGGER("Failed to read model : {}", filePath);
			return false;
		}

		return assimping.MeshValidityCheck(request);
	}

	void MeshLoader::ImportMeshData(Request& request, AssimpSceneInfo& assimp, std::vector<RawMeshData>& assimpData)
	{
		aiVector3D scaling((ai_real)request.currSetting.m_Descriptor.m_L2W.m_Scale[0], (ai_real)request.currSetting.m_Descriptor.m_L2W.m_Scale[1], (ai_real)request.currSetting.m_Descriptor.m_L2W.m_Scale[2]);

		assimp.m_AppliedTransformation.Scaling(scaling, assimp.m_AppliedTransformation);
		assimp.m_AppliedTransformation.RotationX((ai_real)request.currSetting.m_Descriptor.m_L2W.m_Rotate[0], assimp.m_AppliedTransformation);
		assimp.m_AppliedTransformation.RotationY((ai_real)request.currSetting.m_Descriptor.m_L2W.m_Rotate[1], assimp.m_AppliedTransformation);
		assimp.m_AppliedTransformation.RotationZ((ai_real)request.currSetting.m_Descriptor.m_L2W.m_Rotate[2], assimp.m_AppliedTransformation);


		aiVector3D translation((ai_real)request.currSetting.m_Descriptor.m_L2W.m_Translate[0], (ai_real)request.currSetting.m_Descriptor.m_L2W.m_Translate[1], (ai_real)request.currSetting.m_Descriptor.m_L2W.m_Translate[2]);
		assimp.m_AppliedTransformation.Translation(translation, assimp.m_AppliedTransformation);

		std::string rootName = assimp.m_Scene->mRootNode->mName.C_Str();

		//ListNodes(assimp.m_Scene->mRootNode);
		//ListMeshNames(assimp.m_Scene);
		//ListMeshesInEveryNode(assimp.m_Scene->mRootNode, assimp.m_Scene);
		//std::cout << totalmesh << std::endl;
		//std::cout << totalNodesWithMeshes << std::endl;
		ProcessScene(assimpData, *assimp.m_Scene->mRootNode, *assimp.m_Scene, assimp.m_AppliedTransformation, rootName, request);
	}

	aiMatrix4x4 GetScaleMatrix(const aiMatrix4x4& m)
	{
		aiVector3D scale;
		scale.x = std::sqrt(m.a1 * m.a1 + m.b1 * m.b1 + m.c1 * m.c1);
		scale.y = std::sqrt(m.a2 * m.a2 + m.b2 * m.b2 + m.c2 * m.c2);
		scale.z = std::sqrt(m.a3 * m.a3 + m.b3 * m.b3 + m.c3 * m.c3);

		aiMatrix4x4 scaleMatrix;
		scaleMatrix.a1 = scale.x; scaleMatrix.b1 = 0; scaleMatrix.c1 = 0; scaleMatrix.d1 = 0;
		scaleMatrix.a2 = 0; scaleMatrix.b2 = scale.y; scaleMatrix.c2 = 0; scaleMatrix.d2 = 0;
		scaleMatrix.a3 = 0; scaleMatrix.b3 = 0; scaleMatrix.c3 = scale.z; scaleMatrix.d3 = 0;
		scaleMatrix.a4 = 0; scaleMatrix.b4 = 0; scaleMatrix.c4 = 0; scaleMatrix.d4 = 1;

		return scaleMatrix;
	}


	aiVector3D GetScale(const aiMatrix4x4& m)
	{
		return aiVector3D(GetScaleMatrix(m).a1, GetScaleMatrix(m).b2, GetScaleMatrix(m).c3);
	}

	aiMatrix4x4 GetRotationMatrix(const aiMatrix4x4& m)
	{
		aiVector3D scale = GetScale(m); // Assuming GetScale returns a vector of scales

		// Remove scaling from the matrix
		aiMatrix4x4 rotationMatrix = m;
		rotationMatrix.a1 /= scale.x;
		rotationMatrix.b1 /= scale.x;
		rotationMatrix.c1 /= scale.x;

		rotationMatrix.a2 /= scale.y;
		rotationMatrix.b2 /= scale.y;
		rotationMatrix.c2 /= scale.y;

		rotationMatrix.a3 /= scale.z;
		rotationMatrix.b3 /= scale.z;
		rotationMatrix.c3 /= scale.z;

		// Set the translation part to 0
		rotationMatrix.a4 = rotationMatrix.b4 = rotationMatrix.c4 = 0;
		rotationMatrix.d4 = 1;

		return rotationMatrix;
	}

	aiMatrix4x4 GetTranslationMatrix(const aiMatrix4x4& m)
	{
		aiMatrix4x4 translationMatrix;
		translationMatrix.a1 = 1; translationMatrix.b1 = 0; translationMatrix.c1 = 0; translationMatrix.d1 = m.a4;
		translationMatrix.a2 = 0; translationMatrix.b2 = 1; translationMatrix.c2 = 0; translationMatrix.d2 = m.b4;
		translationMatrix.a3 = 0; translationMatrix.b3 = 0; translationMatrix.c3 = 1; translationMatrix.d3 = m.c4;
		translationMatrix.a4 = 0; translationMatrix.b4 = 0; translationMatrix.c4 = 0; translationMatrix.d4 = 1;

		return translationMatrix;
	}


	aiVector3D ExtractEulerAngles(const aiMatrix4x4& mat)
	{
		aiVector3D euler{};


		float sy = sqrt(mat.a1 * mat.a1 + mat.b1 * mat.b1);

		bool singular = sy < 1e-6;

		if (!singular)
		{
			euler.x = atan2(mat.c2, mat.c3);
			euler.y = atan2(-mat.c1, sy);
			euler.z = atan2(mat.b1, mat.a1);
		}
		else
		{

			euler.x = atan2(-mat.b3, mat.b2);
			euler.y = atan2(-mat.c1, sy);
			euler.z = 0;
		}

		return euler;
	}
	void ExtractTranslation(const aiMatrix4x4& mat, aiVector3D& translation)
	{
		translation.x = mat.a4;
		translation.y = mat.b4;
		translation.z = mat.c4;
	}

	void ExtractScale(const aiMatrix4x4& mat, aiVector3D& scale)
	{
		scale.x = sqrt(mat.a1 * mat.a1 + mat.b1 * mat.b1 + mat.c1 * mat.c1);
		scale.y = sqrt(mat.a2 * mat.a2 + mat.b2 * mat.b2 + mat.c2 * mat.c2);
		scale.z = sqrt(mat.a3 * mat.a3 + mat.b3 * mat.b3 + mat.c3 * mat.c3);
	}

	void MeshLoader::ProcessScene(std::vector<RawMeshData>& assimpData, const aiNode& Node, const aiScene& Scene, aiMatrix4x4& ParentTransform, std::string_view parentNode, Request& request)
	{
		aiMatrix4x4 AccumulatedTransform = ParentTransform * Node.mTransformation;
		size_t iNode = assimpData.size();

		assimpData.resize(iNode + Scene.mNumMeshes);

		for (std::uint32_t i = 0; i < Node.mNumMeshes; ++i)
		{
			aiMesh& mesh = *Scene.mMeshes[Node.mMeshes[i]];

			std::int32_t iUV = -1, iColor = -1;

			if (mesh.HasPositions() == false || mesh.HasFaces() == false)
			{
				TDS_WARN("This mesh : {}, doesnt have a position! Skipping...", mesh.mName.C_Str());
				continue;
			}

			if (mesh.HasNormals() == false)
			{
				TDS_WARN("This mesh : {}, doesnt have a normal! Skipping...", mesh.mName.C_Str());
				continue;
			}

			if (mesh.HasTangentsAndBitangents() == false)
			{
				TDS_WARN("This mesh : {}, doesnt have tangents! Tangents will be generated instead!", mesh.mName.C_Str());
			}

			if (mesh.GetNumUVChannels() == 0)
			{
				TDS_WARN("This mesh : {}, doesnt have UVs! Assigning (0,0) to UV coordinate", mesh.mName.C_Str());
			}
			else
			{
				TDS_WARN("This mesh {} may have many UV channels! One will be used only", mesh.mName.C_Str());
			}

			auto GetTextureIndex = [&]()->int
			{
				for (std::uint32_t i = 0; i < mesh.GetNumUVChannels(); ++i)
				{
					if (mesh.HasTextureCoords(i))
						return i;
				}
				return -1;
			};
			auto GetColorIndex = [&]()->int
			{
				for (std::uint32_t i = 0; i < mesh.GetNumUVChannels(); ++i)
				{
					if (mesh.HasVertexColors(i))
						return i;
				}
				return -1;
			};
			iUV = GetTextureIndex();
			iColor = GetColorIndex();


			auto& rawMesh = assimpData[iNode++];
			aiQuaternion currRotation;

			aiVector3D pos;
			AccumulatedTransform.DecomposeNoScaling(currRotation, pos);

			aiMatrix4x4 translateMat = GetTranslationMatrix(Node.mTransformation);
			aiMatrix4x4 rotationMat = GetRotationMatrix(Node.mTransformation);
			aiMatrix4x4 scaleMat = GetScaleMatrix(Node.mTransformation);

			aiMatrix4x4 identity;

			aiMatrix4x4 transformWithoutTrans = identity * rotationMat * scaleMat;

			aiVector3D translate{};

			ExtractTranslation(AccumulatedTransform, translate);
			aiVector3D rot = ExtractEulerAngles(AccumulatedTransform);
			aiVector3D scaling;
			ExtractScale(AccumulatedTransform, scaling);
			rawMesh.m_ParentNode = parentNode;
			rawMesh.m_NodeName = std::string(Node.mName.C_Str());
			rawMesh.m_MeshName = std::string(mesh.mName.C_Str());


			aiVector3D aMin = mesh.mAABB.mMin;
			aiVector3D aMax = mesh.mAABB.mMax;

			aMin = translateMat * aMin;
			aMax = translateMat * aMax;


			Vec3 min = Vec3(aMin.x, aMin.y, aMin.z);
			Vec3 max = Vec3(aMax.x, aMax.y, aMax.z);


			rawMesh.m_Vertices.resize(mesh.mNumVertices);
			rawMesh.m_ScenePos = Vec3(translate.x, translate.y, translate.z);
			rawMesh.m_SceneRotate = Vec3(rot.x, rot.y, rot.z);
			rawMesh.m_SceneScale = Vec3(scaling.x, scaling.y, scaling.z);
			for (std::uint32_t i = 0; i < mesh.mNumVertices; ++i)
			{
				auto& vert = rawMesh.m_Vertices[i];

				auto L = AccumulatedTransform * mesh.mVertices[i];

				vert.m_Position = Vec3(static_cast<float>(L.x), static_cast<float>(L.y), static_cast<float>(L.z));

				if (iUV == -1)
					vert.m_UV = Vec2(0.f, 0.f);
				else
					vert.m_UV = Vec2(static_cast<float>(mesh.mTextureCoords[iUV][i].x), static_cast<float>(mesh.mTextureCoords[iUV][i].y));

				if (iColor == -1)
					vert.m_Color = { 255.f, 255.f, 255.f, 255.f };
				else
				{
					vert.m_Color =
					{
						static_cast<float>(mesh.mColors[iColor][i].r)
										, static_cast<float>(mesh.mColors[iColor][i].g)
										, static_cast<float>(mesh.mColors[iColor][i].b)
										, static_cast<float>(mesh.mColors[iColor][i].a)

					};
				}

				if (mesh.HasTangentsAndBitangents())
				{
					const auto T = currRotation.Rotate(mesh.mTangents[i]);
					const auto B = currRotation.Rotate(mesh.mBitangents[i]);
					const auto N = currRotation.Rotate(mesh.mNormals[i]);

					//const auto T = mesh.mTangents[i];
					//const auto B = mesh.mBitangents[i];
					//const auto N = mesh.mNormals[i];

					vert.m_Normal = { N.x, N.y, N.z };
					vert.m_Tangent = { T.x, T.y, T.z };
					vert.m_Bitangent = { B.x, B.y, B.z };

					vert.m_Normal.Normalize();
					vert.m_Tangent.Normalize();
					vert.m_Bitangent.Normalize();
				}
				else
				{
					const auto N = currRotation.Rotate(mesh.mNormals[i]);
	/*				const auto N = mesh.mNormals[i];*/
					vert.m_Normal = { N.x, N.y, N.z };
					vert.m_Tangent = { 1.f, 0.f, 0.f };
					vert.m_Bitangent = { 1.f, 0.f, 0.f };

					vert.m_Normal.Normalize();
				}
			}
			for (std::uint32_t i = 0; i < mesh.mNumFaces; ++i)
			{
				auto& face = mesh.mFaces[i];
				for (std::uint32_t j = 0; j < face.mNumIndices; ++j)
					rawMesh.m_Indices.emplace_back(face.mIndices[j]);

			}

			int iMaterial = -1;

			if (request.currSetting.m_LoadMaterials)
			{
				if (mesh.mMaterialIndex >= 0)
				{
					const aiMaterial& Material = *Scene.mMaterials[mesh.mMaterialIndex];
					std::string matName = std::string(Material.GetName().C_Str());
					bool materialExist = false;

					for (auto& Mat : request.m_MaterialList.m_MaterialInfos)
					{
						if (Mat.m_MaterialGUID == reinterpret_cast<std::size_t>(&Material))
						{
							materialExist = true;
							break;
						}
					}

					if (materialExist == false)
					{
						auto& matInfo = request.m_MaterialList.m_MaterialInfos.emplace_back();
						matInfo.m_MaterialGUID = reinterpret_cast<std::size_t>(&Material);

						for (std::uint32_t i = 0; i < Material.mNumProperties; ++i)
						{
							auto& materialProp = *Material.mProperties[i];

							if (materialProp.mType == aiPTI_String)
							{

								aiString* pString = (aiString*)materialProp.mData;
								std::string matName = std::string(pString->C_Str());
								if (materialProp.mSemantic == aiTextureType_NONE)
								{
									matInfo.m_MaterialName = matName;
								}
								else
								{
									auto& materialEnumMap = MaterialMapList::Get();
									auto findItr = materialEnumMap.find(materialProp.mSemantic);

									if (findItr != materialEnumMap.end())
									{
										matInfo.m_MaterialName = findItr->second;
									}
									else
										matInfo.m_MaterialName = "";
								}
							}
						}
					}
				}

				iMaterial = static_cast<int>(request.m_MaterialList.m_MaterialInfos.size() - 1);
			}
			rawMesh.iMatertialInstance = mesh.mMaterialIndex;

		}
		if (iNode != assimpData.size())
		{
			assimpData.erase(assimpData.begin() + iNode, assimpData.end());
		}
		for (std::uint32_t i = 0; i < Node.mNumChildren; ++i)
		{
			ProcessScene(assimpData, *Node.mChildren[i], Scene, AccumulatedTransform, std::string(Node.mName.C_Str()), request);
		}

	}



	bool MeshLoader::AssimpSceneInfo::MeshValidityCheck(Request& request)
	{
		m_MeshRef.resize(m_Scene->mNumMeshes);
		std::function<void(const aiNode& Node)> ProcessNode = [&](const aiNode& Node) noexcept
		{
			for (std::uint32_t i = 0; i < Node.mNumMeshes; ++i)
			{
				aiMesh* pMesh = m_Scene->mMeshes[Node.mMeshes[i]];

				m_MeshRef[Node.mMeshes[i]].m_Nodes.emplace_back(&Node);
			}

			for (std::uint32_t i = 0; i < Node.mNumChildren; ++i)
			{
				ProcessNode(*Node.mChildren[i]);
			}
		};

		ProcessNode(*m_Scene->mRootNode);

		for (std::uint32_t i = 0; i < m_Scene->mNumMeshes; ++i)
		{
			const aiMesh& mesh = *m_Scene->mMeshes[i];
			const auto& ref = m_MeshRef[i].m_Nodes;

			if (request.currSetting.m_LoadMesh && ref.empty())
			{
				TDS_ERROR("NO REFERENCE TO THIS CURRENT MESH : {}, STOPPING LOADER...", mesh.mName.C_Str());
				return false;
			}

			if (request.currSetting.m_LoadAnimation && mesh.HasBones())
			{
				if (ref.size() > 1)
				{
					TDS_ERROR("Skin mesh {} is reference {} times in this scene... This is currently not allowed...", mesh.mName.C_Str(), (int)ref.size());
					return false;
				}
			}
			else if (request.currSetting.m_LoadAnimation == false || mesh.HasBones() == false)
			{
				if (ref.size() > 1)
					TDS_INFO("Mesh {} is reference {} times in this scene... I will keep the duplicates.", mesh.mName.C_Str(), (int)ref.size());
			}
		}
		return true;
	}

	void MeshLoader::MergeMesh(Request& request, std::vector<RawMeshData>& assimpData)
	{
		if (request.currSetting.m_Descriptor.MergeMesh)
		{
			for (size_t i = 0; i < assimpData.size(); ++i)
			{
				if (assimpData[i].m_Vertices.size() == 0 || assimpData[i].m_Indices.size() == 0)
				{
					assimpData.erase(assimpData.begin() + i);
					--i;
				}
			}
			for (size_t i = 0; i < assimpData.size(); ++i)
			{
				for (size_t j = i + 1; j < assimpData.size(); ++j)
				{

					if (assimpData[i].m_MeshName == assimpData[j].m_MeshName &&
						assimpData[i].iMatertialInstance == assimpData[j].iMatertialInstance)
					{
						std::int32_t iVertex = std::int32_t(assimpData[i].m_Vertices.size());
						std::int32_t iIndex = std::int32_t(assimpData[i].m_Indices.size());

						assimpData[i].m_Vertices.insert(assimpData[i].m_Vertices.end(), assimpData[j].m_Vertices.begin(), assimpData[j].m_Vertices.end());
						assimpData[i].m_Indices.insert(assimpData[i].m_Indices.end(), assimpData[j].m_Indices.begin(), assimpData[j].m_Indices.end());

						for (size_t k = iIndex; k < assimpData[i].m_Indices.size(); ++k)
						{
							assimpData[i].m_Indices[k] += iVertex;
						}

						assimpData.erase(assimpData.begin() + j);
						--j;

					}
				}
			}
		}



	}
	void MeshLoader::CreateLODs(Request& request, std::vector<RawMeshData>& assimpData)
	{
		if (request.currSetting.m_LodOptions.m_CreateLOD)
		{
			for (auto& Mesh : assimpData)
			{
				for (size_t i = 1; i < request.currSetting.m_LodOptions.m_Max_num_lods; ++i)
				{
					const float threshold = std::powf(request.currSetting.m_LodOptions.ReductionFactor, float(i));
					const size_t targetIndexCount = size_t(Mesh.m_Indices.size() * threshold) / 3 * 3;
					const float target_error = 1e-2f;
					const auto& S = (Mesh.m_Lod.size()) ? Mesh.m_Lod.back().m_Indices : Mesh.m_Indices;

					if (S.size() < targetIndexCount)
						break;

					auto& newLOD = Mesh.m_Lod.emplace_back();
					newLOD.m_Indices.resize(S.size());
					newLOD.m_Indices.resize(meshopt_simplify(newLOD.m_Indices.data(), S.data(), S.size(), &Mesh.m_Vertices[0].m_Position.x, Mesh.m_Vertices.size(), sizeof(TDSModel::Vertex), targetIndexCount, target_error));


				}
			}
		}
	}

	void MeshLoader::CreateFinalGeom(const std::vector<RawMeshData>& rawMesh, GeomData& geom)
	{
		for (auto& mesh : rawMesh)
		{
			int iMyMesh = -1;
			/*	for (size_t i = 0; i < geom.m_Meshes.size(); ++i)
				{
					if (geom.m_Meshes[i].m_Name == mesh.m_MeshName)
					{
						iMyMesh = i;
						break;
					}
				}*/

			if (iMyMesh == -1)
			{
				iMyMesh = int(geom.m_Meshes.size());
				auto& myLastMesh = geom.m_Meshes.emplace_back();
				myLastMesh.m_Name = mesh.m_MeshName;

				myLastMesh.m_ParentNode = mesh.m_ParentNode;
				myLastMesh.m_NodeName = mesh.m_NodeName;
			}

			auto& myMesh = geom.m_Meshes[iMyMesh];
			auto& subMesh = myMesh.m_Submeshes.emplace_back();
			subMesh.m_ScenePos = mesh.m_ScenePos;
			subMesh.m_SceneRotate = mesh.m_SceneRotate;
			subMesh.m_SceneScale = mesh.m_SceneScale;
			subMesh.m_Vertex = mesh.m_Vertices;
			subMesh.m_Indices = mesh.m_Indices;
			subMesh.m_iMaterial = mesh.iMatertialInstance;
			subMesh.m_Lods = mesh.m_Lod;
			subMesh.m_SubMeshName = mesh.m_MeshName;

		}
	}

	void MeshLoader::LoadAnimationData(Request& request, AssimpSceneInfo& assimp)
	{
		std::unordered_map<std::string, const aiNode*> m_NodeNameMap;
		std::unordered_map<std::string, const aiBone*> m_BoneNameMap;

		for (std::uint32_t i = 0; i < assimp.m_Scene->mNumMeshes; ++i)
		{
			const auto& mesh = assimp.m_Scene->mMeshes[i];
			for (std::uint32_t boneIndx = 0; boneIndx < mesh->mNumBones; ++boneIndx)
			{
				const auto& aibone = *mesh->mBones[boneIndx];
				auto boneItr = m_BoneNameMap.find(aibone.mName.data);
				if (boneItr == m_BoneNameMap.end())
				{
					auto pNode = assimp.m_Scene->mRootNode->FindNode(aibone.mName);
					m_BoneNameMap[aibone.mName.data] = &aibone;
					m_NodeNameMap[aibone.mName.data] = pNode;

				}

			}
		}

		for (std::uint32_t i = 0; i < assimp.m_Scene->mNumAnimations; ++i)
		{
			const aiAnimation& currentAnim = *assimp.m_Scene->mAnimations[i];
			for (std::uint32_t stream = 0; stream < currentAnim.mNumChannels; ++stream)
			{
				auto& currChannel = *currentAnim.mChannels[stream];
				auto findItr = m_NodeNameMap.find(currChannel.mNodeName.data);

				if (findItr == m_NodeNameMap.end())
				{
					m_NodeNameMap.insert({ currChannel.mNodeName.data,
						assimp.m_Scene->mRootNode->FindNode(currChannel.mNodeName) });

				}
			}
		}

		for (auto& itr : m_NodeNameMap)
		{
			for (auto pParentNode = m_NodeNameMap.find(itr.first)->second->mParent; pParentNode != nullptr; pParentNode = pParentNode->mParent)
			{
				if (auto e = m_NodeNameMap.find(pParentNode->mName.C_Str()); e == m_NodeNameMap.end())
				{
					m_NodeNameMap[pParentNode->mName.C_Str()] = pParentNode;
				}
			}
		}

		if (m_NodeNameMap.size() > 255)
		{
			TDS_ERROR("This mesh have {} bones! Only upto 256 is supported...");
		}

		struct SkeletonBuildTemp
		{
			const aiNode* Node;
			std::int32_t	layer;
			std::int32_t	children;
			std::int32_t	totalChildren;
		};

		std::vector<SkeletonBuildTemp> tempBuild(m_NodeNameMap.size());

		int i = 0;
		for (auto& itr : m_NodeNameMap)
		{
			auto& p = tempBuild[i++];
			p.Node = itr.second;
		}

		for (size_t i = 0; i < tempBuild.size(); ++i)
		{
			auto& p = tempBuild[i];
			bool foundParent = false;

			for (aiNode* pNode = p.Node->mParent; pNode; pNode = pNode->mParent)
			{
				p.layer++;

				for (size_t j = 0; j < tempBuild.size(); ++j)
				{
					auto& parent = tempBuild[j];
					if (pNode == parent.Node)
					{
						parent.totalChildren++;
						if (foundParent == false)
							parent.children++;
						foundParent = true;
						break;
					}
				}
			}
		}

		std::qsort(tempBuild.data(), tempBuild.size(), sizeof(SkeletonBuildTemp), [](const void* a, const void* b) -> int
			{
				const auto& A = *reinterpret_cast<const SkeletonBuildTemp*>(a);
				const auto& B = *reinterpret_cast<const SkeletonBuildTemp*>(b);

				if (A.layer < B.layer) return -1;

				if (A.layer > B.layer) return 1;

				if (A.totalChildren < B.totalChildren) return -1;

				return (A.totalChildren > B.totalChildren);
			});

		request.m_AnimationMesh.m_SkeletonData.m_BoneList.resize(tempBuild.size());

		i = 0;
		for (auto& bone : request.m_AnimationMesh.m_SkeletonData.m_BoneList)
		{
			auto& tempBone = tempBuild[i++];
			bone.m_Name = std::string(tempBone.Node->mName.C_Str());
			bone.m_ParentID = -1;

			for (aiNode* pNode = tempBone.Node->mParent; bone.m_ParentID == -1 && pNode != nullptr; pNode = pNode->mParent)
			{
				for (int j = 0; j < i; ++j)
				{
					if (tempBuild[j].Node == tempBone.Node->mParent)
					{
						bone.m_ParentID = j;
						bone.m_ParentName = std::string(tempBuild[j].Node->mName.C_Str());
						break;
					}
				}
			}

			auto itr = m_BoneNameMap.find(tempBone.Node->mName.data);

			if (itr != m_BoneNameMap.end())
			{
				aiMatrix4x4 OffsetMatrix = itr->second->mOffsetMatrix;
				bone.m_BoneTransform = OffsetMatrix;
				bone.m_BoneTransform = bone.m_BoneTransform.Transpose();
			}

		}




	}

	void MeshLoader::LoadAnimations(Request& request, AssimpSceneInfo& assimp, int SamplingFPS)
	{

	}


	void MeshLoader::OptimizeMesh(std::vector<RawMeshData>& assimpData)
	{
		std::vector<RawMeshData> out;

		for (auto& mesh : assimpData)
		{
			size_t indxCnt = mesh.m_Indices.size();
			std::vector<std::uint32_t> remappedIndices(indxCnt);

			size_t VtxCnt = meshopt_generateVertexRemap(&remappedIndices[0], mesh.m_Indices.data(), indxCnt, mesh.m_Vertices.data(), mesh.m_Vertices.size(), sizeof(TDSModel::Vertex));

			RawMeshData compressedMesh{};
			compressedMesh.m_Indices.resize(mesh.m_Indices.size());
			compressedMesh.m_Vertices.resize(VtxCnt);
			compressedMesh.iMatertialInstance = mesh.iMatertialInstance;
			compressedMesh.m_MeshName = mesh.m_MeshName;
			compressedMesh.m_ScenePos = mesh.m_ScenePos;
			compressedMesh.m_SceneRotate = mesh.m_SceneRotate;
			compressedMesh.m_SceneScale = mesh.m_SceneScale;
			compressedMesh.m_NodeName = mesh.m_NodeName;
			compressedMesh.m_ParentNode = mesh.m_ParentNode;

			meshopt_remapIndexBuffer(compressedMesh.m_Indices.data(), mesh.m_Indices.data(), mesh.m_Indices.size(), &remappedIndices[0]);
			meshopt_remapVertexBuffer(compressedMesh.m_Vertices.data(), mesh.m_Vertices.data(), mesh.m_Indices.size(), sizeof(TDSModel::Vertex), &remappedIndices[0]);
			meshopt_optimizeVertexCache(compressedMesh.m_Indices.data(), compressedMesh.m_Indices.data(), indxCnt, VtxCnt);
			meshopt_optimizeOverdraw(compressedMesh.m_Indices.data(), compressedMesh.m_Indices.data(), indxCnt, &compressedMesh.m_Vertices[0].m_Position.x, VtxCnt, sizeof(TDSModel::Vertex), 1.05f);
			compressedMesh.m_Vertices.resize(meshopt_optimizeVertexFetch(compressedMesh.m_Vertices.data(), compressedMesh.m_Indices.data(), indxCnt, compressedMesh.m_Vertices.data(), VtxCnt, sizeof(TDSModel::Vertex)));
			for (auto& Lod : mesh.m_Lod)
			{
				meshopt_optimizeVertexCache(Lod.m_Indices.data(), Lod.m_Indices.data(), Lod.m_Indices.size(), mesh.m_Vertices.size());
				meshopt_optimizeOverdraw(Lod.m_Indices.data(), Lod.m_Indices.data(), Lod.m_Indices.size(), &mesh.m_Vertices[0].m_Position.x, mesh.m_Vertices.size(), sizeof(TDSModel::Vertex), 1.05f);
			}
			out.push_back(compressedMesh);
		}
		assimpData = out;
	}


	void MeshLoader::GeomData::ConvertToTDSModel(TDSModel& model)
	{

		std::uint32_t iLod = 0;
		for (auto& inputMesh : m_Meshes)
		{
			TDSModel::Mesh mesh{};
			mesh.m_Name = inputMesh.m_Name;
			mesh.m_NodeName = inputMesh.m_NodeName;
			mesh.m_ParentNode = inputMesh.m_ParentNode;
			mesh.m_iLOD = iLod;

			for (const auto& inputSubMesh : inputMesh.m_Submeshes)
			{
				TDSModel::SubMesh submesh{};
				submesh.m_nFaces = std::uint32_t(inputSubMesh.m_Indices.size() / 3);
				submesh.m_iIndices = std::uint32_t(model.m_Indices.size());
				submesh.m_iVertices = std::uint32_t(model.m_ModelVertex.size());
				submesh.m_nIndices = inputSubMesh.m_Indices.size();
				submesh.m_nVertices = inputSubMesh.m_Vertex.size();
				submesh.m_iMaterial = inputSubMesh.m_iMaterial;
				submesh.m_ScenePos = inputSubMesh.m_ScenePos;
				submesh.m_SceneRotate = inputSubMesh.m_SceneRotate;
				submesh.m_SceneScale = inputSubMesh.m_SceneScale;
				model.m_ModelVertex.insert(model.m_ModelVertex.end(), inputSubMesh.m_Vertex.begin(), inputSubMesh.m_Vertex.end());
				model.m_Indices.insert(model.m_Indices.end(), inputSubMesh.m_Indices.begin(), inputSubMesh.m_Indices.end());
				model.m_SubMesh.push_back(submesh);
				mesh.m_nLODs += inputSubMesh.m_Lods.size();

			}
			iLod = mesh.m_nLODs;
			model.m_Mesh.push_back(mesh);



		}

	}

}