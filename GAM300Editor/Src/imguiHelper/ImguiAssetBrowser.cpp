#include "imguiHelper/ImguiAssetBrowser.h"
#include "Tools/FontLoader.h"
#include "Tools/FileExtensions.h"
#include "Tools/TextureCompressor.h"
#include "Tools/DDSConverter.h"
#include "Tools/GeomCompiler.h"
#include "imguiHelper/ImguiHierarchy.h"
#include "AssetManagement/AssetManager.h"
#include "vulkanTools/vulkanInstance.h"
#include "vulkanTools/VulkanTexture.h"
#include "imguiHelper/ImguiAudio.h"
#include "Tools/CompilerRevamped/MeshLoader.h"
#include <string>
#include "Rendering/GraphicsManager.h"
#include "Rendering/Revamped/MaterialManager.h"
#include "AssetManagement/Revamped/MeshFactory.h"
#include "imguiHelper/ImguiCompilerDescriptor.h"

#define ASSET_PATH "../assets"
namespace TDS
{
	bool lookUp = false;

	//for icon loading
	Texture fileIcon{}, folderIcon{};
	VkDescriptorSet file_DescSet{}, folder_DescSet{};
	//int file_image_count = 0, folder_image_count = 0;

	void CreateNewEntityWithSubMesh(EntityID ParentEntity, aiMatrix4x4& aiTransform, TypeReference<AssetModel>& model, std::string_view MeshName)
	{
		std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);
		Entity newEntity;
		EntityID newEntityID = newEntity.getID();

		addComponentByName("Name Tag", newEntityID);
		addComponentByName("Graphics Component", newEntityID);
		addComponentByName("Transform", newEntityID);

		IComponent* nameTag = getComponentByName("Name Tag", newEntityID);
		IComponent* GraphicComponent = getComponentByName("Graphics Component", newEntityID);
		IComponent* TransformComp = getComponentByName("Transform", newEntityID);

		GraphicsComponent* graphComp = reinterpret_cast<GraphicsComponent*>(GraphicComponent);
		graphComp->m_ModelName = model.m_AssetName;
		graphComp->m_MeshName = MeshName;
		graphComp->m_AssetReference = model;
		NameTag* NameTagComp = reinterpret_cast<NameTag*>(nameTag);
		Transform* transformComp = reinterpret_cast<Transform*>(TransformComp);

		NameTagComp->SetHierarchyParent(ParentEntity);
		NameTagComp->SetHierarchyIndex(panel->hierarchyList.size());
		NameTagComp->SetName(MeshName.data());

		aiVector3D scale{};
		aiVector3D Translate{};
		ExtractScale(aiTransform, scale);
		aiVector3D EulerRotate = ExtractEulerAngles(aiTransform);
		ExtractTranslation(aiTransform, Translate);
		Vec3 Translation = Vec3(Translate.x, Translate.y, Translate.z);
		Vec3 Rotatation = Vec3(EulerRotate.x, EulerRotate.y, EulerRotate.z);
		Vec3 Scaling = Vec3(scale.x, scale.y, scale.z);
		transformComp->SetPosition(Translation);
		transformComp->SetRotation(Rotatation);
		transformComp->SetScale(Scaling);

