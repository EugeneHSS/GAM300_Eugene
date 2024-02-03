#pragma once
#include "FrameBufferObject.h"
namespace TDS
{
	class DebugView : public FBO
	{
	public:
		DebugView(Vec2 DimensionInput);
		~DebugView();

		bool	Create();
		bool	Resize(std::uint32_t width, std::uint32_t height);
		void	Destroy();

		void	GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& pipelineEntry);
	};


}