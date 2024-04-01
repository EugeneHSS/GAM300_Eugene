#include <pch.h>
#include "GraphicsResource/Revamped/MeshController.h"
#include "GraphicsResource/Revamped/ModelPack.h"
#include "vulkanTools/VMATypes/VMABuffer.h"
#include "Rendering/GraphicsManager.h"
namespace TDS
{

    bool MeshController::LoadMeshData()
    {
        std::map<std::string, int> duplicates;

        if (m_ModelPack->m_ModelName == "Bucket - Copy_Bin.bin")
        {
            std::cout << "Styop here" << std::endl;
        }
        for (size_t i = 0; i < m_ModelPack->m_ModelHandle.m_Mesh.size(); ++i)
        {
            auto& Mesh = m_ModelPack->m_ModelHandle.m_Mesh[i];
            auto& SubMesh = m_ModelPack->m_ModelHandle.m_SubMesh[i];

            size_t start = SubMesh.m_iVertices;
            size_t end = start + SubMesh.m_nVertices;

            std::vector<TDSModel::Vertex> vertexData;

            for (auto& vert : m_ModelPack->m_ModelHandle.m_ModelVertex)
            {
                vert.m_MeshID.y = vert.m_Color.w;
                vert.m_Color.w = 1.f;
            }

            vertexData.insert(vertexData.end(), m_ModelPack->m_ModelHandle.m_ModelVertex.begin() + start, m_ModelPack->m_ModelHandle.m_ModelVertex.begin() + end);

           

            Vec3 minBoundingBox(FLT_MAX, FLT_MAX, FLT_MAX);
            Vec3 maxBoundingBox(-FLT_MAX, -FLT_MAX, -FLT_MAX);
            for (auto& vertex : vertexData)
            {
                auto Vert = vertex.m_Position;
                minBoundingBox.x = Mathf::Min(minBoundingBox.x, Vert.x);
                minBoundingBox.y = Mathf::Min(minBoundingBox.y, Vert.y);
                minBoundingBox.z = Mathf::Min(minBoundingBox.z, Vert.z);

                maxBoundingBox.x = Mathf::Max(maxBoundingBox.x, Vert.x);
                maxBoundingBox.y = Mathf::Max(maxBoundingBox.y, Vert.y);
                maxBoundingBox.z = Mathf::Max(maxBoundingBox.z, Vert.z);
            }
            m_SceneBoundingBox.SetMinMax(minBoundingBox, maxBoundingBox);


            size_t indexStart = SubMesh.m_iIndices;
            size_t indexEnd = indexStart + (SubMesh.m_nFaces * 3);
            std::vector<std::uint32_t> indexData;

            indexData.insert(indexData.end(),
                m_ModelPack->m_ModelHandle.m_Indices.begin() + indexStart,
                m_ModelPack->m_ModelHandle.m_Indices.begin() + indexEnd);


            auto itr = m_MeshIDMap.find(Mesh.m_Name);

            if (itr != m_MeshIDMap.end())
            {
                Mesh.m_Name += "__";
                Mesh.m_Name += std::to_string(++duplicates[itr->first]);

            }

            m_MeshIDMap[Mesh.m_Name] = m_MeshIDMap.size();


            m_MeshBuffer.m_VertexBuffer = std::make_shared<VMABuffer>();
            m_MeshBuffer.m_VertexBuffer->MappedStaging(vertexData.size() * sizeof(TDSModel::Vertex), VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, GraphicsManager::getInstance().getVkInstance(), vertexData.data());
            m_MeshBuffer.m_VertexBuffer->SetDataCnt(vertexData.size());

            m_MeshBuffer.m_IndexBuffer = std::make_shared<VMABuffer>();
            m_MeshBuffer.m_IndexBuffer->MappedStaging(indexData.size() * sizeof(std::uint32_t), VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, GraphicsManager::getInstance().getVkInstance(), indexData.data());
            m_MeshBuffer.m_IndexBuffer->SetDataCnt(indexData.size());



        }

        m_Instancing = true;
        m_Loaded = true;
        return m_Loaded;
    }
    bool MeshController::LoadBatchData()
    {

        std::vector<TDSModel::Vertex> batchedVertexData;
        std::vector<std::uint32_t> batchedIndexData;

        std::vector<VkDrawIndexedIndirectCommand> indirectCommands;
        size_t currentVertexOffset = 0;
        size_t currentIndexOffset = 0; 

        std::map<std::string, int> duplicates;

        m_MeshCnt = std::uint32_t(m_ModelPack->m_ModelHandle.m_Mesh.size());
        for (size_t i = 0; i < m_MeshCnt; ++i)
        {
            auto& Mesh = m_ModelPack->m_ModelHandle.m_Mesh[i];
            auto& SubMesh = m_ModelPack->m_ModelHandle.m_SubMesh[i];


            std::uint32_t meshID = m_MeshOffset + i;

            auto itr = m_MeshIDMap.find(Mesh.m_Name);


            if (itr != m_MeshIDMap.end())
            {
                Mesh.m_Name += "__" + std::to_string(++duplicates[itr->first]);
            }

            m_MeshIDMap[Mesh.m_Name] = meshID;


            m_RootNodes[Mesh.m_NodeName].m_SceneTranslation = SubMesh.m_ScenePos;
            m_RootNodes[Mesh.m_NodeName].m_SceneRotation = SubMesh.m_SceneRotate;
            m_RootNodes[Mesh.m_NodeName].m_SceneScale = SubMesh.m_SceneScale;

            m_RootNodes[Mesh.m_NodeName].m_MeshList[Mesh.m_Name] = (MeshNode(true, Mesh.m_Name));



            Vec3 minBoundingBox(FLT_MAX, FLT_MAX, FLT_MAX);
            Vec3 maxBoundingBox(-FLT_MAX, -FLT_MAX, -FLT_MAX);

            for (size_t j = SubMesh.m_iVertices; j < SubMesh.m_iVertices + SubMesh.m_nVertices; ++j)
            {
                auto& vertex = m_ModelPack->m_ModelHandle.m_ModelVertex[j];
                auto Vert = vertex.m_Position;

                //if (m_ModelPack->m_ModelName == "Adjusted_Bin.bin")
                //{
                //    Mat4 sceneTransform = Mat4::Translate(SubMesh.m_ScenePos) * Mat4::Rotate(SubMesh.m_SceneRotate) * Mat4::Scale(SubMesh.m_SceneScale);
                //    Vert = sceneTransform * Vert;
                //}
                vertex.m_MeshID.x = float(meshID);

                batchedVertexData.push_back(vertex);

                minBoundingBox.x = Mathf::Min(minBoundingBox.x, Vert.x);
                minBoundingBox.y = Mathf::Min(minBoundingBox.y, Vert.y);
                minBoundingBox.z = Mathf::Min(minBoundingBox.z, Vert.z);

                maxBoundingBox.x = Mathf::Max(maxBoundingBox.x, Vert.x);
                maxBoundingBox.y = Mathf::Max(maxBoundingBox.y, Vert.y);
                maxBoundingBox.z = Mathf::Max(maxBoundingBox.z, Vert.z);
            }

            m_RootNodes[Mesh.m_NodeName].m_MeshList[Mesh.m_Name].m_MeshBoundingBox.SetMinMax(minBoundingBox, maxBoundingBox);


            for (size_t j = SubMesh.m_iIndices; j < SubMesh.m_iIndices + SubMesh.m_nIndices; ++j)
            {
                batchedIndexData.push_back(m_ModelPack->m_ModelHandle.m_Indices[j] + currentVertexOffset);
            }


            VkDrawIndexedIndirectCommand command = {};
            command.indexCount = SubMesh.m_nIndices;
            command.instanceCount = 1; 
            command.firstIndex = currentIndexOffset;
            command.vertexOffset = 0; 
            command.firstInstance = 0; 

            indirectCommands.push_back(command);

            // Update offsets
            currentIndexOffset += SubMesh.m_nIndices;
            currentVertexOffset += SubMesh.m_nVertices;
        }

        m_MeshBuffer.m_VertexBuffer = std::make_shared<VMABuffer>();
        m_MeshBuffer.m_VertexBuffer->MappedStaging(batchedVertexData.size() * sizeof(TDSModel::Vertex), VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, GraphicsManager::getInstance().getVkInstance(), batchedVertexData.data());
        m_MeshBuffer.m_VertexBuffer->SetDataCnt(batchedVertexData.size());

        m_MeshBuffer.m_IndexBuffer = std::make_shared<VMABuffer>();
        m_MeshBuffer.m_IndexBuffer->MappedStaging(batchedIndexData.size() * sizeof(std::uint32_t), VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT, GraphicsManager::getInstance().getVkInstance(), batchedIndexData.data());
        m_MeshBuffer.m_IndexBuffer->SetDataCnt(batchedIndexData.size());

        m_MeshBuffer.m_IndirectBuffer = std::make_shared<VMABuffer>();
        size_t indirectBufferSize = indirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand);
        m_MeshBuffer.m_IndirectBuffer->MappedStaging(indirectBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, GraphicsManager::getInstance().getVkInstance(), indirectCommands.data());
        m_MeshBuffer.m_IndirectBuffer->SetDataCnt(indirectCommands.size());
        m_Loaded = true;
        return m_Loaded;
    }

    void MeshController::Destroy()
    {
        vkQueueWaitIdle(GraphicsManager::getInstance().getVkInstance().getGraphicsQueue());

        if (m_MeshBuffer.m_VertexBuffer && m_MeshBuffer.m_IndexBuffer && m_MeshBuffer.m_IndirectBuffer)
        {
            m_MeshBuffer.m_VertexBuffer->DestroyBuffer();
            m_MeshBuffer.m_IndexBuffer->DestroyBuffer();
            m_MeshBuffer.m_IndirectBuffer->DestroyBuffer();
        }
        m_MeshBuffer.m_VertexBuffer = nullptr;
        m_MeshBuffer.m_IndexBuffer = nullptr;
        m_MeshBuffer.m_IndirectBuffer = nullptr;
        if (m_ModelPack)
        {
            delete m_ModelPack;
            m_ModelPack = nullptr;
        }
        m_RootNodes.clear();
        m_MeshIDMap.clear();
        //m_MeshIndexMap.clear();
        m_Loaded = false;

    }
    void MeshController::BuildMeshTree()
    {
        std::uint32_t i = 0;

        for (auto mesh : m_ModelPack->m_ModelHandle.m_Mesh)
        {
            auto& submesh = m_ModelPack->m_ModelHandle.m_SubMesh[i++];

            m_RootNodes[mesh.m_NodeName].m_MeshList[mesh.m_Name].m_FirstRender = true;
            m_RootNodes[mesh.m_NodeName].m_MeshList[mesh.m_Name].m_MeshName = mesh.m_Name;
            m_RootNodes[mesh.m_NodeName].m_SceneTranslation = submesh.m_ScenePos;
            m_RootNodes[mesh.m_NodeName].m_SceneRotation = submesh.m_SceneRotate;
            m_RootNodes[mesh.m_NodeName].m_SceneScale = submesh.m_SceneScale;
            m_RootNodes[mesh.m_NodeName].m_MeshList[mesh.m_Name].m_MaterialID = submesh.m_iMaterial;
            m_RootNodes[mesh.m_NodeName].m_NodeBoundingBox.extend(m_RootNodes[mesh.m_NodeName].m_MeshList[mesh.m_Name].m_MeshBoundingBox);


        }


        BuildSceneAABB();
    }


    Modelpack* MeshController::GetModelPack()
    {
        return m_ModelPack;
    }



    std::int32_t MeshController::GetMeshID(std::string_view meshName)
    {
        auto meshID = m_MeshIDMap.find(meshName.data());

        if (meshID != m_MeshIDMap.end())
        {
            return meshID->second;
        }
        return -1;
    }

    void MeshController::BuildSceneAABB()
    {

        for (auto& [rootName, sceneNode] : m_RootNodes)
        {
            for (auto& [meshName, mesh] : sceneNode.m_MeshList)
            {
                sceneNode.m_NodeBoundingBox.extend(mesh.m_MeshBoundingBox);
            }
            m_SceneBoundingBox.extend(sceneNode.m_NodeBoundingBox);
        }
    }

    void MeshController::ResetMeshPosition()
    {
        if (m_EntityToNodeName.empty())
            return;

        for (auto itr : m_EntityToNodeName)
        {
            auto findRoot = m_RootNodes.find(itr.second);

            if (findRoot == m_RootNodes.end()) continue;

            Vec3 translate = m_RootNodes[itr.second].m_SceneTranslation;
            Vec3 scale = m_RootNodes[itr.second].m_SceneScale;
            Vec3 Rotation = m_RootNodes[itr.second].m_SceneRotation;
            Transform* trans = ecs.getComponent<Transform>(itr.first);
            GraphicsComponent* graphComp = ecs.getComponent<GraphicsComponent>(itr.first);
            trans->SetPosition(m_RootNodes[itr.second].m_SceneTranslation);
            trans->SetScale(m_RootNodes[itr.second].m_SceneScale);
            trans->SetRotation(m_RootNodes[itr.second].m_SceneRotation);
            trans->GenerateTransform();

            graphComp->m_MeshName = m_RootNodes[itr.second].m_MeshList.begin()->first;


        }
        
    }


    MeshBuffer* MeshController::GetMeshBuffer()
    {
        return &m_MeshBuffer;
    }

    std::map<std::string, SceneNode>& MeshController::GetRoots()
    {
        return m_RootNodes;
    }




}

