// GamApp.cpp
#include <shlwapi.h>                // GetModuleFileNameA(), PathRemoveFileSpecA()
#pragma comment(lib, "shlwapi.lib") // Needed for <shlwapi.h>
#include <vector>
#include <array>
#include <sstream>

#include "GameApp.h"
#include "Logger/Logger.h"
#include "Input/Input.h"
#include "vulkanTools/Model.h"

#include "vulkanTools/FrameInfo.h"
#include "Timestep/Timestep.h"
#include "sceneManager/sceneManager.h"
#include "Shader/ShaderLoader.h"
#include "Rendering/GraphicsManager.h"
#include "components/components.h"
#include "Rendering/RendererSystem.h"
#include "vulkanTools/VulkanHelper.h"

#include "vulkanTools/CommandManager.h"
#include "GraphicsResource/TextureInfo.h"
#include "vulkanTools/VulkanTexture.h"
#include "Rendering/renderPass.h"
#include "vulkanTools/FrameBuffer.h"


#include "Physics/PhysicsSystem.h"
#include "Rendering/ObjectPicking.h"

namespace TDS
{
    bool SceneManager::isPlaying;

    GamApp::GamApp(HINSTANCE hinstance, int& nCmdShow, const wchar_t* classname, WNDPROC wndproc)
        :m_window(hinstance, nCmdShow, classname)
    {
        Log::Init();
        m_window.createWindow(wndproc, 1280, 720);
        TDS_INFO("window width: {}, window height: {}", m_window.getWidth(), m_window.getHeight());
    }
    void  GamApp::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        SetWindowHandle(hWnd);

