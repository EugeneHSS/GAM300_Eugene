#pragma once
#include <pch.h>
#include "Geometry/AABB.h"
#include "dotnet/ImportExport.h"
#include "ModelPack.h"
namespace TDS
{
	class VMABuffer;

	struct MeshBuffer
	{
		std::uint32_t m_VertexOffset, m_VertexCount, m_IndexOffset, m_IndexCount;
		std::shared_ptr<VMABuffer>							m_VertexBuffer = nullptr;
		std::shared_ptr<VMABuffer>							m_IndexBuffer = nullptr;
		std::shared_ptr<VMABuffer>							m_IndirectBuffer = nullptr;
	};

	struct Modelpack;

	struct MeshNode
	{
		bool												m_FirstRender;
		std::string											m_MeshName;
		AABB												m_MeshBoundingBox;
		int													m_MaterialID;
	};

	struct SceneNode
	{
		std::string											m_NodeName = "";
		std::map<std::string, MeshNode>						m_MeshList;
		Vec3												m_SceneTranslation;
		Vec3												m_SceneRotation;
		Vec3												m_SceneScale;
		AABB												m_NodeBoundingBox;
	};


	class MeshController
	{
	private:
		AABB												m_SceneBoundingBox;
		std::uint32_t										m_MeshOffset = 0;
		MeshBuffer											m_MeshBuffer;
		std::uint32_t										m_MeshCnt = 0;
		std::map<std::string, SceneNode>					m_RootNodes;
		std::unordered_map<std::string, std::uint32_t>		m_MeshIDMap;
		

	public:
		std::unordered_map<std::uint32_t, std::string>		m_EntityToNodeName;
		Modelpack* m_ModelPack = nullptr;

	public:
		bool												m_Instancing = false;
		bool												m_Loaded = false;

		MeshController() = default;
		~MeshController() = default;

		DLL_API bool							LoadMeshData();

		DLL_API bool							LoadBatchData();

		DLL_API void							Destroy();

		DLL_API void							BuildMeshTree();

		DLL_API void							BuildSceneAABB();

		DLL_API void							ResetMeshPosition();

		DLL_API Modelpack* GetModelPack();

		DLL_API std::int32_t					GetMeshID(std::string_view meshName);

		DLL_API MeshBuffer* GetMeshBuffer();

		inline std::uint32_t					GetMeshOffset() const
		{
			return m_MeshOffset;
		}

		inline std::uint32_t					GetMeshCnt() const
		{
			return m_MeshCnt;
		}

		inline void								SetMeshOffset(std::uint32_t offset)
		{
			m_MeshOffset = offset;
		}
		inline AABB&							GetSceneBoundingBox()
		{
			return m_SceneBoundingBox;
		}
		DLL_API std::map<std::string, SceneNode>& GetRoots();


	};



}