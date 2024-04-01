#include "Rendering/ParticleSystem.h"
#include "vulkanTools/GlobalBufferPool.h"
#include "Rendering/GraphicsManager.h"
#include "vulkanTools/Renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Rendering/Revamped/DeferredController.h"


#define MAX_PARTICLES 100000
#define MAX_EMITTERS 2

namespace TDS {



	ParticleSystem::ParticleSystem() {

	}

	ParticleSystem::~ParticleSystem() {

	}

	void ParticleSystem::Init() {
		//create vulkan stuff to create

		//create a spawn pipeline here
		m_EmitterPipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry EmitterComputeEntry;
		EmitterComputeEntry.m_NumDescriptorSets = MAX_EMITTERS;
		EmitterComputeEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		EmitterComputeEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::COMPUTE_SHADER, "../assets/shaders/ParticleEmitter.spv"));

		for (std::uint32_t i = 0; i < MAX_EMITTERS; ++i)
		{
			std::string particleOutName = "v_ParticleOut" + std::to_string(i);
			std::string freeListName = "FreeList" + std::to_string(i);
			GlobalBufferPool::GetInstance()->AddToGlobalPool(MAX_PARTICLES * sizeof(Particle), 31, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, particleOutName, nullptr, i);
			GlobalBufferPool::GetInstance()->AddToGlobalPool((MAX_PARTICLES + 1) * sizeof(int), 32, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, freeListName, nullptr, i);
		}
		m_EmitterPipeline->Create(EmitterComputeEntry);

		//compute pipeline
		m_ComputePipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry ParticleComputeEntry;
		ParticleComputeEntry.m_NumDescriptorSets = MAX_EMITTERS;
		ParticleComputeEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_ZERO;
		ParticleComputeEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::COMPUTE_SHADER, "../assets/shaders/ParticleCompute.spv"));
		//for (std::uint32_t i = 0; i < MAX_EMITTERS; ++i)
		//{
		//	GlobalBufferPool::GetInstance()->AddToGlobalPool(MAX_PARTICLES * sizeof(Mat4), 34, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "v_TransformMatrix", nullptr, i);
		//}




		m_ComputePipeline->Create(ParticleComputeEntry);


		//rendering pipeline
		m_RenderPipeline = std::make_shared<VulkanPipeline>();

		PipelineCreateEntry ParticleRenderEntry;
		ParticleRenderEntry.m_NumDescriptorSets = MAX_EMITTERS;
		ParticleRenderEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_SRC_ALPHA;
		ParticleRenderEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		ParticleRenderEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_SRC_ALPHA;
		ParticleRenderEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		ParticleRenderEntry.m_FBTarget = GraphicsManager::getInstance().GetDeferredController()->GetFrameBuffer(RENDER_PASS::RENDER_G_BUFFER);
		ParticleRenderEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/RenderParticleVert.spv"));
		ParticleRenderEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/RenderParticleFrag.spv"));

		/*VertexLayout layout = VertexLayout({
			VertexBufferElement(VAR_TYPE::VEC2, "in_Position")
			});*/
			//ParticleRenderEntry.m_ShaderInputs.m_InputVertex.push_back(VertexBufferInfo(false, layout, sizeof(Vec2)));

		/*MeshRenderBuffers[CUBE].m_MeshReference.m_ResourcePtr = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("cube_Bin.bin", MeshRenderBuffers[CUBE].m_MeshReference);

		MeshRenderBuffers[SPHERE].m_MeshReference.m_ResourcePtr = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("cube_Bin.bin", MeshRenderBuffers[SPHERE].m_MeshReference);

		MeshRenderBuffers[CAPSULE].m_MeshReference.m_ResourcePtr = AssetManager::GetInstance()->GetMeshFactory().GetMeshController("cube_Bin.bin", MeshRenderBuffers[CAPSULE].m_MeshReference);*/

		m_RenderPipeline->Create(ParticleRenderEntry);

		//struct FreeList
		//{
		//	int count = MAX_PARTICLES;
		//	std::array<int, MAX_PARTICLES> arr;
		//}initList;

		//Memset the array to 0
		//for (int i = 0; i < MAX_PARTICLES; ++i)
		//	initList.arr[i] = i;


		//m_RenderPipeline->UpdateUBO(&initList, sizeof(FreeList), 32, 0);

		//creating index buffer for quad
		std::vector<std::uint32_t> IndexQuad;
		IndexQuad.push_back(0);
		IndexQuad.push_back(1);
		IndexQuad.push_back(2);
		IndexQuad.push_back(0);
		IndexQuad.push_back(2);
		IndexQuad.push_back(3);
		m_IndexQuad = std::make_shared<VMABuffer>();
		m_IndexQuad->CreateIndexBuffer(IndexQuad.size() * sizeof(std::uint32_t), false, IndexQuad.data());

	}



	void ParticleSystem::UpdateSystem(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* EmitterList) {
		//send data into compute shader for calculations
		/*
		* loop through all entities with the particle component and run the emitter computeshader
		*/
		if (Entities.empty()) return;

		uint32_t currentframe = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();

		auto commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
		m_EmitterPipeline->SetCommandBuffer(commandBuffer);
		m_EmitterPipeline->BindComputePipeline();
		for (unsigned int i{ 0 }; i < Entities.size(); ++i) {
			Particle_Component& currentEmitter = EmitterList[i];
			currentEmitter.GetSpawnTimer() += deltatime;

			//currentEmitter.SetSpawnTimer(currentEmitter.GetSpawnTimer() + deltatime);

			int SpawnAmt = currentEmitter.GetSpawnTimer() / currentEmitter.GetSpawnInterval();

			if (SpawnAmt <= 0)
				continue;
			currentEmitter.GetEmitter().Position = Xform[i].GetPosition();
			Particle_Emitter_PushData GPUPush = { currentEmitter.GetEmitter(),
												std::min(SpawnAmt, currentEmitter.GetMaxParticles()),
												currentEmitter.GetSpawnTimer()
			};

			m_EmitterPipeline->UpdateUBO(&GPUPush, sizeof(Particle_Emitter_PushData), 33, currentframe, 0, false, i);

			AddEmitterGroup(&currentEmitter, i, SpawnAmt);
			//ParticleInstanceGroup* group = (m_GroupCnt >= m_Group.size()) ? &m_Group.emplace_back() : &m_Group[m_GroupCnt];
			//m_GroupCnt++;

			//group->m_ParticleAmount = SpawnAmt;
			//group->m_PRenderBuffers = &MeshRenderBuffers[EmitterList[i].GetMeshType()];
			m_EmitterPipeline->BindDescriptor(0, 1, 0, true, i);

			m_EmitterPipeline->DispatchCompute(ceil((SpawnAmt + 64) / 64), 1, 1);

		}

		VkMemoryBarrier memBarrier{};
		memBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0, 1, &memBarrier, 0, nullptr, 0, nullptr);

		m_ComputePipeline->SetCommandBuffer(commandBuffer);

		float dt = TimeStep::GetDeltaTime();
		struct timeblock {
			float deltatime;
			float EmitterRunTime;
		}TimeBlock;
		TimeBlock.deltatime = dt;
		m_ComputePipeline->BindComputePipeline();
		for (unsigned int i{ 0 }; i < Entities.size(); ++i)
		{
			Particle_Component& currentEmitter = EmitterList[i];
			float EmitterRunTime = currentEmitter.GetSpawnTimer();
			TimeBlock.EmitterRunTime = EmitterRunTime;
			m_ComputePipeline->BindDescriptor(0, 1, 0, true, i);
			m_ComputePipeline->UpdateUBO(&dt, sizeof(float), 35, currentframe, 0, false, i);
			int numwrkgrp = (currentEmitter.GetMaxParticles() + 128 - 1) / 128;
			m_ComputePipeline->DispatchCompute(numwrkgrp, 1, 1);
		}
		//m_ComputePipeline->UpdateUBO(test.data(), sizeof(Particle) * test.size(), 31, currentframe, 0, true);
	}

	void ParticleSystem::Render()
	{
		uint32_t currentframe = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();

		//camera stuffy
		TDSCamera cam = GraphicsManager::getInstance().GetCamera();
		Mat4 view = cam.GetViewMatrix();
		Mat4 proj = Mat4::Perspective(cam.m_Fov * Mathf::Deg2Rad,
			GraphicsManager::getInstance().GetSwapchainRenderer().getAspectRatio(), 0.1f, 1000000.f);
		proj.m[1][1] *= -1;

		//send data into vertex and fragment shader to render into scene
		auto commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
		m_RenderPipeline->SetCommandBuffer(commandBuffer);
		m_RenderPipeline->BindPipeline();
		CameraUBO temp = { view, proj };



		for (std::uint32_t i = 0; i < m_GroupCnt; ++i)
		{
			//m_RenderPipeline->BindVertexBuffer(*m_Group[i].m_PRenderBuffers->m_MeshReference.m_ResourcePtr->GetMeshBuffer()->m_VertexBuffer);
			//m_RenderPipeline->BindIndexBuffer(*m_Group[i].m_PRenderBuffers->m_MeshReference.m_ResourcePtr->GetMeshBuffer()->m_IndexBuffer);
			m_RenderPipeline->UpdateUBO(&temp, sizeof(CameraUBO), 5, currentframe, 0, false, i);
			m_RenderPipeline->BindIndexBuffer(*m_IndexQuad);
			m_RenderPipeline->BindDescriptor(currentframe, 1, 0, false, i);

			m_RenderPipeline->Draw(6, currentframe, m_Group[i].m_ParticleAmount);
			//m_RenderPipeline->DrawInstancedIndexed(*m_Group[i].m_PRenderBuffers->m_MeshReference.m_ResourcePtr->GetMeshBuffer()->m_VertexBuffer, *m_Group[i].m_PRenderBuffers->m_MeshReference.m_ResourcePtr->GetMeshBuffer()->m_IndexBuffer, m_Group[i].m_ParticleAmount, currentframe);
		}

		m_GroupCnt = 0;
	}

	void ParticleSystem::ShutDown() {
		m_ComputePipeline->ShutDown();
		m_EmitterPipeline->ShutDown();
		m_RenderPipeline->ShutDown();
		m_IndexQuad->DestroyBuffer();
	}
	void ParticleSystem::AddEmitterGroup(Particle_Component* Particles, std::uint32_t descIndex, std::uint32_t spawnAmount)
	{
		bool IncreaseGroupCnt = (m_GroupCnt >= m_Group.size());
		ParticleInstanceGroup* group = IncreaseGroupCnt ? &m_Group.emplace_back() : &m_Group[m_GroupCnt];

		m_GroupCnt++;
		if (IncreaseGroupCnt)
		{
			struct FreeList
			{
				int count = MAX_PARTICLES;
				std::array<int, MAX_PARTICLES> arr;
			}initList;




			for (int i = 0; i < MAX_PARTICLES; ++i)
				initList.arr[i] = i;

			m_EmitterPipeline->UpdateUBO(&initList, sizeof(FreeList), 32, 0, 0, false, descIndex);
			m_EmitterPipeline->UpdateUBO(&initList, sizeof(FreeList), 32, 1, 0, false, descIndex);
		}
		group->m_ParticleAmount = spawnAmount;
		/*group->m_PRenderBuffers = &MeshRenderBuffers[EmitterList[i].GetMeshType()];*/
	}
}