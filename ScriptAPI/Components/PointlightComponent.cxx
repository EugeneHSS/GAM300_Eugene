#include "PointlightComponent.hxx"
#include "../TypeConversion.hxx"
#include "../EngineInterface.hxx"

namespace ScriptAPI
{
	PointlightComponent::PointlightComponent(TDS::EntityID ID) : entityID(ID), transform(TransformComponent(ID))
	{
		gameObject = EngineInterface::GetGameObject(ID);
	}


	Vector4 PointlightComponent::GetColor()
	{
		if (!TDS::GetPointLightComponent(entityID))
		{
			// throw error instead (not sure how)
			return Vector4();;
		}

		float x, y, z, w;
		TDS::GetPointLightComponent(entityID)->GetPointLightColorValues(x, y, z, w);

		return Vector4(x,y,z,w);
	}

	void PointlightComponent::SetColor(Vector4 value)
	{
		if (!TDS::GetPointLightComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}
		TDS::GetPointLightComponent(entityID)->SetPointLightColor(value.X, value.Y, value.Z, value.W);
	}

	float PointlightComponent::GetColorAlpha()
	{
		if (!TDS::GetPointLightComponent(entityID))
		{
			// throw error instead (not sure how)
			return 0.0f;
		}

		return TDS::GetPointLightComponent(entityID)->GetPointLightColorAlpha();
	}

	void PointlightComponent::SetColorAlpha(float value)
	{
		if (!TDS::GetPointLightComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}
		TDS::GetPointLightComponent(entityID)->SetPointLightColorAlpha(value);
	}

	Vector4 PointlightComponent::GetAttenuation()
	{
		if (!TDS::GetPointLightComponent(entityID))
		{
			// throw error instead (not sure how)
			return Vector4();;
		}
		float x, y, z, w;
		TDS::GetPointLightComponent(entityID)->GetPointLightAttenuation(x, y, z, w);

		return Vector4(x, y, z, w);
	}

	void PointlightComponent::SetAttenuation(Vector4 value)
	{
		if (!TDS::GetPointLightComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}
		TDS::GetPointLightComponent(entityID)->SetPointLightAttenuation(value.X, value.Y, value.Z, value.W);
	}

	TDS::EntityID PointlightComponent::GetEntityID()
	{
		return entityID;
	}

	void PointlightComponent::SetEntityID(TDS::EntityID ID)
	{
		entityID = ID;
		transform = TransformComponent(ID);
		gameObject = EngineInterface::GetGameObject(ID);
	}

	void PointlightComponent::SetEnabled(bool enabled)
	{
		TDS::setComponentIsEnable("PointLight", GetEntityID(), enabled);
	}

	bool PointlightComponent::GetEnabled()
	{
		return TDS::getComponentIsEnable("PointLight", GetEntityID());
	}

}