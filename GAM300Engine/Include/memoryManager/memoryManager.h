/*!*************************************************************************
****
\file memoryManager.h
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program declares the functions in the memoryManager class
****************************************************************************
***/

#ifndef MEMORYMANAGER
#define MEMORYMANAGER

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "dotnet/ImportExport.h"
namespace TDS
{
	class MemoryManager // for ECS
	{
	private:
		typedef std::string ArchetypeID;

	public:
		/*!*************************************************************************
		Returns an instance of the MemoryManager
		****************************************************************************/
		DLL_API static std::unique_ptr<MemoryManager>& GetInstance();

		struct BookData
		{
			// For reserved memory (total memory that can be committed)
			std::vector<std::uint32_t> reservedSize;
			// For memory committed / going to commit
			std::vector<std::uint32_t> usedSize;
			//
			std::vector<std::uint32_t> committedSize;
			// 
			std::vector<unsigned char*> startOfComponent;
		};

		/*!*************************************************************************
		Sets the number of components (from ECS)
		****************************************************************************/
		DLL_API void setNumberOfComponent(std::uint32_t _numberOfComponents);

		/*!*************************************************************************
		Adds a new book (Archetype)
		****************************************************************************/
		DLL_API void newBook(const ArchetypeID& archetypeID);
		/*!*************************************************************************
		Adds a new page (Component) in given book using ArchetypeID & ComponentID
		****************************************************************************/
		DLL_API unsigned char* newPage(const ArchetypeID& archetypeID, std::uint32_t componentID);
		/*!*************************************************************************
		Resizes the page (Component) in given book using ArchetypeID & ComponentID
		****************************************************************************/
		DLL_API void resizePage(const ArchetypeID& archetypeID, std::uint32_t componentID);

		/*!*************************************************************************
		Commits the book (Archetypes) in given book using ArchetypeID
		****************************************************************************/
		DLL_API void commitBook(const ArchetypeID& archetypeID);
		/*!*************************************************************************
		Commits the page (Archetypes) in given book using ArchetypeID & ComponentID
		****************************************************************************/
		DLL_API void commitPage(const ArchetypeID& archetypeID, std::uint32_t componentID);

		/*!*************************************************************************
		Frees the book (Archetypes) in given book using ArchetypeID
		****************************************************************************/
		DLL_API void freeBook(const ArchetypeID& archetypeID);
		/*!*************************************************************************
		Frees the page (Component) in given pointer
		****************************************************************************/
		DLL_API void freePage(unsigned char* pointer);

		/*!*************************************************************************
		Reserve space in the given book (Archetypes) using ArchetypeID & ComponentID
		****************************************************************************/
		DLL_API void reserveComponentSpace(const ArchetypeID& archetypeID, std::uint32_t componentID, std::uint32_t componentSize);

		/*!*************************************************************************
		Getting all the component data in the given book (Archetypes) using 
		ArchetypeID
		****************************************************************************/
		DLL_API std::vector<unsigned char*>& getComponents(ArchetypeID archetypeID);
		/*!*************************************************************************
		Getting the component data in the given book (Archetypes) using ArchetypeID
		& ComponentID
		****************************************************************************/
		DLL_API unsigned char* getComponentData(ArchetypeID archetypeID, std::uint32_t componentID, std::uint32_t componentSize, std::uint32_t index);
		/*!*************************************************************************
		Adding the component data in the given book (Archetypes) using ArchetypeID
		& ComponentID
		****************************************************************************/
		DLL_API unsigned char* addComponentData(ArchetypeID archetypeID, std::uint32_t componentID, std::uint32_t componentSize, std::uint32_t index);
		/*!*************************************************************************
		Removing the end the component data in the given book (Archetypes) using
		ArchetypeID & ComponentID
		****************************************************************************/
		DLL_API void removeComponentEndData(ArchetypeID archetypeID, std::uint32_t componentID, std::uint32_t componentSize);

	private:
		// Unique pointer to MemoryManager
		 static std::unique_ptr<MemoryManager> m_instance;

		const std::uint32_t PAGE_SIZE = 10000000;
		std::uint32_t numberOfComponents;

		 std::unordered_map<ArchetypeID, BookData> books;

		/*!*************************************************************************
		Reserve memory using VirtualAlloc
		****************************************************************************/
		unsigned char* reserve(std::uint32_t newPageSize, unsigned char* startingPointer = nullptr);
		/*!*************************************************************************
		Coomit memory using VirtualAlloc
		****************************************************************************/
		unsigned char* commit(std::uint32_t usedSize, unsigned char* startingPointer = nullptr);
		/*!*************************************************************************
		Decommit memory using VirtualAlloc
		****************************************************************************/
		void decommit(std::uint32_t componentSize, unsigned char* pointer);
		/*!*************************************************************************
		Release memory using VirtualAlloc
		****************************************************************************/
		void release(unsigned char* startingPointer);
	};
}

#endif