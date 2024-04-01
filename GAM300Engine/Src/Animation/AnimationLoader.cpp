#include "Animation/AnimationLoader.h"
#include "Animation/AnimationLoader.h"
#include "Serialization/JSONSerializer.h"
#include "components/CompReflectionMacro.h"


namespace TDS
{

	RTTR_REGISTRATION
	{


		RTTR_REGISTER_WITH_NAME(AnimKeyFrame, "AnimKeyFrame").constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(AnimKeyFrame, time);

		RTTR_REGISTER_WITH_NAME(AnimPos, "AnimPos")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(AnimPos, Pos);

		RTTR_REGISTER_WITH_NAME(AnimRotQ, "AnimPos")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(AnimRotQ, RotQ);

		RTTR_REGISTER_WITH_NAME(AnimScale, "AnimScale")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(AnimScale, Scale);

		RTTR_REGISTER_WITH_NAME(AnimNode, "AnimNode").constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(AnimNode, Transform)
			RTTR_REGISTER_PROPERTY(AnimNode, boneOffset)
			RTTR_REGISTER_PROPERTY(AnimNode, Child)
			RTTR_REGISTER_PROPERTY(AnimNode, ParentNode)
			RTTR_REGISTER_PROPERTY(AnimNode, BoneID);

		RTTR_REGISTER_WITH_NAME(AnimationNodes, "AnimationNodes")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(AnimationNodes, ModelNode)
			RTTR_REGISTER_PROPERTY(AnimationNodes, positions)
			RTTR_REGISTER_PROPERTY(AnimationNodes, rotationsQ)
			RTTR_REGISTER_PROPERTY(AnimationNodes, scalings);


		RTTR_REGISTER_WITH_NAME(Animation, "Animation").constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(Animation, name)
			RTTR_REGISTER_PROPERTY(Animation, duration)
			RTTR_REGISTER_PROPERTY(Animation, ticks)
			RTTR_REGISTER_PROPERTY(Animation, nodes);


		RTTR_REGISTER_WITH_NAME(AnimationData, "AnimationData").constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(AnimationData, Animations)
			RTTR_REGISTER_PROPERTY(AnimationData, Bones);
			//RTTR_REGISTER_PROPERTY(AnimationData, BoneMap);

		RTTR_REGISTER_WITH_NAME(BonelessAnimationNodes, "BonelessAnimationNodes").constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(BonelessAnimationNodes, name)
			RTTR_REGISTER_PROPERTY(BonelessAnimationNodes, positions)
			RTTR_REGISTER_PROPERTY(BonelessAnimationNodes, rotationsQ)
			RTTR_REGISTER_PROPERTY(BonelessAnimationNodes, scalings);


		RTTR_REGISTER_WITH_NAME(BonelessAnimation, "BonelessAnimation").constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(BonelessAnimation, duration)
			RTTR_REGISTER_PROPERTY(BonelessAnimation, ticksPerSecond)
			RTTR_REGISTER_PROPERTY(BonelessAnimation, channels);
	}
		

	void BonelessAnimationData::Serialize(BonelessAnimationData& anim, std::string_view fileName, bool read)
	{
		JSONSerializer m_JsonSerializer;

		std::string fullPath = ANIM_PATH + std::string(fileName.data());

		if (m_JsonSerializer.OpenData(fullPath, read) == JSONSerializer::ERROR_TYPE::SUCCESS)
		{
			if (read)
			{
				m_JsonSerializer.StartDeserializer(&anim);
			}
			else
			{
				m_JsonSerializer.StartSerializer(&anim);
			}
			m_JsonSerializer.CloseData(read);
		}
	}
	void AnimationData::Serialize(AnimationData& anim, std::string_view fileName, bool read)
	{
		JSONSerializer m_JsonSerializer;

		std::string fullPath = ANIM_PATH + std::string(fileName.data());

		if (m_JsonSerializer.OpenData(fullPath, read) == JSONSerializer::ERROR_TYPE::SUCCESS)
		{
			if (read)
			{
				m_JsonSerializer.StartDeserializer(&anim);
			}
			else
			{
				m_JsonSerializer.StartSerializer(&anim);
			}
			m_JsonSerializer.CloseData(read);
		}
	}
}