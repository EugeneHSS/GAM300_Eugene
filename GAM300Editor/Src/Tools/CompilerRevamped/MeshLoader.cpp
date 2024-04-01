#include "Tools/CompilerRevamped/MeshLoader.h"
#include "meshoptimizer-master/src/meshoptimizer.h"
#include "Logger/Logger.h"
#include "Identifier/UniqueID.h"
#include "Rendering/GraphicsManager.h"
#include "GraphicsResource/GeomSerializer.h"
#include "Animation/AnimationLoader.h"


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
		int								iMatertialInstance{};
	};
	struct AnimModel
	{
		std::vector<AnimationMesh> m_MeshData;
		std::vector<AnimNode> m_Nodes;

		std::map<std::string, int> nodeMap;
		std::vector<Mat4> bones;
		std::map<std::string, unsigned int> boneMap;
		std::vector<Animation> animations;
	};

	inline Mat4 aiToMat4(aiMatrix4x4 mat);
	MeshLoader::MeshLoader()
	{
	}
	MeshLoader::~MeshLoader()
	{
	}

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
		//importer.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true);

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
			| aiProcess_RemoveRedundantMaterials
			| aiProcess_FindInvalidData
			| aiProcess_FlipUVs;

		if (request.currSetting.m_OptimizeNormals)
			flags |= aiProcess_GenSmoothNormals;
		else
			flags |= aiProcess_GenNormals;

		if (request.currSetting.m_MergeIdenticalVertices)
			flags != aiProcess_JoinIdenticalVertices;

		if (request.currSetting.m_RemoveChildMeshes)
			flags |= aiProcess_PreTransformVertices;

		currSceneInfo->m_Scene = importer.ReadFile(filePath, flags);

		if (currSceneInfo->m_Scene == nullptr)
		{
			TDS_LOGGER("Failed to read model : {}", filePath);
		}

		if (currSceneInfo->MeshValidityCheck(request) == false)
			return;


		std::vector<RawMeshData> assimpData;
		auto model = AnimModel{};

		if (request.currSetting.m_LoadAnimation && currSceneInfo->m_Scene->HasAnimations())
		{
			ImportAnimation(request, *currSceneInfo, model);

			assimpData.resize(model.m_MeshData.size());

			for (unsigned int l{ 0 }; l < model.m_MeshData.size(); l++)
			{
				for (int vert{ 0 }; vert < model.m_MeshData[l].m_AnimVertex.size(); vert++)
				{
					
					TDSModel::Vertex vertex{};
					vertex.m_Position = model.m_MeshData[l].m_AnimVertex[vert].m_Position;
					vertex.m_Normal = model.m_MeshData[l].m_AnimVertex[vert].m_Normal;
					vertex.m_UV = model.m_MeshData[l].m_AnimVertex[vert].m_UV;

					for (int vertElem{ 0 }; vertElem < 4; vertElem++)
					{
						if (model.m_MeshData[l].m_AnimVertex[vert].m_BoneIDs.size() <= vertElem)
						{
							vertex.m_BoneID[vertElem] = -1;
							vertex.m_Weights[vertElem] = 0;
						}
						else
						{
							vertex.m_BoneID[vertElem] = model.m_MeshData[l].m_AnimVertex[vert].m_BoneIDs[vertElem];
							vertex.m_Weights[vertElem] = model.m_MeshData[l].m_AnimVertex[vert].m_BoneWeights[vertElem];
						}
					}

					if(model.m_MeshData[l].m_AnimVertex[vert].m_BoneIDs.size() > 4)
						TDS_LOGGER("vertex influenced by more than 4 bones, but only 4 bones will be used!");

					
					assimpData[l].m_Vertices.push_back(vertex);
				}

				//indices
				assimpData[l].m_Indices = model.m_MeshData[l].m_Indices;
			}
		}
		else
		{
			if (request.currSetting.m_LoadModelAnimation)
			{
				ImportBonelessAnimation(request, *currSceneInfo);
			}
			


			ImportMeshData(request, *currSceneInfo, assimpData);
		}

		if (request.currSetting.m_LoadMaterials)
			LoadMaterialsMeta(*currSceneInfo, request, assimpData);


		MergeMesh(request, assimpData);
		CreateLODs(request, assimpData);

		GeomData data{};
		
		if (request.currSetting.m_Compress)
			OptimizeMesh(assimpData);

		CreateFinalGeom(assimpData, data, request, model);

		if (request.currSetting.m_LoadMesh)
		{
			if (request.currSetting.m_UseStreams)
			{
				data.ConvertToSOATDSModel(request.m_SOAOutput);
				TDSSOAModel::Serialize(request.m_SOAOutput, request.m_OutFile);
			}
			else
			{
				data.ConvertToTDSModel(request.m_Output);
				request.m_Output.Serialize(request.m_OutFile);
			}


			
		}

		if (request.currSetting.m_LoadAnimation)
		{
			AnimationData::Serialize(request.m_AnimationData, request.m_AnimOutFile, false);
		}

		if (request.currSetting.m_LoadMaterials)
		{
			std::string OutFilePath = MATERIAL_PATH + request.m_MaterialOutFile;
			MaterialData::Serialize(request.m_MaterialOut, OutFilePath, false);
		}
		//std::ofstream fileStream(request.m_OutFile.data(), std::ios::binary);
		//if (!fileStream)
		//{
		//	return;
		//}
		//SerializeGeom(request.m_Output, fileStream);

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
		
		ListNodes(assimp.m_Scene->mRootNode);
		ListMeshNames(assimp.m_Scene);
		ListMeshesInEveryNode(assimp.m_Scene->mRootNode, assimp.m_Scene);
		std::cout << totalmesh << std::endl;
		std::cout << totalNodesWithMeshes << std::endl;

		if (request.currSetting.m_Normalized)
		{
			Vec3 minBoundingBox(FLT_MAX, FLT_MAX, FLT_MAX);
			Vec3 maxBoundingBox(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			for (unsigned int m = 0; m < assimp.m_Scene->mNumMeshes; m++)
			{
				aiMesh* mesh = assimp.m_Scene->mMeshes[m];
				for (unsigned int v = 0; v < mesh->mNumVertices; v++)
				{
					aiVector3D vertex = mesh->mVertices[v];
					minBoundingBox.x = Mathf::Min(minBoundingBox.x, vertex.x);
					minBoundingBox.y = Mathf::Min(minBoundingBox.y, vertex.y);
					minBoundingBox.z = Mathf::Min(minBoundingBox.z, vertex.z);

					maxBoundingBox.x = Mathf::Max(maxBoundingBox.x, vertex.x);
					maxBoundingBox.y = Mathf::Max(maxBoundingBox.y, vertex.y);
					maxBoundingBox.z = Mathf::Max(maxBoundingBox.z, vertex.z);
				}


			}

			m_BoundingBoxScene.SetMinMax(minBoundingBox, maxBoundingBox);
		}


		
		ProcessScene(assimpData, *assimp.m_Scene->mRootNode, *assimp.m_Scene, assimp.m_AppliedTransformation, rootName, request);
		
	}
	void MeshLoader::ImportBonelessAnimation(Request& request, AssimpSceneInfo& assimp)
	{
		for (std::uint32_t i = 0; i < assimp.m_Scene->mNumAnimations; ++i)
		{
			auto& aiAnim = assimp.m_Scene->mAnimations[i];
			auto& back = request.m_BonelessAnimationData.m_Animations.emplace_back();
			back.m_duration = aiAnim->mDuration;
			back.m_ticksPerSecond = aiAnim->mTicksPerSecond;
			for (std::uint32_t j = 0; j < aiAnim->mNumChannels; ++j)
			{
				auto& aiChannel = aiAnim->mChannels[j];
				auto& newChannel = back.m_channels.emplace_back();

				newChannel.m_name = aiChannel->mNodeName.C_Str();

				for (std::uint32_t k = 0; k < aiChannel->mNumPositionKeys; ++k)
				{
					auto& newPositionKey = newChannel.m_positions.emplace_back();
					newPositionKey.m_time = aiChannel->mPositionKeys[k].mTime;

					newPositionKey.m_Pos = Vec3(aiChannel->mPositionKeys[k].mValue.x, aiChannel->mPositionKeys[k].mValue.y, aiChannel->mPositionKeys[k].mValue.z);

				}

				for (std::uint32_t k = 0; k < aiChannel->mNumRotationKeys; ++k)
				{
					auto& newRotationKeys = newChannel.m_rotationsQ.emplace_back();
					newRotationKeys.m_time = aiChannel->mRotationKeys[k].mTime;

					newRotationKeys.m_RotQ.x = aiChannel->mRotationKeys[k].mValue.x;
					newRotationKeys.m_RotQ.y = aiChannel->mRotationKeys[k].mValue.y;
					newRotationKeys.m_RotQ.z = aiChannel->mRotationKeys[k].mValue.z;
					newRotationKeys.m_RotQ.w = aiChannel->mRotationKeys[k].mValue.w;

				}
				
				for (std::uint32_t k = 0; k < aiChannel->mNumScalingKeys; ++k)
				{
					auto& newScalingKeys = newChannel.m_scalings.emplace_back();
					newScalingKeys.m_time = aiChannel->mScalingKeys[k].mTime;

					newScalingKeys.m_Scale.x = aiChannel->mScalingKeys[k].mValue.x;
					newScalingKeys.m_Scale.y = aiChannel->mScalingKeys[k].mValue.y;
					newScalingKeys.m_Scale.z = aiChannel->mScalingKeys[k].mValue.z;
		
				}
				
				

			}

		}
	}
	void MeshLoader::ImportAnimation(Request& request, AssimpSceneInfo& assimp, AnimModel& model)
	{
		
		AnimProcessNode(request, &model, assimp.m_Scene->mRootNode, assimp.m_Scene, aiMatrix4x4(), -1);

		//BoneMap

		for (auto& e : model.boneMap)
		{
			model.m_Nodes[model.nodeMap[e.first]].m_BoneID = e.second;
			model.m_Nodes[model.nodeMap[e.first]].m_boneOffset = model.bones[e.second];
		}

		TDS_LOGGER("model bone count: ", model.bones.size());

		TDS_LOGGER("model animation count: ", assimp.m_Scene->mNumAnimations);

		for (size_t i = 0; i < assimp.m_Scene->mNumAnimations; i++) {
			buildAnimation(&model, assimp.m_Scene->mAnimations[i]);
		}

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

	void MeshLoader::ProcessScene(std::vector<RawMeshData>& assimpData, const aiNode& Node, const aiScene& Scene, aiMatrix4x4& ParentTransform, std::string_view ParentName, Request& request)
	{
		aiMatrix4x4 AccumulatedTransform = ParentTransform * Node.mTransformation;
		aiMatrix4x4 transformWithoutTrans;
		size_t iNode = assimpData.size();

		assimpData.resize(iNode + Scene.mNumMeshes);

		Vec3 center = (m_BoundingBoxScene.getMax() + m_BoundingBoxScene.getMin()) / 2.0f;
		Vec3 size = m_BoundingBoxScene.getMax() - m_BoundingBoxScene.getMin();
		float maxDimension = std::max({ size.x, size.y, size.z });

		float scale = 1.0f / maxDimension;

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
					{
						std::uint32_t uVChannels = mesh.GetNumUVChannels();
						if (uVChannels >= 1)
						{
							return uVChannels - 1;
						}
					}
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

			aiMatrix4x4 translateMat = GetTranslationMatrix(AccumulatedTransform);
			aiMatrix4x4 rotationMat = GetRotationMatrix(AccumulatedTransform);
			aiMatrix4x4 scaleMat = GetScaleMatrix(AccumulatedTransform);

			aiMatrix4x4 identity;

			transformWithoutTrans = identity * rotationMat * scaleMat;

			aiVector3D translate{};

			ExtractTranslation(AccumulatedTransform, translate);
			aiVector3D rot = ExtractEulerAngles(AccumulatedTransform);
			aiVector3D scaling;
			ExtractScale(AccumulatedTransform, scaling);
			rawMesh.m_ParentNode = ParentName;
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

				aiVector3D L;
				if (request.currSetting.m_PretransformedVertices)
					L = AccumulatedTransform * mesh.mVertices[i];
				else
					L = mesh.mVertices[i];

				if (request.currSetting.m_Normalized)
				{
					L.x = (L.x - center.x) * scale;
					L.y = (L.y - center.y) * scale;
					L.z = (L.z - center.z) * scale;
				}


				vert.m_Position = Vec3(static_cast<float>(L.x), static_cast<float>(L.y), static_cast<float>(L.z));



				if (iUV == -1)
					vert.m_UV = Vec2(0.f, 0.f);
				else
					vert.m_UV = Vec2(static_cast<float>(mesh.mTextureCoords[iUV][i].x), static_cast<float>(mesh.mTextureCoords[iUV][i].y));

				if (iColor == -1)
					vert.m_Color = { 1.f, 1.f, 1.f, 0.f };
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
					aiVector3D T{}, B{}, N{};
					if (request.currSetting.m_PretransformedVertices)
					{
						T = currRotation.Rotate(mesh.mTangents[i]);
						B = currRotation.Rotate(mesh.mBitangents[i]);
						N = currRotation.Rotate(mesh.mNormals[i]);
					}
					else
					{
						T = mesh.mTangents[i];
						B = mesh.mBitangents[i];
						N = mesh.mNormals[i];
					}

					
					vert.m_Normal = { N.x, N.y, N.z };
					vert.m_Tangent = { T.x, T.y, T.z };
					vert.m_Bitangent = { B.x, B.y, B.z };
					
					vert.m_Normal.Normalize();
					vert.m_Tangent.Normalize();
					vert.m_Bitangent.Normalize();
				}
				else
				{
					aiVector3D N{};

					if (request.currSetting.m_PretransformedVertices)
						N = currRotation.Rotate(mesh.mNormals[i]);
					
					else
						N = mesh.mNormals[i];
					
					
					vert.m_Normal = { N.x, N.y, N.z };
					vert.m_Tangent = { 1.f, 0.f, 0.f };
					vert.m_Bitangent = { 1.f, 0.f, 0.f };
					vert.m_Normal.Normalize();
				
				}
				vert.m_MeshID.y = mesh.mMaterialIndex;
				vert.m_Color.w = mesh.mMaterialIndex;
			}
			for (std::uint32_t i = 0; i < mesh.mNumFaces; ++i)
			{
				auto& face = mesh.mFaces[i];
				for (std::uint32_t j = 0; j < face.mNumIndices; ++j)
					rawMesh.m_Indices.emplace_back(face.mIndices[j]);

			}

			rawMesh.iMatertialInstance = mesh.mMaterialIndex;
			
			/*if (request.currSetting.m_LoadMaterials)
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
			}*/
			//rawMesh.iMatertialInstance = mesh.mMaterialIndex;

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

	
	void MeshLoader::AnimProcessNode(Request& request, AnimModel* model, aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform, int parentNode)
	{
		aiMatrix4x4 transform = parentTransform * node->mTransformation;

		model->m_Nodes.push_back(AnimNode{});
		model->m_Nodes.back().m_ParentNode = parentNode;
		model->m_Nodes.back().m_Transform = aiToMat4(node->mTransformation);

		int thisID = static_cast<int>(model->m_Nodes.size() - 1);
		model->nodeMap[node->mName.C_Str()] = thisID;
		if (parentNode >= 0)
			model->m_Nodes[parentNode].m_Child.push_back(thisID);

		for (unsigned int i{ 0 }; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			model->m_MeshData.push_back(AnimationMesh{});
			AnimProcessMesh(request, model, mesh, scene, transform);
		}


		for (unsigned int i{ 0 }; i < node->mNumChildren; i++)
		{
			AnimProcessNode(request, model, node->mChildren[i], scene, transform, thisID);
		}

	}
	void MeshLoader::AnimProcessMesh(Request& request, AnimModel* model, aiMesh* aimesh, const aiScene* scene, aiMatrix4x4 transform)
	{
		AnimationMesh* mesh = &model->m_MeshData[model->m_MeshData.size() - 1];
		mesh->m_BindTransform = aiToMat4(transform);

		//vertcies
		for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
			AnimationVertex vertex;
			vertex.m_Position.x = aimesh->mVertices[i].x;
			vertex.m_Position.y = aimesh->mVertices[i].y;
			vertex.m_Position.z = aimesh->mVertices[i].z;
			//vertex.m_Position.normalize();

			if (aimesh->HasNormals()) {
				vertex.m_Normal.x = aimesh->mNormals[i].x;
				vertex.m_Normal.y = aimesh->mNormals[i].y;
				vertex.m_Normal.z = aimesh->mNormals[i].z;
				//vertex.m_Normal.normalize();
			}
			else
				vertex.m_Normal = Vec3(0.f);

			if (aimesh->mTextureCoords[0]) {
				vertex.m_UV.x = aimesh->mTextureCoords[0][i].x;
				vertex.m_UV.y = aimesh->mTextureCoords[0][i].y;
				//vertex.m_UV.normalize();
			}
			else
				vertex.m_UV = Vec2(0.f);

			mesh->m_AnimVertex.push_back(vertex);
		}
		//bones - relies on verticies
		for (unsigned int i = 0; i < aimesh->mNumBones; i++) {
			auto aibone = aimesh->mBones[i];
			unsigned int boneID;
			std::string boneName = aibone->mName.C_Str();
			if (model->boneMap.find(boneName) == model->boneMap.end()) {
				model->bones.push_back(aiToMat4(aibone->mOffsetMatrix) * mesh->m_BindTransform);
				boneID = (unsigned int)(model->bones.size() - 1);
				model->boneMap[boneName] = boneID;
			}
			else
				boneID = model->boneMap[boneName];

			for (unsigned int weightI = 0; weightI < aibone->mNumWeights; weightI++) {
				auto vertexWeight = aibone->mWeights[weightI];
				mesh->m_AnimVertex[vertexWeight.mVertexId].m_BoneIDs.push_back(boneID == -1 ? 0 : boneID);
				mesh->m_AnimVertex[vertexWeight.mVertexId].m_BoneWeights.push_back(vertexWeight.mWeight);
			}
		}
		//indicies
		for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
			aiFace face = aimesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				mesh->m_Indices.push_back(face.mIndices[j]);
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
		if (request.currSetting.m_MergeMesh)
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

					/*if (assimpData[i].m_MeshName == assimpData[j].m_MeshName &&
						assimpData[i].iMatertialInstance == assimpData[j].iMatertialInstance)
					{*/
						std::int32_t iVertex = std::int32_t(assimpData[i].m_Vertices.size());
						std::int32_t iIndex = std::int32_t(assimpData[i].m_Indices.size());
						std::vector<TDSModel::Vertex> temp = assimpData[j].m_Vertices;

						assimpData[i].m_Vertices.insert(assimpData[i].m_Vertices.end(), assimpData[j].m_Vertices.begin(), assimpData[j].m_Vertices.end());
						assimpData[i].m_Indices.insert(assimpData[i].m_Indices.end(), assimpData[j].m_Indices.begin(), assimpData[j].m_Indices.end());

						for (size_t k = iIndex; k < assimpData[i].m_Indices.size(); ++k)
						{
							assimpData[i].m_Indices[k] += iVertex;
						}

						assimpData.erase(assimpData.begin() + j);
						--j;

					/*}*/
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

	void MeshLoader::CreateFinalGeom(const std::vector<RawMeshData>& rawMesh, GeomData& geom, Request& request, const AnimModel& model)
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


		for (auto& animation : model.animations)
			request.m_AnimationData.m_Animations.push_back(animation);

		for (auto& bone : model.bones)
			request.m_AnimationData.m_Bones.push_back(bone);

	}

	void MeshLoader::buildAnimation(AnimModel* data, aiAnimation* aiAnim)
	{
		data->animations.push_back(TDS::Animation());
		auto anim = &data->animations[data->animations.size() - 1];
		anim->m_name = aiAnim->mName.C_Str();


		//copy nodes from the fbx
		anim->m_nodes.resize(data->m_Nodes.size());
		for (int i{ 0 }; i < data->m_Nodes.size(); i++)
		{
			anim->m_nodes[i].m_ModelNode = data->m_Nodes[i];
		}

		anim->m_duration = aiAnim->mDuration;
		anim->m_ticks = aiAnim->mTicksPerSecond/* * 0.001f*/; //in terms of millisec time
		if (anim->m_ticks == 0.f)
		{
			//if fbx some how does specify any tics/ms assume 1/ms
			anim->m_ticks = 1.f;
		}


		//setup the animation properties for anime nodes
		for (unsigned int i{ 0 }; i < aiAnim->mNumChannels; i++)
		{
			auto channel = aiAnim->mChannels[i];
			std::string nodeName = channel->mNodeName.C_Str();
			auto node = &anim->m_nodes[data->nodeMap[nodeName]];
			extractKeyFrame(node, channel);
		}

	}

	void MeshLoader::extractKeyFrame(TDS::AnimationNodes* pNode, aiNodeAnim* AssimpNode)
	{
		//key for pos
		for (unsigned int pos{ 0 }; pos < AssimpNode->mNumPositionKeys; pos++)
		{
			auto poskey = &AssimpNode->mPositionKeys[pos];
			TDS::AnimPos animPos;
			animPos.m_Pos = Vec3(poskey->mValue.x, poskey->mValue.y, poskey->mValue.z)/*.normalize()*/;
			animPos.m_time = poskey->mTime;
			pNode->m_positions.push_back(animPos);
		}

		//key for rot
		for (unsigned int rotQ{ 0 }; rotQ < AssimpNode->mNumRotationKeys; rotQ++)
		{
			auto rotKey = &AssimpNode->mRotationKeys[rotQ];
			TDS::AnimRotQ animRotQ;
			animRotQ.m_RotQ = Quat(rotKey->mValue.x, rotKey->mValue.y, rotKey->mValue.z, rotKey->mValue.w);
			animRotQ.m_time = rotKey->mTime;
			pNode->m_rotationsQ.push_back(animRotQ);
		}


		//key for scale
		for (unsigned int scale{ 0 }; scale < AssimpNode->mNumScalingKeys; scale++)
		{
			auto scaleKey = &AssimpNode->mScalingKeys[scale];
			AnimScale scaling;
			scaling.m_Scale = Vec3(scaleKey->mValue.x, scaleKey->mValue.y, scaleKey->mValue.z)/*.normalize()*/;
			scaling.m_time = scaleKey->mTime;
			pNode->m_scalings.push_back(scaling);
		}
	}

	void MeshLoader::LoadMaterialsMeta(AssimpSceneInfo& assimp, Request& request, std::vector<RawMeshData>& assimpData)
	{

		auto RemoveFileExtension = [](std::string& fileName)
		{
			size_t lastindex = fileName.find_last_of(".");
			fileName = fileName.substr(0, lastindex);
		};
		auto getShadingType = [](int& ShadingType) -> MaterialAttributes::SHADING_TYPE
		{
			switch (ShadingType)
			{
			case aiShadingMode_Flat:
			case aiShadingMode_Phong:
			case aiShadingMode_Blinn:       return MaterialAttributes::SHADING_TYPE::SHADING_PHONG_BLINN;
			case aiShadingMode_OrenNayar:
			case aiShadingMode_Minnaert:
			case aiShadingMode_Fresnel:
			case aiShadingMode_CookTorrance:
			case aiShadingMode_PBR_BRDF:    return MaterialAttributes::SHADING_TYPE::SHADING_PBR;
			case aiShadingMode_NoShading:
			default:                        return MaterialAttributes::SHADING_TYPE::SHADING_NONE;

			}
		};


		for (auto& Mesh : assimpData)
		{
			auto mat = assimp.m_Scene->mMaterials[Mesh.iMatertialInstance];
			
			auto& MaterialAttri = request.m_MaterialOut.m_materialAttributes[Mesh.iMatertialInstance];
			MaterialAttri.m_MaterialName = mat->GetName().C_Str();

			
			int shadingType = -1;
			aiGetMaterialInteger(mat, AI_MATKEY_SHADING_MODEL, (int*)&shadingType);

			MaterialAttri.m_shading = getShadingType(shadingType);

			//Get the main texture

			aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);

			if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString texturePath;
				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), (int*)&mapU);
				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), (int*)&mapV);

				if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {

					std::string fileName = std::string(texturePath.C_Str());
					RemoveFileExtension(fileName);
					fileName += ".dds";
					MaterialAttri.m_AlbedoTex = fileName;
				}
				else
				{
					for (std::uint32_t i = 0; i < mat->mNumProperties; ++i)
					{
						const auto& Props = *mat->mProperties[i];
						if (Props.mType == aiPTI_String)
						{
							if (Props.mSemantic != aiTextureType_NONE)
							{
								aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U(aiTextureType_BASE_COLOR, 0), (int*)&mapU);
								aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V(aiTextureType_BASE_COLOR, 0), (int*)&mapV);
								if (Props.mSemantic == aiTextureType_BASE_COLOR)
								{
									texturePath = *(aiString*)Props.mData;
									auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
									NewTextureInfo.m_UMode = mapU;
									NewTextureInfo.m_VMode = mapV;
									break;
								}
								else if (Props.mSemantic == aiTextureType_UNKNOWN)
								{
									std::string defaultClr = std::string(((aiString*)Props.mData)->C_Str());
									if (defaultClr.find("_Base_Color") != -1)
									{
										texturePath = *(aiString*)Props.mData;
										auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();

										NewTextureInfo.m_UMode = mapU;
										NewTextureInfo.m_VMode = mapV;
										break;
									}

								}
							}
						}

						std::string fileName = std::string(texturePath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";
						MaterialAttri.m_AlbedoTex = fileName;
		

					}
				}
			}


			//Phong Blinn
			{

				{
					aiColor4D C(1, 1, 1, 1);
					if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&C) == AI_SUCCESS)
					{
						MaterialAttri.m_phongBlinn.m_DiffuseColor = Vec4(C.r, C.g, C.b, C.a);
					}
				}

				{
					aiColor4D C(0, 0, 0, 1);
					if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, (aiColor4D*)&C) == AI_SUCCESS)
					{
						MaterialAttri.m_phongBlinn.m_AmbientColor = Vec4(C.r, C.g, C.b, C.a);
					}
				}

				{
					aiColor4D C(0, 0, 0, 1);
					if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, (aiColor4D*)&C) == AI_SUCCESS)
					{
						MaterialAttri.m_phongBlinn.m_SpecularColor = Vec4(C.r, C.g, C.b, C.a);
					}
				}


				{
					aiColor4D C(0, 0, 0, 1);
					if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&C) == AI_SUCCESS)
					{
						MaterialAttri.m_phongBlinn.m_EmissiveColor = Vec4(C.r, C.g, C.b, C.a);
					}
				}
				{
					float val = -1.f;
					if (aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, (float*)&val) == AI_SUCCESS)
					{
						MaterialAttri.m_phongBlinn.m_Shininess = val;
					}
				}
				{
					float val = -1.f;
					if (aiGetMaterialFloat(mat, AI_MATKEY_SHININESS_STRENGTH, (float*)&val) == AI_SUCCESS)
					{
						MaterialAttri.m_phongBlinn.m_ShininessStrength = val;
					}
				}
				//get phong textures
				{
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), (int*)&mapV);
					aiString         texPath;

					if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS > 0)
					{
						
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_phongBlinn.m_DiffuseTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}

				
				{
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_AMBIENT(0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_AMBIENT(0), (int*)&mapV);
					aiString         texPath;

					if (mat->GetTexture(aiTextureType_AMBIENT, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_phongBlinn.m_AmbientTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}
				{
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_SPECULAR(0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_SPECULAR(0), (int*)&mapV);
					aiString         texPath;

					if (mat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_phongBlinn.m_SpecularTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}

				{
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_EMISSIVE(0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_EMISSIVE(0), (int*)&mapV);
					aiString         texPath;

					if (mat->GetTexture(aiTextureType_EMISSIVE, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_phongBlinn.m_EmissiveTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}
				{
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_SHININESS(0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_SHININESS(0), (int*)&mapV);
					aiString         texPath;

					if (mat->GetTexture(aiTextureType_SHININESS, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_phongBlinn.m_ShininessTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}
			}

			//PBR
			{
				{
					float metalness = -1;
					if (aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, (float*)&metalness) == AI_SUCCESS)
					{
						MaterialAttri.m_PBRAttributes.m_Metalness = metalness;
					}
				}

				{
					float roughness = -1;
					if (aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, (float*)&roughness) == AI_SUCCESS)
					{
						MaterialAttri.m_PBRAttributes.m_Roughness = roughness;
					}
				}


				{
					float emissiveness = -1;
					if (aiGetMaterialFloat(mat, AI_MATKEY_EMISSIVE_INTENSITY, (float*)&emissiveness) == AI_SUCCESS)
					{
						MaterialAttri.m_PBRAttributes.m_Emissive = emissiveness;
					}
				}
				{
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U(aiTextureType_NORMAL_CAMERA, 0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V(aiTextureType_NORMAL_CAMERA, 0), (int*)&mapV);
					aiString         texPath;

					if (mat->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_PBRAttributes.m_NormalTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}
				{
					aiString         texPath;
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U(aiTextureType_DIFFUSE_ROUGHNESS, 0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V(aiTextureType_DIFFUSE_ROUGHNESS, 0), (int*)&mapV);
					if (mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_PBRAttributes.m_RoughnessTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}
				{
					aiString         texPath;
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U(aiTextureType_EMISSION_COLOR, 0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V(aiTextureType_EMISSION_COLOR, 0), (int*)&mapV);
					if (mat->GetTexture(aiTextureType_EMISSION_COLOR, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_PBRAttributes.m_EmissiveTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}
				{
					aiString         texPath;
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U(aiTextureType_AMBIENT_OCCLUSION, 0), (int*)&mapU);
					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V(aiTextureType_AMBIENT_OCCLUSION, 0), (int*)&mapV);
					if (mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texPath) == AI_SUCCESS > 0)
					{
						std::string fileName = std::string(texPath.C_Str());
						RemoveFileExtension(fileName);
						fileName += ".dds";

						MaterialAttri.m_PBRAttributes.m_AOTex = fileName;
						auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
						NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
						NewTextureInfo.m_UMode = mapU;
						NewTextureInfo.m_VMode = mapV;
					}
				}

			}




			//Others

			{
				aiColor4D C(0, 0, 0, 1);
				if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_REFLECTIVE, (aiColor4D*)&C) == AI_SUCCESS)
				{
					MaterialAttri.m_Reflectance = Vec4(C.r, C.g, C.b, C.a);
				}
				
			}
			{
				float val = 0.f;
				if (aiGetMaterialFloat(mat, AI_MATKEY_REFLECTIVITY, (float*)&val) == AI_SUCCESS)
				{
					MaterialAttri.m_Reflectivity = val;
				}
			}
			{
				aiString         texPath;
				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U(aiTextureType_REFLECTION, 0), (int*)&mapU);
				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V(aiTextureType_REFLECTION, 0), (int*)&mapV);
				if (mat->GetTexture(aiTextureType_REFLECTION, 0, &texPath) == AI_SUCCESS > 0)
				{
					std::string fileName = std::string(texPath.C_Str());
					RemoveFileExtension(fileName);
					fileName += ".dds";

					MaterialAttri.m_ReflectanceTexture = fileName;
					auto& NewTextureInfo = request.m_MaterialOut.m_TextureToload.emplace_back();
					NewTextureInfo.m_TexturePath = std::string(texPath.C_Str());
					NewTextureInfo.m_UMode = mapU;
					NewTextureInfo.m_VMode = mapV;
				}
			}

		}

		std::filesystem::path filePath(request.m_OutFile);

		request.m_MaterialOut.m_ModelName = filePath.filename().string();
	}


	//void MeshLoader::LoadMaterials(AssimpSceneInfo& assimp, Request& request, std::vector<RawMeshData>& assimpData)
	//{

	//


	//	for (auto& Mesh : assimpData)
	//	{
	//		auto mat = assimp.m_Scene->mMaterials[Mesh.iMatertialInstance];
	//		auto& MatInstance = request
	//		auto& MatInstance = request.m_MaterialData.m_MaterialInfos.emplace_back();

	//		MatInstance.m_MaterialName = mat->GetName().C_Str();

	//		int shadingType = -1;
	//		aiGetMaterialInteger(mat, AI_MATKEY_SHADING_MODEL, (int*)&shadingType);
	//		switch (shadingType)
	//		{
	//			case aiShadingMode_Gouraud:     MatInstance.m_ShadingType = SHADING_TYPE::GOURAUD; break;
	//			case aiShadingMode_Flat:
	//			case aiShadingMode_Phong:
	//			case aiShadingMode_Blinn:       MatInstance.m_ShadingType = SHADING_TYPE::PHONG_BLINN; break;
	//			case aiShadingMode_Toon:        MatInstance.m_ShadingType = SHADING_TYPE::TOOL; break;
	//			case aiShadingMode_NoShading:   MatInstance.m_ShadingType = SHADING_TYPE::UNLIGHT; break;
	//			case aiShadingMode_OrenNayar:
	//			case aiShadingMode_Minnaert:
	//			case aiShadingMode_Fresnel:
	//			case aiShadingMode_CookTorrance:
	//			case aiShadingMode_PBR_BRDF:    MatInstance.m_ShadingType = SHADING_TYPE::PBR; break;
	//			default:                        MatInstance.m_ShadingType = SHADING_TYPE::UNKNOWN; break;
	//			
	//		}

	//		{
	//			aiColor4D C(1, 1, 1, 1);
	//			aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&C);
	//			MatInstance.m_MaterialColor[MATERIAL_COLOR::MATERIAL_COLOR_DIFFUSE] = Vec4(C.r, C.g, C.b, C.a);
	//		}
	//		{
	//			aiColor4D C(0, 0, 0, 1);
	//			aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, (aiColor4D*)&C);
	//			MatInstance.m_MaterialColor[MATERIAL_COLOR::MATERIAL_COLOR_AMBIENT] = Vec4(C.r, C.g, C.b, C.a);
	//		}
	//		{
	//			aiColor4D C(0, 0, 0, 1);
	//			aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, (aiColor4D*)&C);
	//			MatInstance.m_MaterialColor[MATERIAL_COLOR::MATERIAL_COLOR_EMISSIVE] = Vec4(C.r, C.g, C.b, C.a);
	//		}

	//	
	//			aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &MatInstance.m_MaterialFloat[MATERIAL_FLOAT::MATERIAL_FLOAT_OPACITY]);
	//			aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &MatInstance.m_MaterialFloat[MATERIAL_FLOAT::MATERIAL_FLOAT_SHININESS]);
	//			aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &MatInstance.m_MaterialFloat[MATERIAL_FLOAT::MATERIAL_FLOAT_SHININESS_STRENGTH]);

	//			{
	//				aiString texPath{};
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				if (aiGetMaterialString(mat, AI_MATKEY_TEXTURE_DIFFUSE(0), &texPath) != AI_SUCCESS)
	//				{
	//					for (std::uint32_t i = 0; i < mat->mNumProperties; ++i)
	//					{
	//						const auto& Props = *mat->mProperties[i];
	//						if (Props.mType == aiPTI_String)
	//						{
	//							if (Props.mSemantic != aiTextureType_NONE)
	//							{
	//								if (Props.mSemantic == aiTextureType_BASE_COLOR)
	//								{
	//									texPath = *(aiString*)Props.mData;
	//									break;
	//								}
	//								else if (Props.mSemantic == aiTextureType_UNKNOWN)
	//								{
	//									std::string defaultClr = std::string(((aiString*)Props.mData)->C_Str());
	//									if (defaultClr.find("_Base_Color") != -1)
	//									{
	//										texPath = *(aiString*)Props.mData;
	//										break;
	//									}
	//									
	//								}
	//							}
	//						}
	//					}
	//				}
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), (int*)&mapV);

	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_DIFFUSE].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());


	//			}

	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				aiGetMaterialString(mat, AI_MATKEY_TEXTURE_SPECULAR(0), &texPath);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_SPECULAR(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_SPECULAR(0), (int*)&mapV);
	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_SPECULAR].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());
	//			}
	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				if (AI_SUCCESS == aiGetMaterialString(mat, AI_MATKEY_TEXTURE_OPACITY(0), &texPath))
	//				{
	//					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_OPACITY(0), (int*)&mapU);
	//					aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_OPACITY(0), (int*)&mapV);
	//					auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_OPACITY].emplace_back();
	//					texInfo.m_TextureName = std::string(texPath.C_Str());
	//				}
	//				else
	//				{
	//					int flags = 0;
	//					aiGetMaterialInteger(mat, AI_MATKEY_TEXFLAGS_DIFFUSE(0), &flags);

	//					auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_OPACITY].emplace_back();
	//					texInfo.m_TextureName = std::string(texPath.C_Str());
	//				}
	//			}

	//			// Ambient Texture
	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				aiGetMaterialString(mat, AI_MATKEY_TEXTURE_AMBIENT(0), &texPath);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_AMBIENT(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_AMBIENT(0), (int*)&mapV);
	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_AMBIENT].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());
	//			}

	//			// Emmisive Texture
	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				aiGetMaterialString(mat, AI_MATKEY_TEXTURE_EMISSIVE(0), &texPath);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_EMISSIVE(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_EMISSIVE(0), (int*)&mapV);
	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_EMISSIVE].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());
	//			}

	//			// Shininess Texture
	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				aiGetMaterialString(mat, AI_MATKEY_TEXTURE_SHININESS(0), &texPath);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_SHININESS(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_SHININESS(0), (int*)&mapV);
	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_SHININESS].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());
	//			}

	//			// Lightmap Texture
	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				aiGetMaterialString(mat, AI_MATKEY_TEXTURE_LIGHTMAP(0), &texPath);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_LIGHTMAP(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_LIGHTMAP(0), (int*)&mapV);
	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_LIGHTMAP].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());
	//			}

	//			// Normal Texture
	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				aiGetMaterialString(mat, AI_MATKEY_TEXTURE_NORMALS(0), &texPath);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_NORMALS(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_NORMALS(0), (int*)&mapV);
	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_NORMALS].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());
	//			}

	//			// Height Texture
	//			{
	//				aiString         texPath;
	//				aiTextureMapMode mapU(aiTextureMapMode_Wrap), mapV(aiTextureMapMode_Wrap);
	//				aiGetMaterialString(mat, AI_MATKEY_TEXTURE_HEIGHT(0), &texPath);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_U_HEIGHT(0), (int*)&mapU);
	//				aiGetMaterialInteger(mat, AI_MATKEY_MAPPINGMODE_V_HEIGHT(0), (int*)&mapV);
	//				auto& texInfo = MatInstance.m_TexturesPath[MATERIALS_TEXTURE::MATERIAL_HEIGHT].emplace_back();
	//				texInfo.m_TextureName = std::string(texPath.C_Str());
	//			}

	//	}
	//}



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
	void MeshLoader::GeomData::ConvertToSOATDSModel(TDSSOAModel& model)
	{
		std::uint32_t iLod = 0;
		for (auto& inputMesh : m_Meshes)
		{
			TDSSOAModel::Mesh mesh{};
			mesh.m_Name = inputMesh.m_Name;
			mesh.m_NodeName = inputMesh.m_NodeName;
			mesh.m_ParentNode = inputMesh.m_ParentNode;
			mesh.m_iLOD = iLod;

			for (const auto& inputSubMesh : inputMesh.m_Submeshes)
			{
				TDSSOAModel::SubMesh submesh{};
				submesh.m_nFaces = std::uint32_t(inputSubMesh.m_Indices.size() / 3);
				submesh.m_iIndices = std::uint32_t(model.m_Indices.size());
				submesh.m_iVertices = std::uint32_t(model.m_Position.size());
				submesh.m_nIndices = inputSubMesh.m_Indices.size();
				submesh.m_nVertices = inputSubMesh.m_Vertex.size();
				submesh.m_iMaterial = inputSubMesh.m_iMaterial;
				submesh.m_ScenePos = inputSubMesh.m_ScenePos;
				submesh.m_SceneRotate = inputSubMesh.m_SceneRotate;
				submesh.m_SceneScale = inputSubMesh.m_SceneScale;

				model.m_Position.resize(inputSubMesh.m_Vertex.size());
				model.m_Bitangent.resize(inputSubMesh.m_Vertex.size());
				model.m_Tangent.resize(inputSubMesh.m_Vertex.size());
				model.m_Normal.resize(inputSubMesh.m_Vertex.size());
				model.m_UV.resize(inputSubMesh.m_Vertex.size());
				model.m_BoneID.resize(inputSubMesh.m_Vertex.size());
				model.m_Weights.resize(inputSubMesh.m_Vertex.size());
				for (size_t i = 0; i < inputSubMesh.m_Vertex.size(); ++i)
				{
					model.m_Position[i] = inputSubMesh.m_Vertex[i].m_Position;
					model.m_Bitangent[i] = inputSubMesh.m_Vertex[i].m_Bitangent;
					model.m_Tangent[i] = inputSubMesh.m_Vertex[i].m_Tangent;
					model.m_Normal[i] = inputSubMesh.m_Vertex[i].m_Normal;
					model.m_UV[i] = inputSubMesh.m_Vertex[i].m_UV;
					model.m_BoneID[i] = inputSubMesh.m_Vertex[i].m_BoneID;
					model.m_Weights[i] = inputSubMesh.m_Vertex[i].m_Weights;
					
				}
				model.m_Indices.insert(model.m_Indices.end(), inputSubMesh.m_Indices.begin(), inputSubMesh.m_Indices.end());
				model.m_SubMesh.push_back(submesh);
				mesh.m_nLODs += inputSubMesh.m_Lods.size();

			}
			iLod = mesh.m_nLODs;
			model.m_Mesh.push_back(mesh);



		}
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
	inline Mat4 aiToMat4(aiMatrix4x4 mat)
	{
		Mat4 convertMatrix{};

		convertMatrix.m[0][0] = mat.a1;
		convertMatrix.m[0][1] = mat.b1;
		convertMatrix.m[0][2] = mat.c1;
		convertMatrix.m[0][3] = mat.d1;

		convertMatrix.m[1][0] = mat.a2;
		convertMatrix.m[1][1] = mat.b2;
		convertMatrix.m[1][2] = mat.c2;
		convertMatrix.m[1][3] = mat.d2;

		convertMatrix.m[2][0] = mat.a3;
		convertMatrix.m[2][1] = mat.b3;
		convertMatrix.m[2][2] = mat.c3;
		convertMatrix.m[2][3] = mat.d3;

		convertMatrix.m[3][0] = mat.a4;
		convertMatrix.m[3][1] = mat.b4;
		convertMatrix.m[3][2] = mat.c4;
		convertMatrix.m[3][3] = mat.d4;

		return convertMatrix;
	}

}