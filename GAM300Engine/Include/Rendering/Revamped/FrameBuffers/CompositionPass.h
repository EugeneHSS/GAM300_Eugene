#pragma once
#include "FrameBufferObject.h"

namespace TDS
{
	class CompositionPass : public FBO
	{
	public:
		CompositionPass(Vec2 DimensionInput);
		~CompositionPass();
		bool	Create();
		bool	Resize(std::uint32_t width, std::uint32_t height);
		void	Destroy();

		void	GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& pipelineEntry);
	};
}