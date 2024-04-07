#pragma once

#include "TransformComponent.hxx"
#include "ComponentBase.hxx"

namespace ScriptAPI
{
	public value class PointlightComponent : ComponentBase
	{
	public:
		Vector4 GetColor();
		void SetColor(Vector4 value);

		float GetColorAlpha();
		void SetColorAlpha(float value);

		Vector4 GetAttenuation();
		void SetAttenuation(Vector4 value);

		virtual TDS::EntityID GetEntityID();
		virtual void SetEntityID(TDS::EntityID ID);

		virtual void SetEnabled(bool enabled);
		virtual bool GetEnabled();

		GameObject^ gameObject;
		TransformComponent transform;
	internal:
		PointlightComponent(TDS::EntityID ID);

	private:
		TDS::EntityID entityID;
	};


}