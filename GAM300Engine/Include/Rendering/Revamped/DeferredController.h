#pragma once
#include "vulkanTools/FrameInfo.h"
namespace TDS
{
	enum DEFERRED_STAGE
	{
		STAGE_G_BUFFER_BATCH = 0,
		STAGE_G_BUFFER_INSTANCE,
		STAGE_LIGTHING,
		//STAGE_SHADOW,
		//STAGE_DEPTH_SHADOW,
		//STAGE_SHADOW_OMNI,

		STAGE_COMPOSITION,
		STAGE_MAX

	};

	enum RENDER_PASS
	{
		RENDER_G_BUFFER = 0,
		//RENDER_SHADOW_MAPPING,
		//RENDER_SHADOW_OMNI,
		RENDER_LIGTHING,
		//RENDER_POST,

		RENDER_COMPOSITION,
		RENDER_TOTAL
	};


	class VulkanPipeline;
	class MaterialBase;
	class DeferredFrames;

	struct Transform;
	struct GraphicsComponent;
	struct MeshBuffer;

	class FrameBufferObject;

	static constexpr int MAX_INSTANCE_BUFFER = 10000;
	static constexpr int MAX_POSSIBLE_BATCH = 10000;
	static constexpr int MAX_POINT_LIGHTS = 100;
	static constexpr int MAX_DIR_LIGHTS = 100;
	static constexpr int MAX_SPOT_LIGHTS = 100;

	struct Transform;





	struct MeshUpdate
	{
		Transform* m_pTransform = nullptr;
		int						m_MeshID;
		int						m_EntityID = -1;
		int						m_TextureID = -1;
		bool					m_IsAnimated = false;
		bool					m_ShowMesh = false;
	};

	struct UpdateData
	{
		std::vector<MeshUpdate>		m_MeshUpdates;
		MeshBuffer* m_MeshBuffer = nullptr;
	};


	struct alignas(16) BatchData
	{
		std::uint32_t	m_MaterialID;
		std::uint32_t	m_TextureID;
		std::uint32_t	m_isAnimated;
		std::uint32_t	m_EntityID;
		Mat4			m_modelMatrix;
	};

	//Batch Rendering Data

	struct Batch3D
	{

		std::array<BatchData, MAX_POSSIBLE_BATCH>												m_BatchBuffers;
		std::map<std::string, UpdateData>														m_BatchUpdateInfo;
		std::uint32_t																			m_BatchCnt;


	};

	//For single mesh, I would prefer to use instancing.
	struct Instance3D
	{

		struct InstanceRenderManager
		{
			struct RenderInstanceInfo
			{
				std::uint32_t			m_Instances = 0;
				std::uint32_t			m_InstanceOffset = 0;
			};


			struct InstanceRequest
			{
				MeshBuffer* m_MeshBuffer = nullptr;
				RenderInstanceInfo				m_RenderInstanceInfo;
			};


			struct InstanceUpdatePack
			{
				std::uint32_t				m_Index = 0;
				std::vector<MeshUpdate>		m_Updates;
			};
			std::map<MeshBuffer*, InstanceUpdatePack>	m_InstanceUpdateInfo;
		};
		std::uint32_t																m_TotalInstances = 0;
		std::uint32_t																m_GroupIdx = 0;
		InstanceRenderManager														m_instanceRenderManager;
		std::array<InstanceRenderManager::InstanceRequest, MAX_INSTANCE_BUFFER>		m_InstanceRequests;
		std::array<BatchData, MAX_INSTANCE_BUFFER>									m_InstanceBuffers;


	};


	struct alignas(16) SceneCamera
	{
		Mat4 view;
		Mat4 projection;
		Vec4 cameraPos;
	};


	struct alignas(16) LightingPushConstant
	{
		Vec4 ambientlightcolor = { 1.f, 1.f, 1.f, 0.02f };
		int activepointlights;
		int activeDirLights;
		int activeSpotLights;
		int toggleDebugLight = 1;
	};