        switch (uMsg)
        {

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_PAINT:
            ValidateRect(m_window.getWindowHandler(), NULL);
            break;
        case WM_SIZE: //for resize of window may need it for fullscreen?
            m_window.setWidth(LOWORD(lParam));
            m_window.setHeight(HIWORD(lParam));
            m_window.WindowIsResizing(true);
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        {
            Input::processMouseInput(wParam, lParam);
        }break;

        case WM_MOUSEMOVE:
        {
            Input::updateMousePosition(lParam);
        }break;

        case WM_MOUSEWHEEL:
        {
            Input::processMouseScroll(wParam);
        }break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        {
            uint32_t VKcode = static_cast<uint32_t>(wParam);
            WORD keyflags = HIWORD(lParam);
            if (!(keyflags & KF_REPEAT))
            {
                Input::keystatus = Input::KeyStatus::PRESSED;
            }
            else if (keyflags & KF_REPEAT)
                Input::keystatus = Input::KeyStatus::REPEATED;

            bool wasDown = (lParam & (1 << 30)) != 0;
            bool isDown = (static_cast<unsigned int>(lParam) & (1 << 31)) == 0;
            Input::processKeyboardInput(VKcode, wasDown, isDown);
        }break;
        case WM_KEYUP:
        {
            uint32_t VKcode = static_cast<uint32_t>(wParam);
            bool wasDown = (lParam & (1 << 30)) != 0;
            bool isDown = (static_cast<unsigned int>(lParam) & (1 << 31)) == 0;

            Input::processKeyboardInput(VKcode, wasDown, isDown);
            Input::keystatus = Input::KeyStatus::RELEASED;
            Input::keystatus = Input::KeyStatus::IDLE;
        }break;
        }
    }
    void GamApp::SetWindowHandle(HWND hWnd)
    {
        m_handler = hWnd;
    }
    HWND GamApp::GetWindowHandle()
    {
        return m_handler;
    }

    void GamApp::Initialize()
    {
       GraphicsManager::getInstance().Init(&m_window);
       AssetManager::GetInstance()->PreloadAssets();
       skyboxrender.Init();
    }

    void GamApp::FixedUpdate()
    {
        auto executeFixedUpdate = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteFixedUpdate"
            );
        executeFixedUpdate();
    }

    void GamApp::Update()
    {
        auto executeUpdate = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteUpdate"
            );

        while (m_window.processInputEvent()) //while is_playing?
        {
            TimeStep::CalculateDeltaTime();
            float DeltaTime = TimeStep::GetDeltaTime();
          
            GraphicsManager::getInstance().setCamera(m_GameCamera);
            GraphicsManager::getInstance().GetCamera().setEditorCamera(false);

           GraphicsManager::getInstance().GetCamera().UpdateCamera(DeltaTime, true);

            Vec3 m_windowdimension{ static_cast<float>(m_window.getWidth()), static_cast<float>(m_window.getHeight()), 1.f };
            if (GraphicsManager::getInstance().getFrameBuffer().getDimensions() != m_windowdimension && m_windowdimension.x > 0 && m_windowdimension.y > 0)
            {
                GraphicsManager::getInstance().getFrameBuffer().resize(m_windowdimension, GraphicsManager::getInstance().getRenderPass().getRenderPass());
 
            }
            GraphicsManager::getInstance().StartFrame();
            VkCommandBuffer commandBuffer = GraphicsManager::getInstance().getCommandBuffer();
            std::uint32_t frame = GraphicsManager::getInstance().GetSwapchainRenderer().getFrameIndex();
            

            /*GraphicsManager::getInstance().getRenderPass().beginRenderPass(commandBuffer, &GraphicsManager::getInstance().getFrameBuffer());*/

           /* skyboxrender.RenderSkyBox(commandBuffer, frame);*/

            FixedUpdate();
            ecs.runSystems(1, DeltaTime); // Other systems
            executeUpdate();
            LateUpdate();
            ecs.runSystems(2, DeltaTime); // Event handler
            ecs.runSystems(3, DeltaTime); // Graphics

            GraphicsManager::getInstance().DrawFrame();
            //// event handling systems 
            /*GraphicsManager::getInstance().getRenderPass().endRenderPass(commandBuffer);*/

           

            GraphicsManager::getInstance().GetSwapchainRenderer().BeginSwapChainRenderPass(commandBuffer);
            GraphicsManager::getInstance().RenderFullScreen();
            GraphicsManager::getInstance().GetSwapchainRenderer().EndSwapChainRenderPass(commandBuffer);
            GraphicsManager::getInstance().EndFrame();

            Input::scrollStop();

        }
        stopScriptEngine();
        AssetManager::GetInstance()->ShutDown();
        //vkDeviceWaitIdle(GraphicsManager::getInstance().getVkInstance().getVkLogicalDevice());
        ecs.destroy();
      /*  skyboxrender.ShutDown();*/
        GraphicsManager::getInstance().ShutDown();
    }

    void GamApp::LateUpdate()
    {
        auto executeLateUpdate = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteLateUpdate"
            );
        executeLateUpdate();
    }

    void GamApp::Run()
    {
        startScriptEngine();
        //compileScriptAssembly();

        // Step 1: Get Functions
        auto init = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "Init"
            );

        // Step 2: Initialize
        init();

        SceneManager::GetInstance()->addScript = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "AddScriptViaName"
            );
        SceneManager::GetInstance()->removeScript = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "RemoveScriptViaName"
            );

        SceneManager::GetInstance()->getScriptVariables = GetFunctionPtr<std::vector<ScriptValues>(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "GetScriptVariables"
            );

        SceneManager::GetInstance()->hasScript = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "HasScriptViaName"
            );

        SceneManager::GetInstance()->getAllScripts = GetFunctionPtr<std::vector<std::string>(*)()>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "GetAllScripts"
            );

        ecs.addScriptList = GetFunctionPtr<void(*)(EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "AddScriptList"
            );

        ecs.removeScriptList = GetFunctionPtr<void(*)(EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "RemoveEntity"
            );

        /*
        SceneManager::GetInstance()->setBool = GetFunctionPtr<void(*)(EntityID, std::string, std::string, bool)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueBool"
            );

        SceneManager::GetInstance()->setInt = GetFunctionPtr<void(*)(EntityID, std::string, std::string, int, bool)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueInt"
            );

        SceneManager::GetInstance()->setDouble = GetFunctionPtr<void(*)(EntityID, std::string, std::string, double)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueDouble"
            );

        SceneManager::GetInstance()->setFloat = GetFunctionPtr<void(*)(EntityID, std::string, std::string, float)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueFloat"
            );

        SceneManager::GetInstance()->setString = GetFunctionPtr<void(*)(EntityID, std::string, std::string, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetValueString"
            );

        //SceneManager::GetInstance()->setChar = GetFunctionPtr<void(*)(EntityID, std::string, std::string, char)>
        //    (
        //        "ScriptAPI",
        //        "ScriptAPI.EngineInterface",
        //        "SetValueChar"
        //    );

        SceneManager::GetInstance()->setVector3 = GetFunctionPtr<void(*)(EntityID, std::string, std::string, Vec3)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetVector3"
            );

        SceneManager::GetInstance()->setGameObject = GetFunctionPtr<void(*)(EntityID, std::string, std::string, EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetGameObject"
            );

        SceneManager::GetInstance()->setComponent = GetFunctionPtr<void(*)(EntityID, std::string, std::string, EntityID)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetComponent"
            );

        SceneManager::GetInstance()->setScriptReference = GetFunctionPtr<void(*)(EntityID, std::string, std::string, EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetScript"
            );
        */

        SceneManager::GetInstance()->setScriptValue = GetFunctionPtr<void(*)(EntityID, std::string, ScriptValues)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetVariable"
            );

        SceneManager::GetInstance()->setScriptValues = GetFunctionPtr<void(*)(EntityID, std::string, std::vector<ScriptValues>&)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "SetVariables"
            );

        SceneManager::GetInstance()->updateName = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "UpdateGameObjectName"
            );

        SceneManager::GetInstance()->isScriptEnabled = GetFunctionPtr<bool(*)(EntityID, std::string)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "IsScriptEnabled"
            );

        SceneManager::GetInstance()->awake = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteAwake"
            );

        SceneManager::GetInstance()->start = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteStart"
            );

        SceneManager::GetInstance()->isPlaying = true;
        SceneManager::GetInstance()->Init();
        ecs.initializeSystems(1);
        ecs.initializeSystems(2);
        ecs.initializeSystems(3);

        auto awake = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteAwake"
            );
        awake();

        auto start = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteStart"
            );
        start();
    }

    void GamApp::Awake()
    {
        auto awake = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteAwake"
            );
        awake();
    }

    void GamApp::Start()
    {
        auto start = GetFunctionPtr<void(*)(void)>
            (
                "ScriptAPI",
                "ScriptAPI.EngineInterface",
                "ExecuteStart"
            );
        start();
    }

    GamApp::~GamApp()
    {
        m_window.~WindowsWin();
    }

    void GamApp::startScriptEngine()
    {
         //Get the .NET Runtime's path first
        const auto DOT_NET_PATH = getDotNetRuntimePath();
        if (DOT_NET_PATH.empty())
            throw std::runtime_error("Failed to find .NET Runtime.");

        // Get the current executable directory so that we can find the coreclr.dll to load
        std::string runtimePath(MAX_PATH, '\0');
        GetModuleFileNameA(nullptr, runtimePath.data(), MAX_PATH);
        PathRemoveFileSpecA(runtimePath.data());
        // Since PathRemoveFileSpecA() removes from data(), the size is not updated, so we must manually update it
        runtimePath.resize(std::strlen(runtimePath.data()));

        // Also, while we're at it, set the current working directory to the current executable directory
        std::filesystem::current_path(runtimePath);

        // Construct the CoreCLR path
        const std::string CORE_CLR_PATH = DOT_NET_PATH + "\\coreclr.dll";

        // Load the CoreCLR DLL
        coreClr = LoadLibraryExA(CORE_CLR_PATH.c_str(), nullptr, 0);
        if (!coreClr)
            throw std::runtime_error("Failed to load CoreCLR.");

        // Step 2: Get CoreCLR hosting functions
        initializeCoreClr = getCoreClrFuncPtr<coreclr_initialize_ptr>("coreclr_initialize");
        createManagedDelegate = getCoreClrFuncPtr<coreclr_create_delegate_ptr>("coreclr_create_delegate");
        shutdownCoreClr = getCoreClrFuncPtr<coreclr_shutdown_ptr>("coreclr_shutdown");

        // Step 3: Construct AppDomain properties used when starting the runtime
        std::string tpaList = buildTpaList(runtimePath) + buildTpaList(DOT_NET_PATH);

        // Define CoreCLR properties
        std::array propertyKeys =
        {
            "TRUSTED_PLATFORM_ASSEMBLIES",      // Trusted assemblies (like the GAC)
            "APP_PATHS",                        // Directories to probe for GamApp assemblies
        };
        std::array propertyValues =
        {
            tpaList.c_str(),
            runtimePath.c_str()
        };

        // Step 4: Start the CoreCLR runtime
        int result = initializeCoreClr
        (
            runtimePath.c_str(),     // AppDomain base path
            "SampleHost",            // AppDomain friendly name, this can be anything you want really
            propertyKeys.size(),     // Property count
            propertyKeys.data(),     // Property names
            propertyValues.data(),   // Property values
            &hostHandle,             // Host handle
            &domainId                // AppDomain ID
        );

        // Check if intiialization of CoreCLR failed
        if (result < 0)
        {
            std::ostringstream oss;
            oss << std::hex << std::setfill('0') << std::setw(8)
                << "Failed to initialize CoreCLR. Error 0x" << result << "\n";
            throw std::runtime_error(oss.str());
        }
    }

    void GamApp::stopScriptEngine()
    {
        // Shutdown CoreCLR
        const int RESULT = shutdownCoreClr(hostHandle, domainId);
        if (RESULT < 0)
        {
            std::stringstream oss;
            oss << std::hex << std::setfill('0') << std::setw(8)
                << "Failed to shut down CoreCLR. Error 0x" << RESULT << "\n";
            throw std::runtime_error(oss.str());
        }
    }

    std::string GamApp::buildTpaList(const std::string& directory)
    {
        // Constants
        const std::string SEARCH_PATH = directory + "\\*.dll";
        static constexpr char PATH_DELIMITER = ';';

        // Create a osstream object to compile the string
        std::ostringstream tpaList;

        // Search the current directory for the TPAs (.DLLs)
        WIN32_FIND_DATAA findData;
        HANDLE fileHandle = FindFirstFileA(SEARCH_PATH.c_str(), &findData);
        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                // Append the assembly to the list
                tpaList << directory << '\\' << findData.cFileName << PATH_DELIMITER;
            } while (FindNextFileA(fileHandle, &findData));
            FindClose(fileHandle);
        }

        return tpaList.str();
    }

    void GamApp::compileScriptAssembly()
    {
        //relative path to the script assembly project file
        const char* PROJ_PATH =
            "../ManagedScripts/ManagedScripts.csproj";

        std::wstring buildCmd = L" build \"" +
            std::filesystem::relative(PROJ_PATH).wstring() +
            L"\" --no-self-contained " +
            L"-o \"../scriptDLL/\" -r \"win-x64\"";

        // Define the struct to config the compiler process call
        STARTUPINFOW startInfo;
        PROCESS_INFORMATION pi;
        ZeroMemory(&startInfo, sizeof(startInfo));
        ZeroMemory(&pi, sizeof(pi));
        startInfo.cb = sizeof(startInfo);

        // Start compiler process
        const auto SUCCESS = CreateProcess
        (
            L"C:\\Program Files\\dotnet\\dotnet.exe", buildCmd.data(),
            nullptr, nullptr, true, NULL, nullptr, nullptr,
            &startInfo, &pi
        );

        // Check that we launched the process
        if (!SUCCESS)
        {
            auto err = GetLastError();
            std::ostringstream oss;
            oss << "Failed to launch compiler. Error code: "
                << std::hex << err;
            throw std::runtime_error(oss.str());
        }

        // Wait for process to end
        DWORD exitCode{};
        while (true)
        {
            const auto EXEC_SUCCESS =
                GetExitCodeProcess(pi.hProcess, &exitCode);
            if (!EXEC_SUCCESS)
            {
                auto err = GetLastError();
                std::ostringstream oss;
                oss << "Failed to query process. Error code: "
                    << std::hex << err;
                throw std::runtime_error(oss.str());
            }
            if (exitCode != STILL_ACTIVE)
                break;
        }

        // Successful build
        if (exitCode == 0)
        {
            // Copy out files
            std::filesystem::copy_file
            (
                "../scriptDLL/ManagedScripts.dll",
                "ManagedScripts.dll",
                std::filesystem::copy_options::overwrite_existing
            );
        }
        // Failed build
        else
        {
            throw std::runtime_error("Failed to build managed scripts!");
        }
    }

    std::string GamApp::getDotNetRuntimePath() const
    {
        // Check if any .NET Runtime is even installed
        const std::filesystem::path PATH =
            std::filesystem::path("C:/Program Files/dotnet/shared/Microsoft.NETCore.App");
        if (!std::filesystem::exists(PATH))
            return "";
        // Check all folders in the directory to find versions
        std::pair<int, std::filesystem::path> latestVer = { -1, {} };
        for (const auto& DIR_ENTRY : std::filesystem::directory_iterator(PATH))
        {
            // Is a file, not a folder
            if (!DIR_ENTRY.is_directory())
                continue;
            // Get the directory's name
            const auto& DIR = DIR_ENTRY.path();
            const auto& DIR_NAME = (--(DIR.end()))->string();
            if (DIR_NAME.empty())
                continue;
            // Get the version number
            const int VER_NUM = DIR_NAME[0] - '0';
            // We will only naively check major version here and ignore the rest of 
            // semantic versioning to keep things simple for this sample.
            if (VER_NUM > latestVer.first)
            {
                latestVer = { VER_NUM, DIR };
            }
        }
        // Check if we found any valid versions
        if (latestVer.first >= 0)
        {
            // Replace all forward slashes with backslashes 
            // (.NET can't handle forward slashes)
            auto dotnetPath = latestVer.second.string();
            std::replace_if
            (
                dotnetPath.begin(), dotnetPath.end(),
                [](char c) { return c == '/'; },
                '\\'
            );
            return dotnetPath;
        }
        return "";
    }

}// end TDS