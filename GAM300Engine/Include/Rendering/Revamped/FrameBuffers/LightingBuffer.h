#pragma once
#include "FrameBufferObject.h"
namespace TDS
{
	

	class LightingBuffer : public FBO
	{
		public:
			LightingBuffer(Vec2 DimensionInput);
			~LightingBuffer();

			bool	Create();
			bool	Resize(std::uint32_t width, std::uint32_t height);
			void	Destroy();

			void	GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& pipelineEntry);
	};
}