		if (ParentEntity != 0)
		{
			NameTag* parentNameTag = ecs.getComponent<NameTag>(ParentEntity);
			if (parentNameTag)
				parentNameTag->GetHierarchyChildren().push_back(newEntityID);

		}

	}


	void BuildEntityMeshHierachy(std::string_view ModelAssetName, EntityID rootEntity)
	{

		//Get Panel
		std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);

		//Get mesh reference
		TypeReference<MeshController> refMeshController{};
		auto meshcontroller = AssetManager::GetInstance()->GetMeshFactory().GetMeshController(ModelAssetName, refMeshController);
		auto& modelHandle = meshcontroller->GetModelPack()->m_ModelHandle;

		if (modelHandle.m_Mesh.size() == 1)
		{
			GraphicsComponent* graphComponent = reinterpret_cast<GraphicsComponent*>(getComponentByName("Graphics Component", rootEntity));
			graphComponent->m_MeshName = modelHandle.m_Mesh[0].m_Name;
			graphComponent->m_MeshNodeName = ModelAssetName;

		}
		else
		{
			//Get mainRoot tag
			NameTag* mainRoot = reinterpret_cast<NameTag*>(getComponentByName("Name Tag", rootEntity));
			mainRoot->SetName(ModelAssetName.data());
			GraphicsComponent* graphComponent = reinterpret_cast<GraphicsComponent*>(getComponentByName("Graphics Component", rootEntity));
			graphComponent->m_MeshName = ModelAssetName;
			graphComponent->m_MeshNodeName = ModelAssetName;


			for (auto& rootNodes : meshcontroller->GetRoots())
			{
				Entity nodeEntity;
				EntityID nodeEntityID = nodeEntity.getID();

				//Add components
				{
					addComponentByName("Name Tag", nodeEntityID);
					addComponentByName("Graphics Component", nodeEntityID);
					addComponentByName("Transform", nodeEntityID);
				}

				GraphicsComponent* graphComp = reinterpret_cast<GraphicsComponent*>(getComponentByName("Graphics Component", nodeEntityID));
				NameTag* rootTag = reinterpret_cast<NameTag*>(getComponentByName("Name Tag", nodeEntityID));
				Transform* transformComp = reinterpret_cast<Transform*>(getComponentByName("Transform", nodeEntityID));
				graphComp->m_MeshNodeName = rootNodes.first;
				{
					//Get asset reference,
					AssetManager::GetInstance()->GetMeshFactory().GetMeshController(ModelAssetName, graphComp->m_MeshControllerRef);
					
					//This is a root, so no rendered mesh
					graphComp->m_MeshName = "";
					graphComp->m_ModelName = ModelAssetName;
					graphComp->m_MeshNodeName = rootNodes.first;

					//set root node name, 
					rootTag->SetName(rootNodes.first);
					rootTag->SetHierarchyParent(rootEntity);
					rootTag->SetHierarchyIndex(panel->hierarchyList.size());
					if (rootEntity != 0)
					{
						if (rootTag)
							mainRoot->GetHierarchyChildren().push_back(nodeEntityID);
					}
					//set initial transform position,

					if (ModelAssetName == "Adjusted_Bin.bin" || ModelAssetName == "Outside Environment_Bin.bin")
					{
						transformComp->SetPosition(rootNodes.second.m_SceneTranslation);
						transformComp->SetRotation(rootNodes.second.m_SceneRotation);
						transformComp->SetScale(rootNodes.second.m_SceneScale);
					}
					else
					{
						transformComp->SetRealPosition(rootNodes.second.m_SceneTranslation);
						//transformComp->SetRealScale(rootNodes.second.m_SceneScale);
						//transformComp->SetRealRotate(rootNodes.second.m_SceneRotation);
					}
					// push this entity into the main root

				}


				// Iterate the meshlist for this root node
				for (auto& meshNode : rootNodes.second.m_MeshList)
				{
					Entity newEntity;
					EntityID newEntityID = newEntity.getID();


					{
						//Add components
						addComponentByName("Name Tag", newEntityID);
						addComponentByName("Graphics Component", newEntityID);
						addComponentByName("Transform", newEntityID);

						//Get components
						GraphicsComponent* childGrapComp = reinterpret_cast<GraphicsComponent*>(getComponentByName("Graphics Component", newEntityID));
						NameTag* childTag = reinterpret_cast<NameTag*>(getComponentByName("Name Tag", newEntityID));
						Transform* childTransformComp = reinterpret_cast<Transform*>(getComponentByName("Transform", newEntityID));


						childTag->SetName(meshNode.first);
						childTag->SetHierarchyParent(nodeEntityID);
						childTag->SetHierarchyIndex(panel->hierarchyList.size());

						//Get Reference
						AssetManager::GetInstance()->GetMeshFactory().GetMeshController(ModelAssetName, childGrapComp->m_MeshControllerRef);
			
						//Set the correct mesh name and model name
						childGrapComp->m_MeshName = meshNode.first;
						childGrapComp->m_ModelName = ModelAssetName;
						childGrapComp->m_MeshNodeName = rootNodes.first;
						//Set initial transform position
						if (ModelAssetName == "Adjusted_Bin.bin" || ModelAssetName == "Outside Environment_Bin.bin")
						{
							childTransformComp->SetPosition(rootNodes.second.m_SceneTranslation);
							childTransformComp->SetRotation(rootNodes.second.m_SceneRotation);
							childTransformComp->SetScale(rootNodes.second.m_SceneScale);
						}
						else
						{
							childTransformComp->SetRealPosition(rootNodes.second.m_SceneTranslation);

							//transformComp->SetRealScale(rootNodes.second.m_SceneScale);
							//transformComp->SetRealRotate(rootNodes.second.m_SceneRotation);
						}
						/*		childTransformComp->SetRealScale(rootNodes.second.m_SceneScale);
								childTransformComp->SetRealRotate(rootNodes.second.m_SceneRotation);*/
								//childTransformComp->SetScale(Vec3(1.f, 1.f, 1.f));
								//childTransformComp->SetRotation(Vec3(0.f, 0.f, 0.f));
								//If entity validity check
						if (nodeEntityID != 0)
						{
							//Push into the root parent
							NameTag* parentNameTag = ecs.getComponent<NameTag>(nodeEntityID);
							if (parentNameTag)
								parentNameTag->GetHierarchyChildren().push_back(newEntityID);
						}

					}
				}


			}
		}


	}
	void DivideSubmesh(EntityID entity, TypeReference<AssetModel>& model)
	{
		for (auto& childMeshes : model.m_ResourcePtr->m_Meshes)
		{
			aiMatrix4x4& originalModelTransform = GeomCompiler::GetInstance()->m_CurrTransforms[childMeshes.first];

			CreateNewEntityWithSubMesh(entity, originalModelTransform, model, childMeshes.first);
		}

	}


	AudioImgui audimg; //This causes COM library error, not sure why

	AssetBrowser::AssetBrowser()
	{
		//selected = 0;
		//selectedFolder = -1;
		//renameCheck = false;

		flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
		panelTitle = "Asset Browser";
		windowPadding = ImVec2(0.f, 0.f);
		m_curr_path = std::filesystem::path(ASSET_PATH);

		//insertEntities();
	}

	void AssetBrowser::getFileNameFromPath(const char* full_path, std::string* path, std::string* file, std::string* fileWithExtension, std::string* extension) const
	{
		if (full_path != nullptr)
		{
			std::string nwFullPath = full_path;
			std::string& full_path_ = nwFullPath;
			//bool toLower;
			for (std::string::iterator charIterator = full_path_.begin(); charIterator != full_path_.end(); ++charIterator)
			{
				if (*charIterator == '\\')
					*charIterator = '/';
				/*else
					if (toLower) {
						*charIterator = tolower(*charIterator);
					}*/
			}
			size_t posSlash = nwFullPath.find_last_of("\\/");
			size_t posDot = nwFullPath.find_last_of(".");

			if (path != nullptr)
			{
				if (posSlash < nwFullPath.length())
					*path = nwFullPath.substr(0, posSlash + 1);
				else
					path->clear();
			}
			if (fileWithExtension != nullptr) {
				if (posSlash < nwFullPath.length()) {
					*fileWithExtension = nwFullPath.substr(posSlash + 1);
				}
				else
					*fileWithExtension = nwFullPath;
			}

			if (file != nullptr)
			{
				nwFullPath = nwFullPath.substr(0, posDot);
				posSlash = nwFullPath.find_last_of("\\/");
				*file = nwFullPath.substr(posSlash + 1);

			}

			if (extension != nullptr)
			{
				if (posDot < nwFullPath.length())
					*extension = nwFullPath.substr(posDot);
				else
					extension->clear();
			}
		}
	}

	static const std::filesystem::path s_AssetDirectory = "../assets";
	static const std::filesystem::path s_ModelDirectory = "../assets/models";
	static const std::filesystem::path s_TextureDirectory = "../assets/textures";
	static const std::filesystem::path s_FontDirectory = "../assets/Fonts";
	void AssetBrowser::update()
	{
		if (m_curr_path != std::filesystem::path(s_AssetDirectory))
		{
			if (ImGui::Button("<-")) //will only show if u went into a folder in the current directory above
			{
				m_curr_path = m_curr_path.parent_path();

				/*if (audimg.ToggleControls())
				{
					std::cout << "Stopping Audio Imgui controls" << '\n';
				}*/
			}
		}
		if (m_curr_path == std::filesystem::path(s_ModelDirectory))
		{
			ImGui::SameLine();
			ImGui::Checkbox("Show .bin files", &show_bin);
			ImGui::SameLine();
			ImGui::Checkbox("Show .fbx files", &show_fbx);
			ImGui::SameLine();
			ImGui::Checkbox("Show .gltf files", &show_gltf);
		}
		if (m_curr_path == std::filesystem::path(s_TextureDirectory))
		{
			ImGui::SameLine();
			ImGui::Checkbox("Show .dds files", &show_dds);
			ImGui::SameLine();
			ImGui::Checkbox("Show .png files", &show_png);
		}
		float cellSize = thumbnail_size + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		ImGui::Columns(std::max(columnCount, 1), 0, false);

		//load texture info ONCE

		if (loadonce == true)
		{
			fileIcon.LoadTexture("../assets/icons/icon.dds"); //can only take dds files for now
			file_DescSet = ImGui_ImplVulkan_AddTexture(fileIcon.m_VulkanTexture->getInfo().sampler, fileIcon.m_VulkanTexture->getInfo().imageView, fileIcon.m_VulkanTexture->getInfo().imageLayout);

			folderIcon.LoadTexture("../assets/icons/folder.dds"); //can only take dds files for now
			folder_DescSet = ImGui_ImplVulkan_AddTexture(folderIcon.m_VulkanTexture->getInfo().sampler, folderIcon.m_VulkanTexture->getInfo().imageView, folderIcon.m_VulkanTexture->getInfo().imageLayout);

			loadonce = false;
		}



		for (auto& directory_entry : std::filesystem::directory_iterator(m_curr_path))
		{
			path1 = directory_entry.path().string();
			auto relative_path = std::filesystem::relative(directory_entry.path(), s_AssetDirectory);

			//ImGui::Button(path1.c_str(), { thumbnail_size, thumbnail_size });
			//shorten the path name

			std::string filename;
			getFileNameFromPath(path1.c_str(), nullptr, nullptr, &filename, nullptr);
			if (m_curr_path == std::filesystem::path(s_ModelDirectory))
			{
				if (directory_entry.is_directory()) //draw folder icon
				{
					ImGui::ImageButton(reinterpret_cast<void*>(folder_DescSet), ImVec2{ thumbnail_size, thumbnail_size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
					//folder_image_count++;
				}
				//if (!show_bin && !show_fbx) { continue; }
				/*else*/ if (show_bin && !show_fbx && !show_gltf)
				{
					if (!strstr(filename.c_str(), ".bin")) //if its not bin, continue
					{
						continue;
					}
				}
				else if (show_fbx && !show_bin && !show_gltf)
				{
					if (!strstr(filename.c_str(), ".fbx")) //if its not bin, continue
					{
						continue;
					}
				}
				else if (show_gltf && !show_bin && !show_fbx)
				{
					if (!strstr(filename.c_str(), ".gltf")) //if its not bin, continue
					{
						continue;
					}
				}

			}
			if (m_curr_path == std::filesystem::path(s_TextureDirectory))
			{
				if (directory_entry.is_directory()) //draw folder icon
				{
					ImGui::ImageButton(reinterpret_cast<void*>(folder_DescSet), ImVec2{ thumbnail_size, thumbnail_size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
					//folder_image_count++;
				}
				//if (!show_dds && !show_png) { continue; }
				if (show_dds && !show_png)
				{
					if (!strstr(filename.c_str(), ".dds"))
					{
						continue;
					}
				}
				else if (show_png && !show_dds)
				{
					if (!strstr(filename.c_str(), ".png"))
					{
						continue;
					}
				}

			}
			ImGui::PushID(filename.c_str()); //store the current id in each button that is created in each iteration of files

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));


			if (directory_entry.is_directory()) //draw folder icon
			{
				ImGui::ImageButton(reinterpret_cast<void*>(folder_DescSet), ImVec2{ thumbnail_size, thumbnail_size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				//folder_image_count++;
			}
			else //draw file icon
			{

				//then render button
				ImGui::ImageButton(reinterpret_cast<void*>(file_DescSet), ImVec2{ thumbnail_size, thumbnail_size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				//do drag drop ONLY on files, not folder
				if (ImGui::BeginDragDropSource())
				{
					std::filesystem::path relativePath(path1);
					const wchar_t* itemPath = relativePath.c_str();
					ImGui::Text(filename.c_str()); //need to get info from image
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}
				//file_image_count++;

				//ImGui::Button(filename.c_str(), { thumbnail_size, thumbnail_size });

			}
			ImGui::PopStyleColor();

			if (ImGui::IsItemClicked(0))
			{

				//if it has a "." in it, it is a file, do not add to path
				//only directories/folders can be added to path

				//if it is a folder, open it and update the asset broswer curr dir
				if (!strstr(filename.c_str(), "."))
				{
					m_curr_path += "/" + filename;


					//attempt at drag drop
					selectedpath = path1;


				}
				//use the rest of the checks below to handle what happens when u press different kinds of file extensions

				//if .jpg/.PNG, load 2d texture...
				//if (strstr(filename.c_str(), ".jpg") || strstr(filename.c_str(), ".png") || strstr(filename.c_str(), ".dds"))
				//{
				//	lookUp = false;



				//	std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);

				//	EntityID currEntity = panel->getSelectedEntity();
				//	IComponent* Graph = getComponentByName("Graphics Component", panel->getSelectedEntity());
				//	if (Graph == nullptr)
				//		Graph = addComponentByName("Graphics Component", panel->getSelectedEntity());
				//	

				//	std::string OutPath = ASSET_PATH;
				//	OutPath += "/textures/";
				//	OutPath += filename.c_str();
				//	std::string inPath = OutPath;
				//	if (strstr(filename.c_str(), ".jpg"))
				//		OutPath = RemoveFileExtension(OutPath, ".jpg");
				//	else if (strstr(filename.c_str(), ".png"))
				//		OutPath = RemoveFileExtension(OutPath, ".png");


				//	if (strstr(filename.c_str(), ".dds"))
				//		lookUp = true;
				//	else
				//		OutPath += ".dds";


				//	if (lookUp == false)
				//		TextureCompressor::GetInstance().Run(inPath, OutPath);

				//	GraphicsComponent* graphComp = reinterpret_cast<GraphicsComponent*>(Graph);
				//	if (graphComp)
				//		AssetManager::GetInstance()->GetTextureFactory().Load(OutPath, graphComp->GetTexture());


				//}
				//if (strstr(filename.c_str(), ".obj") || strstr(filename.c_str(), ".fbx") || strstr(filename.c_str(), ".gltf") || strstr(filename.c_str(), ".bin"))
				//{
				//	lookUp = false;
				//	std::string& OutPath = GeomCompiler::GetInstance()->OutPath;
				//	OutPath = "../assets/models/";
				//	OutPath += filename.c_str();
				//	if (strstr(filename.c_str(), ".fbx"))
				//		OutPath = RemoveFileExtension(OutPath, ".fbx");
				//	else if (strstr(filename.c_str(), ".gltf"))
				//		OutPath = RemoveFileExtension(OutPath, ".gltf");
				//	else if (strstr(filename.c_str(), ".obj"))
				//		OutPath = RemoveFileExtension(OutPath, ".obj");
				//	else if (strstr(filename.c_str(), ".bin"))
				//	{
				//		lookUp = true;
				//		/*OutPath = std::filesystem::path(OutPath).filename().string();*/
				//	}

				//	std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);

				//	EntityID currEntity = panel->getSelectedEntity();
				//	if (currEntity < 1)
				//		return;
				//	IComponent* Graph = getComponentByName("Graphics Component", panel->getSelectedEntity());
				//	if (Graph == nullptr)
				//		Graph = addComponentByName("Graphics Component", panel->getSelectedEntity());
				//	GraphicsComponent* graphComp = reinterpret_cast<GraphicsComponent*>(Graph);

				//	if (lookUp == false)
				//	{
				//		GeomDescriptor m_GeomDescriptor{};
				//		m_GeomDescriptor.m_Descriptor.m_FilePath = std::filesystem::path(filename).filename().string();
				//		GeomCompiler::GetInstance()->InitDesc(m_GeomDescriptor);
				//		std::string OutputFile = GeomCompiler::GetInstance()->LoadModel();

				//		AssetManager::GetInstance()->GetModelFactory().LoadModel(OutputFile, graphComp->GetAsset());

				//	}
				//	else
				//	{
				//		AssetManager::GetInstance()->GetModelFactory().LoadModel(OutPath, graphComp->GetAsset());

				//	}
				//	if (graphComp->m_AssetReference.m_ResourcePtr->m_Meshes.size() > 1)
				//		DivideSubmesh(currEntity, graphComp->m_AssetReference);
				//	
				//	


				//}
				////if .json, load scene...
				//if (strstr(filename.c_str(), ".ttf"))
				//{
				//	std::string selectedTextureDir = m_curr_path.string();
				//	selectedTextureDir += "/";
				//	selectedTextureDir += filename;
				//	std::string inputPath = selectedTextureDir;
				//	std::string outPath = m_curr_path.string();
				//	selectedTextureDir = RemoveFileExtension(selectedTextureDir, ".ttf");

				//	std::string FileName = std::filesystem::path(selectedTextureDir).filename().string();

				//	outPath += ("/OutFont_" + FileName);
				//	if (!std::filesystem::exists(outPath))
				//	{
				//		if (!std::filesystem::create_directory(outPath))
				//			TDS_ERROR("Failed to create directory");
				//	}
				//	std::string folderName = outPath;
				//	outPath += "/" + FileName;
				//	LoaderDescriptor FontDescriptor{};
				//	FontDescriptor.OutPath = outPath;
				//	FontDescriptor.m_InputPath = inputPath;
				//	std::string ddsPath{};
				//	if (FontLoader::RunFontLoader(FontDescriptor, ddsPath))
				//	{
				//		std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);

				//		IComponent* font = getComponentByName("UI Sprite", panel->getSelectedEntity());
				//		if (font != nullptr)
				//		{
				//			UISprite* spriteComp = reinterpret_cast<UISprite*>(font);
				//			AssetManager::GetInstance()->GetFontFactory().Load(folderName, spriteComp->GetFontReference());
				//		}
				//	}
				//	

				//}



				//if (strstr(filename.c_str(), ".json"))
				//{

				//}

				////if .wav, play audio...
				//if (strstr(filename.c_str(), ".wav") || strstr(filename.c_str(), ".flac") || strstr(filename.c_str(), ".mp3"))
				//{
				//	/*audimg.ToggleControls(true);

				//	audimg.play(filename);*/
				//}


			}
			ImGui::TextWrapped(filename.c_str()); //currently using this to display full file name

			/*if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern))
			{
				ImGui::SetDragDropPayload("path", path1.c_str(), 1);
				ImGui::Text(path1.c_str());
				if (!ImGui::IsMouseDown(0))
				{
					ImGui::EndDragDropSource();

				}
			}

			if (ImGui::BeginDragDropTarget())
			{
				ImGui::EndDragDropTarget();
			}*/
			ImGui::NextColumn();
			ImGui::PopID(); //store new id for next button
		}


		ImGui::Columns(1);
		//ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);
	}

	std::string AssetBrowser::LoadAssetRevamped(const std::string& FileName)
	{

		std::shared_ptr<LevelEditorPanel> panel = LevelEditorManager::GetInstance()->panels[PanelTypes::COMPILER_DESCRIPTOR];
		std::shared_ptr<CompilerDescriptors> imguiDesc = std::static_pointer_cast<CompilerDescriptors>(panel);
		std::string OutputPath{};
		std::string OutName{};
		if (strstr(FileName.c_str(), ".jpg") || strstr(FileName.c_str(), ".png") || strstr(FileName.c_str(), ".dds"))
		{
			lookUp = false;
			std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);

			EntityID currEntity = panel->getSelectedEntity();
			//IComponent* Graph = getComponentByName("Graphics Component", panel->getSelectedEntity());
			//if (Graph == nullptr)
			//	return std::string();


			std::string OutPath = ASSET_PATH;
			OutPath += "/textures/";
			OutPath += FileName.c_str();
			std::string inPath = OutPath;
			if (strstr(FileName.c_str(), ".jpg"))
				OutPath = RemoveFileExtension(OutPath, ".jpg");
			else if (strstr(FileName.c_str(), ".png"))
				OutPath = RemoveFileExtension(OutPath, ".png");


			if (strstr(FileName.c_str(), ".dds"))
				lookUp = true;
			else
				OutPath += ".dds";

			CompilerDescriptors::TextureDescDisplay* textureDisplay = reinterpret_cast<CompilerDescriptors::TextureDescDisplay*>(imguiDesc->m_CompilerDescriptors[CompilerDescriptors::DESC_TEXTURE]);
			
			TextureCompressor::GetInstance().SetCompressionSetting(textureDisplay->m_TextureDescriptor);
			if (lookUp == false)
				TextureCompressor::GetInstance().Run(inPath, OutPath);

			//GraphicsComponent* graphComp = reinterpret_cast<GraphicsComponent*>(Graph);
			//if (graphComp)
			AssetManager::GetInstance()->GetTextureFactory().Load(OutPath);
			OutputPath = OutPath;

		}

		if (strstr(FileName.c_str(), ".json"))
		{
			std::string InPath = "../assets/animations/";
			std::string OutPath = InPath;

			OutPath += FileName.c_str();

			

			OutName = AssetManager::GetInstance()->GetAnimationFactory().LoadAnimationPack(OutPath);
			//if (strstr(OutName.c_str(), ".json"))
			//	OutName = RemoveFileExtension(OutName, ".json");

			return OutName;

		}

		if (strstr(FileName.c_str(), ".obj") || strstr(FileName.c_str(), ".fbx") || strstr(FileName.c_str(), ".gltf") || strstr(FileName.c_str(), ".bin"))
		{
			

			lookUp = false;
			std::string& OutPath = GeomCompiler::GetInstance()->OutPath;
			OutPath = "../assets/models/";
			OutPath += FileName.c_str();
			if (strstr(FileName.c_str(), ".fbx"))
				OutPath = RemoveFileExtension(OutPath, ".fbx");
			else if (strstr(FileName.c_str(), ".gltf"))
				OutPath = RemoveFileExtension(OutPath, ".gltf");
			else if (strstr(FileName.c_str(), ".obj"))
				OutPath = RemoveFileExtension(OutPath, ".obj");
			else if (strstr(FileName.c_str(), ".bin"))
			{
				lookUp = true;
				/*OutPath = std::filesystem::path(OutPath).filename().string();*/
			}
			std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);

			EntityID currEntity = panel->getSelectedEntity();
			if (currEntity < 1)
				return std::string();
			IComponent* Graph = getComponentByName("Graphics Component", panel->getSelectedEntity());


			if (Graph == nullptr)
				Graph = addComponentByName("Graphics Component", panel->getSelectedEntity());

			GraphicsComponent* graphComp = reinterpret_cast<GraphicsComponent*>(Graph);


			if (lookUp == false)
			{
				CompilerDescriptors::GeomDescDisplay* geomDisplay = reinterpret_cast<CompilerDescriptors::GeomDescDisplay*>(imguiDesc->m_CompilerDescriptors[CompilerDescriptors::DESC_GEOMETRY]);

				/*GeomDescriptor m_GeomDescriptor{};
				m_GeomDescriptor.m_Descriptor.m_FilePath = std::filesystem::path(FileName).filename().string();*/

				MeshLoader::Request req{};
				req.m_FileName = std::filesystem::path(FileName).filename().string();
				req.m_OutFile = OutPath;

				req.currSetting = geomDisplay->m_GeomDecriptor;
				req.currSetting.m_Descriptor.m_FilePath = std::filesystem::path(FileName).filename().string();

				if (req.currSetting.m_LoadAnimation)
				{
					req.m_AnimOutFile = req.m_FileName;
					req.m_AnimOutFile = RemoveFileExtension(req.m_AnimOutFile, ".fbx");
					req.m_AnimOutFile += ".json";
				}
				if (req.currSetting.m_LoadMaterials)
				{
					req.m_MaterialOutFile = req.m_FileName;
					req.m_MaterialOutFile = RemoveFileExtension(req.m_MaterialOutFile, ".fbx");
					req.m_MaterialOutFile += ".json";
				}
				req.m_OutFile += "_Bin";
				req.m_OutFile += ".bin";

				
		
				MeshLoader::GetInstance().RunCompiler(req);

				if (req.currSetting.m_LoadMaterials)
				{
					if (req.currSetting.m_LoadMaterialTextures)
					{
						std::string OutPath = ASSET_PATH;
						OutPath += "/textures/";

						for (auto& textureTOLoad : req.m_MaterialOut.m_TextureToload)
						{
							OutPath += textureTOLoad.m_TexturePath.c_str();
							std::string inPath = OutPath;
							if (strstr(OutPath.c_str(), ".jpg"))
								OutPath = RemoveFileExtension(OutPath, ".jpg");
							else if (strstr(OutPath.c_str(), ".png"))
								OutPath = RemoveFileExtension(OutPath, ".png");
							OutPath += ".dds";

							TextureCompressor::GetInstance().Run(inPath, OutPath);
							AssetManager::GetInstance()->GetTextureFactory().Load(OutPath);
						}
					}

					GraphicsManager::getInstance().GetMaterialManager().m_ModelToMaterials[req.m_MaterialOut.m_ModelName] = req.m_MaterialOut;
					
				}

				
				std::string OutputFile = req.m_OutFile;
				OutName = AssetManager::GetInstance()->GetMeshFactory().LoadModel(OutputFile);
				


				if (req.currSetting.m_LoadAnimation && !req.currSetting.m_LoadMesh)
				{
					return std::string();
				}
				else
				{
					std::string OutputFile = req.m_OutFile;
					OutName = AssetManager::GetInstance()->GetMeshFactory().LoadModel(OutputFile);
				}

			}
			else
			{
				OutName = AssetManager::GetInstance()->GetMeshFactory().LoadModel(OutPath);
				/*OutputPath = OutPath;*/
			}

			//std::filesystem::path FilePath(OutputPath);
			//std::string assetName = FilePath.filename().string();

			BuildEntityMeshHierachy(OutName, currEntity);
			
			


			return OutName;
		}
		//if .json, load scene...
		if (strstr(FileName.c_str(), ".ttf"))
		{
			std::string selectedTextureDir = m_curr_path.string();
			selectedTextureDir += "/";
			selectedTextureDir += FileName;
			std::string inputPath = selectedTextureDir;
			std::string outPath = m_curr_path.string();
			selectedTextureDir = RemoveFileExtension(selectedTextureDir, ".ttf");

			std::string FileName = std::filesystem::path(selectedTextureDir).filename().string();

			outPath += ("/OutFont_" + FileName);
			if (!std::filesystem::exists(outPath))
			{
				if (!std::filesystem::create_directory(outPath))
					TDS_ERROR("Failed to create directory");
			}
			std::string folderName = outPath;
			outPath += "/" + FileName;
			LoaderDescriptor FontDescriptor{};
			FontDescriptor.OutPath = outPath;
			FontDescriptor.m_InputPath = inputPath;

			std::string ddsFontPath{};
			if (FontLoader::RunFontLoader(FontDescriptor, ddsFontPath))
			{
				std::shared_ptr<Hierarchy> panel = static_pointer_cast<Hierarchy>(LevelEditorManager::GetInstance()->panels[HIERARCHY]);

				IComponent* font = getComponentByName("UI Sprite", panel->getSelectedEntity());
				if (font != nullptr)
				{
					AssetManager::GetInstance()->GetFontFactory().Load(folderName);
				}
			}
			OutputPath = ddsFontPath;

		}
		std::filesystem::path FilePath(OutputPath);
		return FilePath.filename().string();
	}
	void AssetBrowser::destroyIcons()
	{
		fileIcon.m_VulkanTexture->Destroy(); //temp, to prevent mem leak
		folderIcon.m_VulkanTexture->Destroy(); //temp, to prevent mem leak

		/* when breakpoint, these 2 lines below free nothing

		fileIcon.Destroy();
		folderIcon.Destroy();*/

		//seems like the while loops dont cause multiple textures to be freed 
		//while (file_image_count)
		//{
		//	fileIcon.m_VulkanTexture->Destroy(); //temp, to prevent mem leak
		//	//fileIcon.Destroy();
		//	file_image_count--;

		//}
		//while (folder_image_count)
		//{
		//	folderIcon.m_VulkanTexture->Destroy(); //temp, to prevent mem leak
		//	//folderIcon.Destroy();
		//	folder_image_count--;

		//}
	}
}