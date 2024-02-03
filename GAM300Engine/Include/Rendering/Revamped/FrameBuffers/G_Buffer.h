#pragma once
#include "FrameBufferObject.h"
namespace TDS
{
	
	class G_Buffer : public FBO
	{
		public:
			G_Buffer(Vec2 DimensionInput);
			~G_Buffer();
			bool	Create();
			bool	Resize(std::uint32_t width, std::uint32_t height);
			void	Destroy();
			void	GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& pipelineEntry);
	
	};
}