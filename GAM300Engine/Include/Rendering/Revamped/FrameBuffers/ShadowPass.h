#pragma once
#include "FrameBufferObject.h"
namespace TDS
{
	class ShadowPass : public FBO
	{
	private:
		RenderTargetCI						m_ShadowDepth;
	public:
		ShadowPass(Vec2 DimensionInput);

		bool	Create();
		bool	Resize(std::uint32_t width, std::uint32_t height);
		void	Destroy();


		void	GetBlendAttachments(BlendContainer& blendCont, PipelineCreateEntry& createEntry);

	};
}