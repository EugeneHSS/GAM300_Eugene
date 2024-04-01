#include "GraphicsResource/GeomStruct.h"
#include "Logger/Logger.h"
#include "Serialization/BinarySerializor.h"
namespace TDS
{

	void TDSModel::Serialize(std::string_view outPath)
	{

		std::ofstream fileStream(outPath.data(), std::ios::binary);
		if (!fileStream)
		{
			TDS_CRITICAL("Failed to open file {}", outPath.data());
			return;
		}
		const std::size_t bufferSize = 1024 * 1024;
		std::vector<char> buffer(bufferSize);
		fileStream.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

		auto serializeVec3 = [&](const Vec3& vec)
		{
			fileStream.write(reinterpret_cast<const char*>(&vec.x), sizeof(float));
			fileStream.write(reinterpret_cast<const char*>(&vec.y), sizeof(float));
			fileStream.write(reinterpret_cast<const char*>(&vec.z), sizeof(float));
		};

		auto serializeVec2 = [&](const Vec2& vec)
		{
			fileStream.write(reinterpret_cast<const char*>(&vec.x), sizeof(float));
			fileStream.write(reinterpret_cast<const char*>(&vec.y), sizeof(float));
		};

		auto serializeVec4 = [&](const Vec4& vec)
		{
			fileStream.write(reinterpret_cast<const char*>(&vec.x), sizeof(float));
			fileStream.write(reinterpret_cast<const char*>(&vec.y), sizeof(float));
			fileStream.write(reinterpret_cast<const char*>(&vec.z), sizeof(float));
			fileStream.write(reinterpret_cast<const char*>(&vec.w), sizeof(float));
		};

		auto serializeAABB = [&](const AABB& aabb)
		{
			Vec3 min, max;
			min = aabb.getMin();
			max = aabb.getMax();
			serializeVec3(min);
			serializeVec3(max);
		};

		auto serializeString = [&](const std::string& str)
		{
			std::uint16_t length = static_cast<std::uint16_t>(str.length());
			fileStream.write(reinterpret_cast<const char*>(&length), sizeof(length));
			fileStream.write(str.data(), length);
		};


		auto serializeBoneWeights = [&](const BoneWeights& boneWeights)
		{
			serializeVec4(boneWeights.m_BoneID);
			serializeVec4(boneWeights.m_Weights);
		};

		auto serializeLOD = [&](const Lod& lod)
		{
			fileStream.write(reinterpret_cast<const char*>(&lod.m_iSubmesh), sizeof(std::uint16_t));
			fileStream.write(reinterpret_cast<const char*>(&lod.m_nSubmesh), sizeof(std::uint16_t));
		};

		auto serializeMesh = [&](const Mesh& mesh)
		{
			serializeString(mesh.m_Name);
			fileStream.write(reinterpret_cast<const char*>(&mesh.m_nLODs), sizeof(mesh.m_nLODs));
			fileStream.write(reinterpret_cast<const char*>(&mesh.m_iLOD), sizeof(mesh.m_iLOD));
			serializeString(mesh.m_NodeName);
			serializeString(mesh.m_ParentNode);
		};

		auto serializeSubMesh = [&](const SubMesh& subMesh)
		{
			serializeVec3(subMesh.m_ScenePos);
			serializeVec3(subMesh.m_SceneRotate);
			serializeVec3(subMesh.m_SceneScale);
			fileStream.write(reinterpret_cast<const char*>(&subMesh.m_nFaces), sizeof(std::uint32_t));
			fileStream.write(reinterpret_cast<const char*>(&subMesh.m_iIndices), sizeof(std::uint32_t));
			fileStream.write(reinterpret_cast<const char*>(&subMesh.m_nIndices), sizeof(std::uint32_t));
			fileStream.write(reinterpret_cast<const char*>(&subMesh.m_iVertices), sizeof(std::uint32_t));
			fileStream.write(reinterpret_cast<const char*>(&subMesh.m_nVertices), sizeof(std::uint32_t));
			fileStream.write(reinterpret_cast<const char*>(&subMesh.m_iMaterial), sizeof(std::uint16_t));
		};

		auto serializeVertex = [&](const Vertex& vertex)
		{
			serializeVec3(vertex.m_Position);
			serializeVec3(vertex.m_Bitangent);
			serializeVec3(vertex.m_Tangent);
			serializeVec3(vertex.m_Normal);
			serializeVec2(vertex.m_UV);
			serializeVec4(vertex.m_Color);
			serializeVec4(vertex.m_BoneID);
			serializeVec4(vertex.m_Weights);

		};

		auto serializeIndices = [&](const std::vector<std::uint32_t>& indices)
		{
			std::uint32_t size = indices.size();
			fileStream.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
			fileStream.write(reinterpret_cast<const char*>(indices.data()), size * sizeof(std::uint32_t));
		};

		auto serializeMeshes = [&](const std::vector<Mesh>& meshes)
		{
			std::uint32_t size = meshes.size();
			fileStream.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
			for (const auto& mesh : meshes)
			{
				serializeMesh(mesh);
			}
		};

		auto serializeSubMeshes = [&](const std::vector<SubMesh>& subMeshes)
		{
			std::uint32_t size = subMeshes.size();
			fileStream.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
			for (const auto& subMesh : subMeshes)
			{
				serializeSubMesh(subMesh);
			}
		};

		auto serializeLODs = [&](const std::vector<Lod>& lods)
		{
			std::uint32_t size = lods.size();
			fileStream.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
			for (const auto& lod : lods)
			{
				serializeLOD(lod);
			}
		};

		auto serializeVertices = [&](const std::vector<Vertex>& vertices)
		{
			std::uint32_t size = vertices.size();
			fileStream.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
			for (const auto& vertex : vertices)
			{
				serializeVertex(vertex);
			}
		};

		serializeMeshes(m_Mesh);
		serializeSubMeshes(m_SubMesh);
		serializeLODs(m_LODs);
		serializeVertices(m_ModelVertex);
		serializeIndices(m_Indices);
		fileStream.close();


	}
	void TDSModel::Deserialize(std::string_view inPath)
	{
		std::ifstream fileStream(inPath.data(), std::ios::binary);
		if (!fileStream)
		{
			TDS_CRITICAL("Failed to open file {}", inPath.data());
			return;
		}

		auto deserializeVec3 = [&](Vec3& vec)
		{
			fileStream.read(reinterpret_cast<char*>(&vec.x), sizeof(float));
			fileStream.read(reinterpret_cast<char*>(&vec.y), sizeof(float));
			fileStream.read(reinterpret_cast<char*>(&vec.z), sizeof(float));
		};

		auto deserializeVec2 = [&](Vec2& vec)
		{
			fileStream.read(reinterpret_cast<char*>(&vec.x), sizeof(float));
			fileStream.read(reinterpret_cast<char*>(&vec.y), sizeof(float));
		};

		auto deserializeVec4 = [&](Vec4& vec)
		{
			fileStream.read(reinterpret_cast<char*>(&vec.x), sizeof(float));
			fileStream.read(reinterpret_cast<char*>(&vec.y), sizeof(float));
			fileStream.read(reinterpret_cast<char*>(&vec.z), sizeof(float));
			fileStream.read(reinterpret_cast<char*>(&vec.w), sizeof(float));
		};

		auto deserializeString = [&](std::string& str)
		{
			std::uint16_t length;
			fileStream.read(reinterpret_cast<char*>(&length), sizeof(length));
			str.resize(length);
			fileStream.read(&str[0], length);
		};

		auto deserializeAABB = [&](AABB& aabb)
		{
			Vec3 min, max;
			deserializeVec3(min);
			deserializeVec3(max);
			aabb.SetMinMax(min, max);
		};

		auto deserializeBoneWeights = [&](BoneWeights& boneWeights)
		{
			deserializeVec4(boneWeights.m_BoneID);
			deserializeVec4(boneWeights.m_Weights);
		};

		auto deserializeVertex = [&](Vertex& vertex)
		{
			deserializeVec3(vertex.m_Position);
			deserializeVec3(vertex.m_Bitangent);
			deserializeVec3(vertex.m_Tangent);
			deserializeVec3(vertex.m_Normal);
			deserializeVec2(vertex.m_UV);
			deserializeVec4(vertex.m_Color);
			deserializeVec4(vertex.m_BoneID);
			deserializeVec4(vertex.m_Weights);
		};

		auto deserializeMesh = [&](Mesh& mesh)
		{
			deserializeString(mesh.m_Name);
			fileStream.read(reinterpret_cast<char*>(&mesh.m_nLODs), sizeof(mesh.m_nLODs));
			fileStream.read(reinterpret_cast<char*>(&mesh.m_iLOD), sizeof(mesh.m_iLOD));
			deserializeString(mesh.m_NodeName);
			deserializeString(mesh.m_ParentNode);
		};

		auto deserializeSubMesh = [&](SubMesh& subMesh)
		{
			deserializeVec3(subMesh.m_ScenePos);
			deserializeVec3(subMesh.m_SceneRotate);
			deserializeVec3(subMesh.m_SceneScale);
			fileStream.read(reinterpret_cast<char*>(&subMesh.m_nFaces), sizeof(subMesh.m_nFaces));
			fileStream.read(reinterpret_cast<char*>(&subMesh.m_iIndices), sizeof(subMesh.m_iIndices));
			fileStream.read(reinterpret_cast<char*>(&subMesh.m_nIndices), sizeof(subMesh.m_nIndices));
			fileStream.read(reinterpret_cast<char*>(&subMesh.m_iVertices), sizeof(subMesh.m_iVertices));
			fileStream.read(reinterpret_cast<char*>(&subMesh.m_nVertices), sizeof(subMesh.m_nVertices));
			fileStream.read(reinterpret_cast<char*>(&subMesh.m_iMaterial), sizeof(subMesh.m_iMaterial));
		};

		auto deserializeLOD = [&](Lod& lod)
		{
			fileStream.read(reinterpret_cast<char*>(&lod.m_iSubmesh), sizeof(lod.m_iSubmesh));
			fileStream.read(reinterpret_cast<char*>(&lod.m_nSubmesh), sizeof(lod.m_nSubmesh));
		};
		auto deserializeVector = [&](auto& vec, auto deserializeFunc)
		{
			std::uint32_t size;
			fileStream.read(reinterpret_cast<char*>(&size), sizeof(size));
			vec.resize(size);
			for (auto& item : vec)
			{
				deserializeFunc(item);
			}
		};
		deserializeVector(m_Mesh, deserializeMesh);
		deserializeVector(m_SubMesh, deserializeSubMesh);
		deserializeVector(m_LODs, deserializeLOD);
		deserializeVector(m_ModelVertex, deserializeVertex);
		deserializeVector(m_Indices, [&](std::uint32_t& index) { fileStream.read(reinterpret_cast<char*>(&index), sizeof(index)); });
		
		fileStream.close();
	}
	
	void TDSSOAModel::Serialize(TDSSOAModel& model, std::string_view outPath)
	{
		BinarySerializor serializer{};


		if (serializer.OpenData(outPath, false) == BinarySerializor::ERROR_TYPE::FAILED)
			return;

		serializer.StartSerializer(&model);

		serializer.CloseData(false);


	}
	void TDSSOAModel::Deserialize(TDSSOAModel& model, std::string_view inPath)
	{
		BinarySerializor serializer{};


		if (serializer.OpenData(inPath, true) == BinarySerializor::ERROR_TYPE::FAILED)
			return;

		serializer.StartDeserializer(&model);

		serializer.CloseData(true);
	}

}