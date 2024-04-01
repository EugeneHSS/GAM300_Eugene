#include "imguiHelper/ImguiCompilerDescriptor.h"
#include "Tools/CompilerFormats.h"
namespace TDS
{
	CompilerDescriptors::CompilerDescriptors()
	{
		flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiDockNodeFlags_AutoHideTabBar;
		panelTitle = "Compiler Descriptors";
		windowPadding = ImVec2(0.f, 0.f);


		m_CompilerDescriptors[DESC_GEOMETRY] = new GeomDescDisplay();
		m_CompilerDescriptors[DESC_TEXTURE] = new TextureDescDisplay();

	}
	CompilerDescriptors::~CompilerDescriptors()
	{
		for (auto& desc : m_CompilerDescriptors)
		{
			delete desc;
			desc = nullptr;
		}
	}
	void CompilerDescriptors::init()
	{
	}
	void CompilerDescriptors::update()
	{
		static DESCRIPTOR_TYPE m_CurrDescType = DESC_GEOMETRY;

		if (ImGui::BeginCombo("Descriptor Type", m_CurrDescType == DESC_GEOMETRY ? "Geometry" : "Texture"))
		{
			if (ImGui::Selectable("Geometry", m_CurrDescType == DESC_GEOMETRY))
				m_CurrDescType = DESC_GEOMETRY;
			if (ImGui::Selectable("Texture", m_CurrDescType == DESC_TEXTURE))
				m_CurrDescType = DESC_TEXTURE;

			ImGui::EndCombo();
		}
		DisplaySettings(m_CurrDescType);
	}
	void CompilerDescriptors::DisplaySettings(DESCRIPTOR_TYPE descType)
	{
		
		if (descType == DESC_GEOMETRY)
		{
			if (ImGui::TreeNode("Geometry Descriptor"))
			{
				auto geomDesc = static_cast<GeomDescDisplay*>(m_CompilerDescriptors[descType]);
				ImGui::InputFloat3("Scale", geomDesc->m_GeomDecriptor.m_Descriptor.m_L2W.m_Scale);
				ImGui::InputFloat3("Rotate", geomDesc->m_GeomDecriptor.m_Descriptor.m_L2W.m_Rotate);
				ImGui::InputFloat3("Translate", geomDesc->m_GeomDecriptor.m_Descriptor.m_L2W.m_Translate);
				ImGui::InputFloat2("Normalization", geomDesc->m_GeomDecriptor.m_NDC.m_MinMax);

				ImGui::Checkbox("PreTransform Vertices", &geomDesc->m_GeomDecriptor.m_PretransformedVertices);
				ImGui::Checkbox("Load Optimized Normals", &geomDesc->m_GeomDecriptor.m_OptimizeNormals);
				ImGui::Checkbox("Merge identical Vertices", &geomDesc->m_GeomDecriptor.m_MergeIdenticalVertices);
				ImGui::Checkbox("Compress Mesh", &geomDesc->m_GeomDecriptor.m_Compress);
				ImGui::Checkbox("Load Mesh", &geomDesc->m_GeomDecriptor.m_LoadMesh);
				ImGui::Checkbox("Load Bone Animations", &geomDesc->m_GeomDecriptor.m_LoadAnimation);
				ImGui::Checkbox("Load Model Animations(For doors, closets, etc...)", &geomDesc->m_GeomDecriptor.m_LoadModelAnimation);
				ImGui::Checkbox("Load Materials", &geomDesc->m_GeomDecriptor.m_LoadMaterials);
				ImGui::Checkbox("Load Materials Textures (THIS WILL TAKE EXTREMELY LONG)", &geomDesc->m_GeomDecriptor.m_LoadMaterialTextures);
				ImGui::Checkbox("Merge Mesh", &geomDesc->m_GeomDecriptor.m_MergeMesh);
				ImGui::Checkbox("Normalized Vertices", &geomDesc->m_GeomDecriptor.m_Normalized);
				ImGui::Checkbox("Create LOD", &geomDesc->m_GeomDecriptor.m_LodOptions.m_CreateLOD);
				ImGui::Checkbox("Remove Child Meshes", &geomDesc->m_GeomDecriptor.m_RemoveChildMeshes);
				ImGui::Checkbox("Use Streams(IM ONLY USING THIS FOR THE GARDEN)", &geomDesc->m_GeomDecriptor.m_UseStreams);
				ImGui::SliderFloat("Reduction Factor", &geomDesc->m_GeomDecriptor.m_LodOptions.ReductionFactor, 0.0f, 1.0f);

				ImGui::TreePop();
			}
			
		}
		else if (descType == DESC_TEXTURE)
		{
			if (ImGui::TreeNode("Texture Descriptor"))
			{
				auto texDesc = static_cast<TextureDescDisplay*>(m_CompilerDescriptors[descType]);
			
				static int formatIndex = 0;

				if (ImGui::Combo("Format", &formatIndex, TDS::strTexFormats, IM_ARRAYSIZE(TDS::strTexFormats)))
					texDesc->m_TextureDescriptor.m_Desc.m_FormatString = TDS::strTexFormats[formatIndex];
				
				static int compilerIndex = 0;
				if (ImGui::Combo("Compiler Type", &compilerIndex, TDS::CpuComputeFormats, IM_ARRAYSIZE(TDS::CpuComputeFormats)))
					texDesc->m_TextureDescriptor.m_Desc.m_EncoderString = TDS::CpuComputeFormats[compilerIndex];

				ImGui::Text("Selected Format: %s", TDS::strTexFormats[formatIndex]);
				ImGui::Text("Selected Compiler: %s", TDS::CpuComputeFormats[compilerIndex]);

				ImGui::InputInt("Number of Threads", &texDesc->m_TextureDescriptor.m_Desc.m_NumberOfThreads);
				ImGui::InputInt("Width", &texDesc->m_TextureDescriptor.m_Desc.Width);
				ImGui::InputInt("Height", &texDesc->m_TextureDescriptor.m_Desc.height);
				ImGui::SliderFloat("Quality", &texDesc->m_TextureDescriptor.m_Desc.m_FloatQuality, 0.0f, 1.0f);


				
				ImGui::TreePop();
			}
		}



	}
}