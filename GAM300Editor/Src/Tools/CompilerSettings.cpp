/*!*****************************************************************************
 * \file          Descriptor.cpp
 * \author        Eugene Ho Shu Sheng
 * \par DP email: shushengeugene.ho@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         This file contains the implementation of the Descriptor class.
 *******************************************************************************/
#include "Tools/CompilerSettings.h"


bool TDS::GeomDescriptor::Serialize(std::string_view FilePath, bool Read)
{
    ////Will change this to use YAML or JSON. IF our data structure is more complex
    //if (Read)
    //{
    //    std::ifstream inFile(std::string(FilePath), std::ios::in);
    //    if (!inFile.is_open())
    //    {
    //        return false;
    //    }

    //    std::string line{}, temp{};
    //    while (std::getline(inFile, line))
    //    {
    //        if (line == "[ FilePath ]")
    //        {
    //            if (std::getline(inFile, m_Descriptor.m_FilePath))
    //            {
    //                m_Descriptor.m_FilePath.erase(0, 3);
    //                if (m_Descriptor.m_FilePath.back() == '\"')
    //                {
    //                    m_Descriptor.m_FilePath.pop_back();
    //                }
    //            }
    //        }
    //        else if (line == "[ Scale ]")
    //        {
    //            inFile >> std::ws >> m_Descriptor.m_L2W.m_Scale[0]
    //                >> std::ws >> m_Descriptor.m_L2W.m_Scale[1]
    //                >> std::ws >> m_Descriptor.m_L2W.m_Scale[2];
    //        }
    //        else if (line == "[ Rotate ]")
    //        {
    //            inFile >> std::ws >> m_Descriptor.m_L2W.m_Rotate[0]
    //                >> std::ws >> m_Descriptor.m_L2W.m_Rotate[1]
    //                >> std::ws >> m_Descriptor.m_L2W.m_Rotate[2];
    //        }
    //        else if (line == "[ Translate ]")
    //        {
    //            inFile >> std::ws >> m_Descriptor.m_L2W.m_Translate[0]
    //                >> std::ws >> m_Descriptor.m_L2W.m_Translate[1]
    //                >> std::ws >> m_Descriptor.m_L2W.m_Translate[2];
    //        }
    //        else if (line == "[ MergeAllMesh ]")
    //        {
    //            inFile >> std::ws >> m_Descriptor.MergeMesh;
    //        }
    //        else if (line == "[ RenameMesh ]")
    //        {
    //            if (std::getline(inFile, m_Descriptor.m_MeshFileName))
    //            {
    //                m_Descriptor.m_MeshFileName.erase(0, 3); // Remove the leading whitespace and quote
    //                if (m_Descriptor.m_MeshFileName.back() == '\"')
    //                {
    //                    m_Descriptor.m_MeshFileName.pop_back(); // Remove the trailing quote
    //                }
    //            }

    //            if (std::getline(inFile, m_Descriptor.m_RenamedMesh))
    //            {
    //                m_Descriptor.m_RenamedMesh.erase(0, 3); // Remove the leading whitespace and quote
    //                if (m_Descriptor.m_RenamedMesh.back() == '\"')
    //                {
    //                    m_Descriptor.m_RenamedMesh.pop_back(); // Remove the trailing quote
    //                }
    //            }
    //        }
    //    }

    //    inFile.close();
    //    return true;
    //}
    //else
    //{
    //    std::ofstream outFile(std::string(FilePath), std::ios::out | std::ios::trunc);
    //    if (!outFile.is_open())
    //    {
    //        return false;
    //    }

    //    outFile << "[ FilePath ]\n"
    //        << "  \"" << m_Descriptor.m_FilePath << "\"\n\n"
    //        << "[ Scale ]\n"
    //        << "{ X:f  Y:f  Z:f }\n"
    //        << "   " << m_Descriptor.m_L2W.m_Scale[0] << "    "
    //        << m_Descriptor.m_L2W.m_Scale[1] << "    "
    //        << m_Descriptor.m_L2W.m_Scale[2] << "\n\n"
    //        << "[ Rotate ]\n"
    //        << "{ X:f  Y:f  Z:f }\n"
    //        << "   " << m_Descriptor.m_L2W.m_Rotate[0] << "    "
    //        << m_Descriptor.m_L2W.m_Rotate[1] << "    "
    //        << m_Descriptor.m_L2W.m_Rotate[2] << "\n\n"
    //        << "[ Translate ]\n"
    //        << "{ X:f  Y:f  Z:f }\n"
    //        << "   " << m_Descriptor.m_L2W.m_Translate[0] << "    "
    //        << m_Descriptor.m_L2W.m_Translate[1] << "    "
    //        << m_Descriptor.m_L2W.m_Translate[2] << "\n\n"
    //        << "[ MergeAllMesh ]\n"
    //        << "     " << (m_Descriptor.MergeMesh ? 1 : 0) << "\n\n"
    //        << "[ RenameMesh ]\n"
    //        << "  \"" << m_Descriptor.m_MeshFileName << "\"\n"
    //        << "  \"" << m_Descriptor.m_RenamedMesh << "\"\n";

    //    outFile.close();
    //    return true;
    //}
    return false;
}

//bool TDS::TextureDescriptor::Serialize(std::string_view FilePath, bool Read)
//{
//    if (Read)
//    {
//        std::ifstream inFile(std::string(FilePath), std::ios::in);
//        if (!inFile.is_open())
//        {
//            return false;
//        }
//
//        std::string line{}, temp{};
//        while (std::getline(inFile, line))
//        {
//            if (line == "[ FilePath ]")
//            {
//                if (std::getline(inFile, m_Descriptor.m_BitMapPath))
//                {
//                    m_Descriptor.m_BitMapPath.erase(0, 3);
//                    if (m_Descriptor.m_BitMapPath.back() == '\"')
//                    {
//                        m_Descriptor.m_BitMapPath.pop_back();
//                    }
//                }
//            }
//            else if (line == "[ CompresstionType ]")
//            {
//                if (std::getline(inFile, m_Descriptor.m_Format))
//                {
//                    m_Descriptor.m_Format.erase(0, 3);
//                    if (m_Descriptor.m_Format.back() == '\"')
//                    {
//                        m_Descriptor.m_Format.pop_back();
//                    }
//                }
//            }
//        }
//
//        inFile.close();
//        return true;
//    }
//    else
//    {
//        std::ofstream outFile(std::string(FilePath), std::ios::out | std::ios::trunc);
//        if (!outFile.is_open())
//        {
//            return false;
//        }
//
//        outFile << "[ FilePath ]\n"
//            << "  \"" << m_Descriptor.m_BitMapPath << "\"\n\n"
//            << "[ CompressionType ]\n"
//            << "  \"" << m_Descriptor.m_Format << "\"\n";
//
//
//        outFile.close();
//        return true;
//    }
//    return false;
//}
