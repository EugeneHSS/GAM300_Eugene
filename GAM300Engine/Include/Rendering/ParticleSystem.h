#ifndef PARTICLESYSTEM
#define PARTICLESYSTEM

#include "vulkanTools/vulkanInstance.h"
#include "vulkanTools/VulkanPipeline.h"
#include "components/ParticleComponent.h"
#include "dotnet/ImportExport.h"
#include "components/transform.h"
#include "GraphicsResource/Revamped/MeshController.h"
#include "ResourceManagement/ResourceRef.h"
#include "renderPass.h"

namespace TDS {



	class ParticleSystem {
	public:
		struct InputRenderBuffers 
		{
			TypeReference<MeshController> m_MeshReference;
		};

		struct ParticleInstanceGroup
		{
			InputRenderBuffers*			m_PRenderBuffers;
			std::uint32_t				m_ParticleAmount;
			float						m_EmitterLifetime;
		};

		struct CameraUBO 
		{
			Mat4 view;
			Mat4 Proj;
		};


		DLL_API ParticleSystem();
		DLL_API ~ParticleSystem();

		ParticleSystem(const ParticleSystem&) = delete;
		ParticleSystem& operator=(const ParticleSystem&) = delete;

		DLL_API static void Init();
		//sends data into compute shader for calculations
		DLL_API static void UpdateSystem(const float deltatime, const std::vector<EntityID>& Entities, Transform* Xform, Particle_Component* Particles);
		//Spawn/Draws all particles from all entities
		DLL_API static void Render();
		DLL_API static void ShutDown();
		DLL_API static void AddEmitterGroup(Particle_Component* Particles, std::uint32_t descIndex, std::uint32_t spawnAmount);
		//helper functions

		//for compute
		inline static std::shared_ptr<VulkanPipeline> m_ComputePipeline;
		inline static std::shared_ptr<VulkanPipeline> m_EmitterPipeline;
		//for rendering
		inline static std::shared_ptr<VulkanPipeline> m_RenderPipeline;
		//inline static std::array<InputRenderBuffers, ParticleMesh::MAX_MESHES>  MeshRenderBuffers;

		inline static std::vector<ParticleInstanceGroup>			m_Group;
		inline static std::uint32_t									m_GroupCnt;
	private:

		//std::shared_ptr<VMABuffer> m_ComputeBuffer;
		//std::shared_ptr<VMABuffer> m_EmitterBuffer;
		inline static std::shared_ptr<VMABuffer> m_IndexQuad = nullptr;
	};


}

#endif // !PARTICLESYSTEM

