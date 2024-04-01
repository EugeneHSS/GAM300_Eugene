#pragma once
#include "Components/BoxColliderComponent.hxx"
#include "Components/CameraComponent.hxx"
#include "Components/CapsuleColliderComponent.hxx"
#include "Components/NameTagComponent.hxx"
#include "Components/RigidBodyComponent.hxx"
#include "Components/SphereColliderComponent.hxx"
#include "Components/TransformComponent.hxx"
#include "Components/UISpriteComponent.hxx"
#include "Components/AudioComponent.hxx"
#include "Components/GraphicComponent.hxx"
#include "Components/PointlightComponent.hxx"
#include "Components/DirectionalLightComponent.hxx"
#include "Components/SpotlightComponent.hxx"
#include "Components/AnimatedComponent.hxx"
#include "ecs/ecs.h"

namespace ScriptAPI
{
    public ref class GameObject
    {
    public:
        generic <typename T>
        T GetComponent();

        bool ActiveInHierarchy();
        void SetActive(bool status);
        bool GetActive();

        TDS::EntityID GetEntityID();

        TransformComponent transform;

    internal:
        GameObject() { }
        GameObject(TDS::EntityID ID);
        void SetEntityID(TDS::EntityID ID);
    private:
        //entityID and is_Enabled set at SetEntityID
        TDS::EntityID entityID;

        System::String^ tag;
    };
}