#include "Tools/TextureCompressor.h"
#include "Tools/DDSConverter.h"

#ifdef _DEBUG
#pragma comment(lib, "CMP_Framework_MDd.lib") 
#else
#pragma comment(lib, "CMP_Framework_MD.lib")
#endif

// This file uses components from the Compressenator Common Examples. 
// For the full copyright notice, see LICENSE.txt.
namespace TDS
{
    bool CompressionCallback(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
    {
        (pUser1);
        (pUser2);

        return false;
    }
    TextureCompressor::TextureCompressor()
    {
        TextureCompilerSetting compilerSettings{};
        SetCompressionSetting(compilerSettings);
        CMP_InitFramework();
    }
    TextureCompressor::~TextureCompressor()
    {
    }
    CMP_Compute_type TextureCompressor::GetComputeType(std::string_view computeType)
    {
        if (computeType == "Vulkan")
            return CMP_GPU_VLK;
        else if (computeType == "OpenCL")
            return CMP_GPU_OCL;
        else if (computeType == "CPU")
            return CMP_CPU;
        else if (computeType == "SPMD")
            return CMP_HPC;
        else if (computeType == "DirectX")
            return CMP_GPU_DXC;
        else
            return CMP_UNKNOWN;
    }
    CMP_FORMAT TextureCompressor::GetCompressorFormat(std::string_view setting)
    {
        if (setting == "BC1")
            return CMP_FORMAT_BC1;
        else if (setting == "BC3")
            return CMP_FORMAT_BC3;
        else if (setting == "BC4")
            return CMP_FORMAT_BC4_S;
        else if (setting == "BC4_Signed")
            return CMP_FORMAT_BC4;
        else if (setting == "BC5_Signed")
            return CMP_FORMAT_BC5_S;
        else if (setting == "BC5")
            return CMP_FORMAT_BC5;
        else if (setting == "BC6_Signed")
            return CMP_FORMAT_BC6H_SF;
        else if (setting == "BC6")
            return CMP_FORMAT_BC6H;
        else if (setting == "BC7")
            return CMP_FORMAT_BC7;
        else if (setting == "RGBA")
            return CMP_FORMAT_RGBA_32F;
        else if (setting == "BGRA")
            return CMP_FORMAT_BGRA_32F;
        else if (setting == "RGBA_8888S")
            return CMP_FORMAT_RGBA_8888_S;
        else if (setting == "RGBA_8888")
            return CMP_FORMAT_RGBA_8888;
        else if (setting == "RGBA_16")
            return CMP_FORMAT_RGBA_16;


        else
            m_ToggleCompression = false;

    }
    void TextureCompressor::Run(std::string_view path, std::string_view out)
    {
        if (LoadTexture(path))
        {
            CMP_ERROR result{};
            if (m_ToggleCompression == true)
            {
                result = CMP_ProcessTexture(&m_Input, &m_Output, m_Options, CompressionCallback);
                if (result != CMP_OK)
                    std::printf("Process Texture Failure!\n");
            }


            result = CMP_SaveTexture(out.data(), &m_Output);
            CMP_FreeMipSet(&m_Input);
            CMP_FreeMipSet(&m_Output);

            if (result != CMP_OK)
                std::printf("Failed to save processed file!\n");

            return;
        }

        std::printf("Failed to load raw image!\n");

    }
    bool TextureCompressor::LoadTexture(std::string_view path)
    {
        memset(&m_Output, 0, sizeof(CMP_MipSet));
        return CMP_LoadTexture(path.data(), &m_Input) == CMP_OK;
    }
    void TextureCompressor::SetCompressionSetting(TextureCompilerSetting setting)
    {
        memset(&m_Options, 0, sizeof(KernelOptions));
        m_Options.encodeWith = GetComputeType(setting.m_Desc.m_EncoderString);
        m_Options.format = GetCompressorFormat(setting.m_Desc.m_FormatString);
        m_Options.fquality = setting.m_Desc.m_FloatQuality;
        m_Options.threads = setting.m_Desc.m_NumberOfThreads;
    }

    bool TextureCompilerSetting::SerializeSettings(std::string_view FilePath, bool Read)
    {
        /*if (Read)
        {
            std::ifstream inFile(std::string(FilePath), std::ios::in);
            if (!inFile.is_open())
            {
                return false;
            }

            std::string line{}, temp{};
            while (std::getline(inFile, line))
            {
                if (line == "[ CompresstionType ]")
                {
                    if (std::getline(inFile, m_Desc.m_FormatString))
                    {
                        m_Desc.m_FormatString.erase(0, 3);
                        if (m_Desc.m_FormatString.back() == '\"')
                        {
                            m_Desc.m_FormatString.pop_back();
                        }
                    }
                }
                else if (line == " [ EnconderType ]")
                {
                    if (std::getline(inFile, m_Desc.m_EncoderString))
                    {
                        m_Desc.m_EncoderString.erase(0, 3);
                        if (m_Desc.m_EncoderString.back() == '\"')
                        {
                            m_Desc.m_EncoderString.pop_back();
                        }
                    }
                }
                else if (line == "[ QualityLevel ]")
                {
                    inFile >> std::ws >> m_Desc.m_FloatQuality;
                }
            }

            inFile.close();
            return true;
        }
        else
        {
            std::ofstream outFile(std::string(FilePath), std::ios::out | std::ios::trunc);
            if (!outFile.is_open())
            {
                return false;
            }

            outFile << "[ CompresstionType ]\n"
                << "  \"" << m_Desc.m_FormatString << "\"\n\n"
                << "[ EnconderType ]\n"
                << "  \"" << m_Desc.m_EncoderString << "\n\n"
                << "[ QualityLevel ]\n"
                << "  \"" << m_Desc.m_FloatQuality << "\"\n";


            outFile.close();
            return true;
        }*/
        return false;
    }
}