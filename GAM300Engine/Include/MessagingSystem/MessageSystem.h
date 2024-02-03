#pragma once
#pragma once
#include <pch.h>
#include <Logger/Logger.h>
#include "dotnet/ImportExport.h"
namespace TDS
{




	struct Message
	{
		bool	m_RemovalMessage = false;

	};

	template <typename Ret, typename ...Args>
	struct MessageArg : Message
	{
		std::function<Ret(Args...)>				m_Function = nullptr;

		Ret PassMessage(Args&&... msg)
		{
			return m_Function(std::forward<Args>(msg)...);
		}

	};

	template <typename ...Args>
	struct MessageArg<void, Args...> : Message
	{
		std::function<void(Args...)>			m_Function = nullptr;

		void PassMessage(Args&& ...msg)
		{
			m_Function(std::forward<Args>(msg)...);
		}

	};

	//RECOMMENDED TO NOT USE THIS FOR AI BEHAVIOURS FOR NOW BECAUSE I PLANNED THIS MAINLY TO BE USED FOR MESSAGING TO THE GRAPHICS SYSTEM.

	class MessageSystem
	{
	private:
		std::unordered_map<std::string, Message*>			m_MessageLookup;
		std::vector<std::string>							m_RemovedContexts;
	public:
		DLL_API MessageSystem();
		DLL_API ~MessageSystem();
		DLL_API void												RemoveUnwantedMessages();
		DLL_API void												ShutDown();
		DLL_API void												UnregisterMessage(std::string_view messasgeContext);

		template <typename T, typename Ret, typename ...Args>
		void RegisterMessage(std::string_view messageContext, T* listener, Ret(T::* m_CallBack)(Args...))
		{
			auto itr = m_MessageLookup.find(messageContext.data());

			if (itr != m_MessageLookup.end())
			{
				TDS_WARN("Message context : {} , already exists!", messageContext.data());
				return;
			}

			MessageArg<Ret, Args...>* newMsg = new MessageArg<Ret, Args...>();

			if constexpr (std::is_same_v<Ret, void>)
			{
				newMsg->m_Function = [listener, m_CallBack](Args&&... args)
				{
					(listener->*m_CallBack)(std::forward<Args>(args)...);
				};
			}
			else
			{
				newMsg->m_Function = [listener, m_CallBack](Args&&... args) -> Ret
				{
					return (listener->*m_CallBack)(std::forward<Args>(args)...);
				};
			}

			m_MessageLookup[messageContext.data()] = newMsg;
		}


		template <typename Ret, typename ...Args>
		Ret BroadCastMessage(std::string_view messageContext, Args&&... params)
		{
			auto itr = m_MessageLookup.find(messageContext.data());
			if (itr != m_MessageLookup.end())
			{
				MessageArg<Ret, Args...>* msg = reinterpret_cast<MessageArg<Ret, Args...>*>(itr->second);
				if (!msg->m_RemovalMessage)
				{
					return msg->PassMessage(std::forward<Args>(params)...);
				}
				else
				{
					TDS_WARN("MessageContext : {}, is removed", messageContext.data());
					return Ret();
				}
			}
			else
			{
				TDS_WARN("Failed to find messageContext {}", messageContext.data());
			}
			return Ret();
		}

		template <typename ...Args>
		void BroadCastMessage(std::string_view messageContext, Args&&... params)
		{
			auto itr = m_MessageLookup.find(messageContext.data());
			if (itr != m_MessageLookup.end())
			{
				MessageArg<void, Args...>* msg = reinterpret_cast<MessageArg<void, Args...>*>(itr->second);
				if (!msg->m_RemovalMessage)
				{
					msg->PassMessage(std::forward<Args>(params)...);
				}
				else
				{
					TDS_WARN("MessageContext : {}, is removed", messageContext.data());
				}
			}
			else
			{
				TDS_WARN("Failed to find messageContext {}", messageContext.data());
			}
		}

	};


#define REGISTER_MESSAGE(MSG_CONTEXT, LISTENER, FUNC)\
	GraphicsManager::getInstance().GetMessageSystem().RegisterMessage(##MSG_CONTEXT,LISTENER, FUNC);

#define BROADCAST_MESSAGE(MSG_CONTEXT, ...)\
	GraphicsManager::getInstance().GetMessageSystem().BroadCastMessage(##MSG_CONTEXT, __VA_ARGS__);

#define UNREGISTER_MESSAGE(MSG_CONTEXT)\
	GraphicsManager::getInstance().GetMessageSystem().UnregisterMessage(##MSG_CONTEXT);

}
