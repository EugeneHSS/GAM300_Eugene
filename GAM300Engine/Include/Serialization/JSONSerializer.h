#pragma once
#include "Serialization/SerializerBase.h"
#include "rapidJSON/document.h"
#include "rapidJSON/writer.h"
#include "rapidJSON/prettywriter.h"
#include "rapidJSON/stringbuffer.h"
#include <rttr/type.h>
namespace TDS
{

#define DEFINE_WRITE_TO_TEMPLATE(TYPE, TYPENAME)\
	template <>\
	ERROR_TYPE WriteToJson(TYPE& data)\
	{\
		if (m_Writer.TYPENAME(data))\
			return ERROR_TYPE::SUCCESS;\
		\
		return ERROR_TYPE::FAILED;\
	}



	class JSONSerializer : public SerializerBase<JSONSerializer>
	{
	public:
		std::ofstream* m_OutStream = nullptr;
		std::ifstream* m_InputStream = nullptr;
		rapidjson::Document*									m_Document = nullptr;
		rapidjson::StringBuffer*								m_StringBuffer = nullptr ;
		rapidjson::PrettyWriter<rapidjson::StringBuffer>*		m_Writer = nullptr;


	private:
		bool WriteVariant(const rttr::variant& var);
		bool WriteBasicTypes(const rttr::variant& var);
		bool WriteBasicTypes(const rttr::type& t, const rttr::variant& var);
		void WriteSequential(const rttr::variant& var);
		void SerializeRecursion(const rttr::variant& var);
		void WriteAssociative(const rttr::variant& var);

		rttr::variant extract_basic_types(rapidjson::Value& val);
		void DeserializeRecursion(rttr::instance m_inst, rapidjson::Value& json_obj);
		void ReadSequential(rttr::variant_sequential_view& view, rapidjson::Value& json_array_value);
		void ReadAssociative(rttr::variant_associative_view& view, rapidjson::Value& json_map);
		rttr::variant ExtractValue(rapidjson::Value::MemberIterator& itr, const rttr::type& t);
		bool WritePair(const rttr::variant& var);

	public:
		JSONSerializer();
		~JSONSerializer();

		ERROR_TYPE	Open(std::string_view fileName, bool read);
		void		Close(bool read);

		template <typename T>
		ERROR_TYPE		RunSerializer(T* data)
		{
			SerializeRecursion(*data);
			return ERROR_TYPE::SUCCESS;
		}

		template <typename T>
		ERROR_TYPE		RunDeserializer(T* Data)
		{
			DeserializeRecursion(*Data, *m_Document);
			return ERROR_TYPE::SUCCESS;
		}


		

		
	};
}