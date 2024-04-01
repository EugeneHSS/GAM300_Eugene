#include "GameObject.hxx"
#include "EngineInterface.hxx"

namespace ScriptAPI
{
	bool GameObject::ActiveInHierarchy()
	{
		return TDS::ecs.getEntityIsEnabled(GetEntityID());
	}

	void GameObject::SetActive(bool status)
	{
		if (GetEntityID())
		{
			TDS::ecs.setEntityIsEnabled(GetEntityID(), status);
			return;
		}

		Console::WriteLine("Unreferenced Game Object");
	}

	bool GameObject::GetActive()
	{
		if (GetEntityID())
		{
			return TDS::ecs.getEntityIsEnabled(GetEntityID());
		}

		Console::WriteLine("Unreferenced Game Object");
		return false;
	}

	TDS::EntityID GameObject::GetEntityID()
	{
		return entityID;
	}

	generic <typename T>
	T GameObject::GetComponent()
	{
		System::Type^ type = T::typeid;

		if (type == BoxColliderComponent::typeid)
		{
			if (BoxColliderComponent(entityID).gameObject)
			{
				BoxColliderComponent^ boxCollider = gcnew BoxColliderComponent(entityID);
			}
			return safe_cast<T>(gcnew BoxColliderComponent(entityID));
		}
		else if (type == CameraComponent::typeid)
		{
			return safe_cast<T>(CameraComponent(entityID));
		}
		else if (type == CapsuleColliderComponent::typeid)
		{
			if (CapsuleColliderComponent(entityID).gameObject)
			{
				CapsuleColliderComponent^ boxCollider = gcnew CapsuleColliderComponent(entityID);
			}
			return safe_cast<T>(gcnew CapsuleColliderComponent(entityID));
		}
		else if (type == NameTagComponent::typeid)
		{
			return safe_cast<T>(NameTagComponent(entityID));
		}
		else if (type == RigidBodyComponent::typeid)
		{
			return safe_cast<T>(RigidBodyComponent(entityID));
		}
		else if (type == SphereColliderComponent::typeid)
		{
			if (SphereColliderComponent(entityID).gameObject)
			{
				SphereColliderComponent^ boxCollider = gcnew SphereColliderComponent(entityID);
			}
			return safe_cast<T>(gcnew SphereColliderComponent(entityID));
		}
		else if (type == TransformComponent::typeid)
		{
			return safe_cast<T>(TransformComponent(entityID));
		}
		else if (type == UISpriteComponent::typeid)
		{
			return safe_cast<T>(UISpriteComponent(entityID));
		}
		else if (type == AudioComponent::typeid)
		{
			return safe_cast<T>(AudioComponent(entityID));
		}
		else if (type == GraphicComponent::typeid)
		{
			return safe_cast<T>(GraphicComponent(entityID));
		}
		else if (type == DirectionalLightComponent::typeid)
		{
			return safe_cast<T>(DirectionalLightComponent(entityID));
		}
		else if (type == SpotlightComponent::typeid)
		{
			return safe_cast<T>(SpotlightComponent(entityID));
		}
		else if (type == PointlightComponent::typeid)
		{
			return safe_cast<T>(PointlightComponent(entityID));
		}
		else if (type == AnimatedComponent::typeid)
		{
			return safe_cast<T>(AnimatedComponent(entityID));
		}

		//Console::WriteLine(entityID.ToString() + "\t" + type->FullName);
		return safe_cast<T>(EngineInterface::GetScriptReference(entityID, type->FullName));
	}

	GameObject::GameObject(TDS::EntityID ID) : entityID (ID)
	{ }

	void GameObject::SetEntityID(TDS::EntityID id)
	{
		entityID = id;
		transform = TransformComponent(id);
	}
}