#include <MessagingSystem/MessageSystem.h>

namespace TDS
{
	MessageSystem::MessageSystem() :m_MessageLookup()
	{
	}
	MessageSystem::~MessageSystem()
	{
		ShutDown();
	}
	void MessageSystem::RemoveUnwantedMessages()
	{
		for (auto& msgContext : m_RemovedContexts)
		{
			//Double confirm if marked to remove
			if (m_MessageLookup[msgContext]->m_RemovalMessage == false)
				continue;

			delete m_MessageLookup[msgContext];
			m_MessageLookup.erase(msgContext);
		}
		m_RemovedContexts.clear();

	}
	void MessageSystem::ShutDown()
	{
		for (auto& itr : m_MessageLookup)
			delete itr.second;
		m_MessageLookup.clear();

	}
	void MessageSystem::UnregisterMessage(std::string_view messageContext)
	{
		m_MessageLookup[messageContext.data()]->m_RemovalMessage = true;
		m_RemovedContexts.emplace_back(messageContext.data());
	}

}
