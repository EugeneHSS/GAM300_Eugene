#pragma once
#include "MathCommon.h"
#include "Geometry/Geometry.h"

namespace TDS
{
	struct TDSModel
	{
		struct Lod
		{
			std::uint16_t	m_iSubmesh{1};
			std::uint16_t	m_nSubmesh{1};
		};

		struct Mesh
		{
			std::string				m_Name;
			std::uint16_t			m_nLODs;
			std::uint16_t			m_iLOD;
			std::string				m_NodeName;
			std::string				m_ParentNode;
		};

		struct SubMesh
		{
			Vec3					m_ScenePos;
			Vec3					m_SceneRotate;
			Vec3					m_SceneScale;
			std::uint32_t			m_nFaces;
			std::uint32_t			m_iIndices;
			std::uint32_t			m_nIndices;
			std::uint32_t			m_iVertices;
			std::uint32_t			m_nVertices;
			std::uint16_t			m_iMaterial;
		};

		struct BoneWeights
		{
			Vec4							m_BoneID;
			Vec4							m_Weights;
		};

		struct Vertex
		{
			Vec3				m_Position{};
			Vec3				m_Bitangent{};
			Vec3				m_Tangent{};
			Vec3				m_Normal{};
			Vec2				m_UV{};
			Vec4				m_Color{};
			Vec4				m_BoneID{};
			Vec4				m_Weights{};
			Vec2				m_MeshID{};

		};

		std::vector<Mesh>			m_Mesh;
		std::vector<SubMesh>		m_SubMesh;
		std::vector<Lod>			m_LODs;
		std::vector<Vertex>			m_ModelVertex;
		std::vector<std::uint32_t>	m_Indices;

		void DLL_API Serialize(std::string_view outPath);
		void DLL_API Deserialize(std::string_view inPath);
	};


	struct TDSSOAModel
	{
		struct Lod
		{
			std::uint16_t	m_iSubmesh{1};
			std::uint16_t	m_nSubmesh{1};
		};

		struct Mesh
		{
			std::string				m_Name;
			std::uint16_t			m_nLODs;
			std::uint16_t			m_iLOD;
			std::string				m_NodeName;
			std::string				m_ParentNode;
		};

		struct SubMesh
		{
			Vec3					m_ScenePos;
			Vec3					m_SceneRotate;
			Vec3					m_SceneScale;
			std::uint32_t			m_nFaces;
			std::uint32_t			m_iIndices;
			std::uint32_t			m_nIndices;
			std::uint32_t			m_iVertices;
			std::uint32_t			m_nVertices;
			std::uint16_t			m_iMaterial;
		};

		struct BoneWeights
		{
			Vec4							m_BoneID;
			Vec4							m_Weights;
		};

		std::vector<Mesh>			m_Mesh;
		std::vector<SubMesh>		m_SubMesh;
		std::vector<Lod>			m_LODs;
		std::vector<Vec4>			m_BoneID;
		std::vector<Vec4>			m_Weights;
		std::vector<Vec3>			m_Position;
		std::vector<Vec3>			m_Bitangent;
		std::vector<Vec3>			m_Tangent;
		std::vector<Vec3>			m_Normal;
		std::vector<Vec2>			m_UV;
		std::vector<Vec2>			m_MeshID;

		std::vector<std::uint32_t>	m_Indices;

		static void DLL_API Serialize(TDSSOAModel& model, std::string_view outPath);
		static void DLL_API Deserialize(TDSSOAModel& model, std::string_view inPath);
	};


}