	struct alignas(16) SpotLight
	{
		Vec4 Position;
		Vec4 direction;
		Vec4 Color;
		Vec4 attenuation;
		float cutoff;
		float outerCutoff;
		float padding[2] = { 0.f, 0.f };

		//float radius;
		//float padding;
	};
	struct alignas(16) Pointlight
	{
		Vec4 Position;
		Vec4 Color;
		Vec4 attenuation;

		//float radius;
		//float padding[3];
	};

	struct alignas(16) DirectionalLight
	{
		Vec4 direction = { 0.f, 0.f, -1.f, 0.f };
		Vec4 Color;
	};

	struct LightBuffers
	{
		std::array<Pointlight, 100>			m_PointLights;
		std::array<SpotLight, 100>			m_SpotLights;
		std::array<DirectionalLight, 100>	m_DirectionalLight;
	};
	struct alignas(16) LightSourceProperties
	{
		Vec4 Position;  
		Vec4 Color;     
		float Radius;   
		float padding[3]; 
	};



	struct SceneUniform
	{
		Mat4 m_Proj = Mat4(1.f);
		Mat4 m_View = Mat4(1.f);
	};




	class FBO;
	class PointLightComponent;
	class DirectionalLightComponent;
	class SpotLightComponent;

	class DeferredController
	{
	public:
		typedef std::array<std::shared_ptr<VulkanPipeline>, DEFERRED_STAGE::STAGE_MAX>			PIPELINE_LIST;

		void											Init(std::uint32_t w, std::uint32_t h);
		void											CreatePipelines();
		void											CreateFrameBuffers(std::uint32_t width, std::uint32_t height);
		void											Resize(std::uint32_t width, std::uint32_t height);
		void											UpdateDeferredTextures();
		//GBuffers
		void											G_BufferPassBatch(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);
		void											G_BufferInstanced(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);
		void											G_BufferPass(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);
		//LightingPass
		void											LightingPass(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);

		//Composition
		void											CombinationPass(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);
		void											SubmitPointLight(std::uint32_t entityID, PointLightComponent* graphComp, Transform* transformComp);
		void											SubmitDirectionalLight(std::uint32_t entityID, DirectionalLightComponent* graphComp, Transform* transformComp);
		void											SubmitSpotLight(std::uint32_t entityID, SpotLightComponent* graphComp, Transform* transformComp);

		void											ClearBatchSubmission();
		void											SubmitMesh(std::uint32_t entityID, GraphicsComponent* graphComp, Transform* transformComp);
		void											SubmitBatch(std::uint32_t entityID, int TextureID, Transform* transformComp, GraphicsComponent* graphComp);
		void											SubmitInstance(std::uint32_t entityID, int TextureID, Transform* transformComp, GraphicsComponent* graphComp);
		std::shared_ptr<VulkanPipeline>					GetDeferredPipeline(DEFERRED_STAGE stage);
		void											ShutDown();
		void											SetClearColour(Vec4 clearColor);
		void											UpdateClearColur();
		FBO*											GetFrameBuffer(RENDER_PASS renderpassType);

		SceneUniform&									GetSceneUniform();


	public:
		GlobalUBO globalUBO{}; //Temp for testing

	private:

		//Lighting pass data
		std::uint32_t									m_LightSrcInstance = 0;
		
		LightingPushConstant							m_LightingPushConstant;
		LightBuffers									m_LightUBOs;

		std::array<LightSourceProperties, 1000>			m_LightSourceBuffers;
		SceneCamera										m_SceneCamera;
		SceneUniform									m_SceneUBO;
		Batch3D											m_Batch3D;
		Instance3D										m_Instance3D;
		PIPELINE_LIST									m_DeferredPipelines;
		std::unique_ptr<VulkanPipeline>					m_LightSource;
		std::array<FBO*, RENDER_TOTAL>					m_FrameBuffers;




	};



}