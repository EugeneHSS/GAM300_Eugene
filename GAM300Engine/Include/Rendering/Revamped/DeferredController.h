#pragma once
#include "vulkanTools/FrameInfo.h"
#include "GraphicsResource/Revamped/MaterialAttributes.h"
namespace TDS
{
	const uint32_t MAX_BONES = 1000;
	const uint32_t MAX_BONES_PER_MESH = 80;
	enum DEFERRED_STAGE
	{
		STAGE_G_BUFFER_BATCH = 0,
		STAGE_G_BUFFER_INSTANCE,
		STAGE_LIGTHING,
		STAGE_SHADOW,
		//STAGE_SHADOW,
		//STAGE_DEPTH_SHADOW,
		//STAGE_SHADOW_OMNI,
		STAGE_3D_COMPOSITION_INSTANCE,
		STAGE_3D_COMPOSITION_BATCH,
		STAGE_COMPOSITION,
		STAGE_MAX

	};

	enum RENDER_PASS
	{
		RENDER_G_BUFFER = 0,
		RENDER_SHADOW,
		//RENDER_SHADOW_MAPPING,
		//RENDER_SHADOW_OMNI,
		RENDER_LIGTHING,
		//RENDER_POST,

		RENDER_COMPOSITION,
		RENDER_TOTAL
	};


	class VulkanPipeline;
	class DeferredFrames;

	struct Transform;
	struct GraphicsComponent;
	struct MeshBuffer;

	class FrameBufferObject;
	static constexpr int MAX_MATERIALS = 1000;
	static constexpr int MAX_INSTANCE_BUFFER = 10000;
	static constexpr int MAX_POSSIBLE_BATCH = 10000;
	static constexpr int MAX_POINT_LIGHTS = 100;
	static constexpr int MAX_DIR_LIGHTS = 100;
	static constexpr int MAX_SPOT_LIGHTS = 100;

	struct Transform;

	class AnimationPlayer;

	struct alignas(16) MaterialBuffer
	{
		
		typedef Vec3 vec3;
		
		vec3 diffuse = vec3(0.0);
		int diffuseTex = -1;

		vec3 specular = vec3(0.0);
		int specularTex = -1;

		vec3 emissive = vec3(0.0);
		int emissiveTex = -1;

		vec3 ambient = vec3(0.0);
		int ambientTex = -1;

		int shininessTex = -1;
		float shininess = 0.0;
		float shininessStrength = 0.0;
		float reflectivity = 0.0;

		//General Material
		vec3 reflectance = vec3(0.0);
		int reflectanceTex = -1;

		//PBR 
		//int normalTex = -1;
		//int RoughnessTex = -1;
		//int MetallicTex = -1;
		//int aoTex = -1;

		//float metalness;
		//float roughness;
		//float emission;
		//int  albedoTex = -1;

		int  MaterialID = -1;
		int  ShadingModel = 0;
		int  UseMaterialTextures = 0;
		int  UseMaterialNormal = 0;
	};


	struct MeshUpdate
	{
		std::string				m_ModelName = "";
		Transform*				m_pTransform = nullptr;
		AnimationPlayer*		m_pAnimationPlayer = nullptr;
		MaterialAttributes*		m_pMaterialAttribute = nullptr;
		int						m_MeshID;
		int						m_EntityID = -1;
		int						m_TextureID = -1;
		int						m_MaterialID = -1;
		bool					m_IsAnimated = false;
		bool					m_ShowMesh = false;
		bool					m_RenderIn2D = false;
		bool					m_UseMaterials = false;
		bool					m_UsePreloadedMaterials = false;
	};

	struct UpdateData
	{
		std::vector<MeshUpdate>		m_MeshUpdates;
		MeshBuffer* m_MeshBuffer = nullptr;
		bool		m_UsedIn2D = false;
	};


	struct alignas(16) BatchData
	{
		Mat4			m_modelMatrix;

		std::int32_t	m_MaterialID;
		std::uint32_t	m_TextureID;
		std::uint32_t	m_IsRender;
		std::uint32_t	m_EntityID;

		std::uint32_t	m_AnimOffset;
		std::uint32_t	m_IsAnimated;
		std::uint32_t   m_UseMeshMatID;
		std::uint32_t	m_UseMaterials;


		MaterialBuffer  m_ComponentMaterial;
	};


	//Batch Rendering Data

	struct Batch3D
	{

		std::array<BatchData, MAX_POSSIBLE_BATCH>												m_BatchBuffers;
		/*std::array<MaterialBuffer, MAX_POSSIBLE_BATCH>	m_MaterialList;*/
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
				std::string						m_ModelName;
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
		/*std::array<MaterialBuffer, MAX_INSTANCE_BUFFER>	m_MaterialList;*/


	};


	struct alignas(16) SceneCamera
	{
		Mat4 view;
		Mat4 projection;
		Vec4 cameraPos;
	};


	struct alignas(16) LightingPushConstant
	{
		Vec4 ambientlightcolor = { 1.f, 1.f, 1.f, 0.01f };
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

	//struct BoneUniform
	//{
	//	alignas(16) Mat4 m_Bones[MAX_BONES];
	//};
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
		void											SubmitMeshForUI(std::uint32_t entityID, int TextureID, GraphicsComponent* graphComp, Transform* transformComp);

		void											RenderUISceneMeshBatch(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);
		void											RenderUISceneMeshInstance(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);
		void											SetFadeFactor(float fadeValue);
		float											GetFadeFactor();


		void											ClearBatchSubmission();
		void											SubmitMesh(std::uint32_t entityID, GraphicsComponent* graphComp, Transform* transformComp, float _dt);
		void											SubmitBatch(std::uint32_t entityID, int TextureID, Transform* transformComp, GraphicsComponent* graphComp);
		void											SubmitInstance(std::uint32_t entityID, int TextureID, Transform* transformComp, GraphicsComponent* graphComp);
		std::shared_ptr<VulkanPipeline>					GetDeferredPipeline(DEFERRED_STAGE stage);
		void											ShutDown();
		void											SetClearColour(Vec4 clearColor);
		void											UpdateClearColur();
		void											UpdateAllTextureArrays();
		void											UpdateMaterialList();
		void											UploadMaterialsList(std::string_view modelName);
		FBO* GetFrameBuffer(RENDER_PASS renderpassType);

		SceneUniform& GetSceneUniform();


	public:
		GlobalUBO globalUBO{}; //Temp for testing

	private:

		//Lighting pass data
		float											m_ScreenFadeFactor = 0.1f;
		std::uint32_t									m_LightSrcInstance = 0;

		LightingPushConstant							m_LightingPushConstant;
		LightBuffers									m_LightUBOs;

		std::array<LightSourceProperties, 1000>			m_LightSourceBuffers;
		SceneCamera										m_SceneCamera;
		SceneUniform									m_SceneUBO;

		Batch3D											m_GBufferBatch3D;
		Batch3D											m_Composition3DBatch;

		Instance3D										m_GBufferInstance;
		Instance3D										m_Composition3DInstance;


		PIPELINE_LIST									m_DeferredPipelines;
		std::unique_ptr<VulkanPipeline>					m_LightSource;
		std::array<FBO*, RENDER_TOTAL>					m_FrameBuffers;

		std::array<Mat4, MAX_BONES>						m_Bones;
		std::array<MaterialBuffer, MAX_MATERIALS>		m_MaterialList;
		/*BoneUniform									m_BonesUniform;*/

	};



}