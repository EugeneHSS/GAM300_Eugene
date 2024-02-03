/*!*************************************************************************
****
\file PointLightSystem.cpp
\author Ng Zuo Xian Amadeus
\par DP email: ng.z@digipen.edu
\par Course: CSD3400
\par Section: a
\date 22-9-2023
\brief  Function definitions of the PointLightSystem Class
****************************************************************************
***/
#include "vulkanTools/PointLightSystem.h"
#include "Shader/ShaderLoader.h"
#include "vulkanTools/GlobalBufferPool.h"

namespace TDS {
	struct PointLightPushConstants {
		Vec4	m_Position{};
		Vec4	m_Color{};
		float	m_radius;
	};

	PointLightSystem::PointLightSystem(VulkanInstance& Instance) : m_Instance(Instance) {
		/*createPipelineLayout(globalsetlayout);
		createPipeline(renderpass);*/
		m_pointlightcount = 0;
		GlobalBufferPool::GetInstance()->AddToGlobalPool(sizeof(GlobalUBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, "ubo");
		TDS_INFO("sizeof globalubo {}", sizeof(GlobalUBO));
		PipelineCreateEntry PipelineEntry;
		PipelineEntry.m_NumDescriptorSets = 1;
		PipelineEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::VERTEX, "../assets/shaders/pointlightvert.spv"));
		PipelineEntry.m_ShaderInputs.m_Shaders.insert(std::make_pair(SHADER_FLAG::FRAGMENT, "../assets/shaders/pointlightfrag.spv"));
		PipelineEntry.m_PipelineConfig.m_EnableDepthTest = true;
		PipelineEntry.m_PipelineConfig.m_EnableDepthWrite = true;
		PipelineEntry.m_PipelineConfig.m_SrcClrBlend = VK_BLEND_FACTOR_SRC_ALPHA;
		PipelineEntry.m_PipelineConfig.m_DstClrBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		PipelineEntry.m_PipelineConfig.m_SrcAlphaBlend = VK_BLEND_FACTOR_SRC_ALPHA;
		PipelineEntry.m_PipelineConfig.m_DstAlphaBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_Pipeline = std::make_unique<VulkanPipeline>();
		m_Pipeline->Create(PipelineEntry);
	}

	PointLightSystem::~PointLightSystem() {
		//vkDestroyPipelineLayout(m_Instance.getVkLogicalDevice(), m_pipelineLayout, nullptr);
	}



	void PointLightSystem::update(GlobalUBO& ubo, GraphicsComponent* Gp, Transform* Transform) {
		//if entity is not a pointlight, return
		if (!Gp->IsPointLight()) { //if not a pointlight
		if (Gp->GetPointLightID() != -1) {//check if it was a point light before
			m_vPointLightBoolMap[Gp->GetPointLightID()] = false;
			//reset value of light and color
			ubo.m_vPointLights[Gp->GetPointLightID()].m_Position = { FLT_MAX };
			ubo.m_vPointLights[Gp->GetPointLightID()].m_Color = { FLT_MIN };
			Gp->SetPointLightID(-1);//reset ID
			--m_pointlightcount;
		}
		return;
	}

		if (ubo.m_activelights >= Max_Lights) { //if number of active lights is more than allowed, no change of bool
			//assert that theres too many pointlights than allowed 
			return;
		}
		//if it reaches here, entity is a point light AND there is space in the pointlight system to add a new one

		//if it entity is a point light AND has no ID yet
		if (Gp->GetPointLightID() == -1) {
			//loop through the available array for free light id
			for (int i{ 0 }; i < m_vPointLightBoolMap.size(); ++i) {
				if (!m_vPointLightBoolMap[i]) {//check if id has been taken
					m_vPointLightBoolMap[i] = true;//ID taken
					Gp->SetPointLightID(i);//set ID
					++m_pointlightcount;
					break;
				}
			}
		}

		//std::cout << Gp->GetPointLightID() << std::endl;
		ubo.m_vPointLights[Gp->GetPointLightID()].m_Position = Transform->GetPosition();//update ubo with with light pos
		ubo.m_vPointLights[Gp->GetPointLightID()].m_Color = Gp->GetColor();//white light with intensity at w
		ubo.m_activelights = m_pointlightcount;
	}

	void PointLightSystem::render(GraphicsComponent* Gp, Transform* Trans) {
		m_Pipeline->BindPipeline();
		//ShaderMetaData reflectedMeta = ShaderLoader::GetInstance()->getReflectedLookUp();
		PointLightPushConstants pushdata;
		pushdata.m_Position = Trans->GetPosition();
		pushdata.m_Color = Gp->GetColor();
		pushdata.m_radius = Trans->GetScale().x;
		m_Pipeline->SubmitPushConstant(&pushdata, sizeof(PointLightPushConstants), SHADER_FLAG::VERTEX | SHADER_FLAG::FRAGMENT);

		vkCmdDraw(m_Pipeline->GetCommandBuffer(), 6, 1, 0, 0);
	}
	VulkanPipeline& PointLightSystem::GetPipeline() 
	{
		return *m_Pipeline;
	}
	void PointLightSystem::newupdate(GlobalUBO& UBO, const std::vector<EntityID>& Entities, Transform* xform, GraphicsComponent* Gp) {
		m_pointlightcount = 0;//reset and recount the number of lights
		for (int i{ 0 }; i < Entities.size(); ++i) {
			if (!Gp[i].IsPointLight()) {
				if (Gp[i].GetPointLightID() != -1) {//check if it was previously a pointlight
					UBO.m_vPointLights[Gp[i].GetPointLightID()].m_Position = { FLT_MAX };
					UBO.m_vPointLights[Gp[i].GetPointLightID()].m_Color = { FLT_MIN };
					Gp[i].SetPointLightID(-1);//reset pointlightID to default
				}
				continue;//skip if not pointlight
			}
			/*if (m_pointlightcount >= Max_Lights) {
				return;
			}*/
			Gp->SetPointLightID(i);//set its id to whatever is the entityid
			UBO.m_vPointLights[m_pointlightcount].m_Position = xform[i].GetPosition();//
			UBO.m_vPointLights[m_pointlightcount].m_Color = Gp[i].GetColor();
			//std::cout << "alpha" << Gp[i].GetColor().w << std::endl;
			UBO.m_activelights = ++m_pointlightcount;
			//std::cout << m_pointlightcount << std::endl;
		}
	}
}