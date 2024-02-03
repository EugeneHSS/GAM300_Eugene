#ifndef DEBUG_RENDERER
#define DEBUG_RENDERER
#include "vulkanTools/vulkanInstance.h"
#include "dotnet/ImportExport.h"
#include "components/components.h"
#include "vulkanTools/VulkanPipeline.h"
namespace TDS
{
	static constexpr int MAX_BUFFER_SIZE = 10000;


	enum DebugModel
	{
		DEBUG_RENDER_CUBE = 0,
		DEBUG_RENDER_SPHERE,
		DEBUG_RENDER_CAPSULE,
		DEBUG_RENDER_MAX_DEBUG
	};

	struct alignas(16) DebugInstanceBuffer
	{
		Mat4	m_WorldTransform;
		Vec4	m_Colour;
	};


	struct DebugInstanceInfo
	{
		Transform* m_Transform;
		Vec4			m_Colour;
	};

	struct DebugInputBuffers
	{
		std::shared_ptr<VMABuffer>			m_VertexBuffer = nullptr;
		std::shared_ptr<VMABuffer>			m_IndexBuffer = nullptr;
	};


	struct DebugDrawCommand
	{
		std::uint32_t						m_Offset = 0;
		std::uint32_t						m_Instance = 0;
		DebugInputBuffers* m_pBuffer;
	};

	struct GroupInstance
	{
		std::vector<DebugInstanceInfo>		m_DebugInstanceInfo;
		std::uint32_t						m_Index = 0;
	};


	struct DebugInstance
	{
		std::map<DebugModel, GroupInstance>							m_Instances;
		std::array<DebugDrawCommand, MAX_BUFFER_SIZE>				m_DebugCommand;
		std::array<DebugInstanceBuffer, MAX_BUFFER_SIZE>			m_DebugInstanceBuffer;
		std::array<DebugInputBuffers, DEBUG_RENDER_MAX_DEBUG>		m_DebugDrawing;
		void														SubmitToDebugGroup(Transform* transform, EntityID id);
		void														BuildAllGroups();
		std::uint32_t												m_InstanceCnt = 0;
		std::uint32_t												m_TotalGroup = 0;
	};


	struct SceneInfo
	{
		Mat4 Proj;
		Mat4 View;
	};

	class DebugRenderer
	{
	public:

		DLL_API				DebugRenderer(VulkanInstance& Instance);
		DLL_API				~DebugRenderer();
		DLL_API		void	DestroyPipeline();
		DebugRenderer(const DebugRenderer&) = delete;
		DebugRenderer& operator=(const DebugRenderer&) = delete;
		DLL_API		void	Render(VkCommandBuffer commandBuffer, std::uint32_t frameIndex);
		DLL_API VulkanPipeline& GetPipeline();
		DLL_API		bool	Init();
		DLL_API		void	ToggleDebug(bool condition);
		DLL_API		bool	GetDebugToggle() const;
		DLL_API		void	BuildUpdates();
		inline		DebugInstance& GetDebugInstance() { return m_DebugInstance; }

	public:
		SceneInfo			m_SceneInfo{};
	private:
		TypeReference<MeshController>							m_CubePrimitive;
		TypeReference<MeshController>							m_SpherePrimitive;
		TypeReference<MeshController>							m_CapsulePrimitive;
		VulkanInstance& m_Instance;
		DebugInstance											m_DebugInstance;
		std::unique_ptr<VulkanPipeline>							m_Pipeline;
		bool													m_DebugToggle = true;

	};
}
#endif // !DEBUG_RENDERER