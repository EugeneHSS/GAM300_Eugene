#include "EngineInterface.hxx"
#include "Debug.hxx"
#include "TypeConversion.hxx"
#include "HelperFunctions.hxx"
#include "Time.hxx"
#include "SceneLoader.hxx"
#include "Screen.hxx"
#include "Serialization.hxx"
#include "../GAM300Engine/Include/Timestep/Timestep.h"
using namespace System;
using namespace System::Runtime::InteropServices;
#pragma comment (lib, "GAM300Engine.lib")

namespace ScriptAPI
{
    /*!*************************************************************************
    * Loads the managed script dll and adds the scriptlist to all active entities
    ***************************************************************************/
    void EngineInterface::Init()
    {
        using namespace System::IO;
        loadContext = gcnew System::Runtime::Loader::AssemblyLoadContext(nullptr, true);

        // Load assembly
        FileStream^ managedLibFile = File::Open
        (
            "ManagedScripts.dll",
            FileMode::Open, FileAccess::Read, FileShare::Read
        );
        loadContext->LoadFromStream(managedLibFile);
        managedLibFile->Close();

        // Load Assembly
        /*System::Reflection::Assembly::LoadFrom("ManagedScripts.dll");*/

        scripts = gcnew System::Collections::Generic::SortedList<TDS::EntityID, ScriptList^>();
        gameObjectList = gcnew System::Collections::Generic::SortedList<TDS::EntityID, Tuple<System::String^, GameObject^>^>();

        Screen();
        updateScriptTypeList();
        //Input::InputSetup();
        SceneLoader::dataPath = toSystemString(TDS::GetAssetFolder());
        fixedUpdateTimer = TDS::TimeStep::GetFixedDeltaTime();
        System::Console::WriteLine("Hello Engine Interface Init!");
    }

    void EngineInterface::AddScriptList(TDS::EntityID entityID) // New entity
    {
        if (!scripts->ContainsKey(entityID))
        {
            scripts->Add(entityID, gcnew ScriptList());
            gameObjectList->Add(entityID, gcnew Tuple<String^, GameObject^>("", gcnew GameObject()));
            gameObjectList[entityID]->Item2->SetEntityID(entityID);
        }
    }

    /*!*************************************************************************
    * Adds scripts from the managedscript.dll to the entity via name
    ***************************************************************************/
    bool EngineInterface::AddScriptViaName(TDS::EntityID entityId, std::string script)
    {
        SAFE_NATIVE_CALL_BEGIN
            if (entityId == TDS::NULLENTITY)
                return false;

        String^ scriptName = toSystemString(script);

        // Remove any whitespaces
        scriptName = scriptName->Trim();

        // Look for the correct script
        System::Type^ scriptType = nullptr;
        for each (System::Type ^ type in scriptTypeList)
        {
            if (type->FullName == scriptName || type->Name == scriptName)
            {
                scriptType = type;
                break;
            }
        }

        // Failed to get any script
        if (scriptType == nullptr)
        {
            return false;
        }

        // Create the script
        // Default construct an object of the specified type. 
        // We have to use this as we can�t use the normal gcnew syntax to create it as 
        // scriptType is a variable that stores a type; it itself is not a type that we can pass to gcnew.
        Script^ script = safe_cast<Script^>(System::Activator::CreateInstance(scriptType));
        script->SetFlags();
        script->gameObject = gameObjectList[entityId]->Item2;
        script->transform = TransformComponent(entityId, script->gameObject);

        // Add script to SortedList
        scripts[entityId]->Add(script->GetType()->FullName, script);

        //currentObject = script;
        //Type^ type = script->GetType();
        //currentFieldArray = type->GetFields(BindingFlags::Public | BindingFlags::Instance | BindingFlags::NonPublic);

        return true;
        SAFE_NATIVE_CALL_END

            return false;
    }

    /*!*************************************************************************
    * Remove Scripts via name in managed script library
    ***************************************************************************/
    bool EngineInterface::RemoveScriptViaName(TDS::EntityID entityId, std::string script)
    {
        SAFE_NATIVE_CALL_BEGIN
            if (entityId == TDS::NULLENTITY)
                return false;

        String^ scriptName = toSystemString(script);

        // Remove any whitespaces
        scriptName = scriptName->Trim();

        // Look for the correct script
        //for each (Tuple<String^, Script ^>^ script in scripts[entityId])
        //{
        //    if (script->Item1 == scriptName)
        //    {
        //    }
        //}
        scripts[entityId]->Remove(scriptName);

        return true;
        SAFE_NATIVE_CALL_END

            return false;
    }

    /*!*************************************************************************
    * Updates GameObject Name
    ***************************************************************************/
    bool EngineInterface::UpdateGameObjectName(TDS::EntityID entityID, std::string newName)
    {
        SAFE_NATIVE_CALL_BEGIN
            if (gameObjectList->ContainsKey(entityID))
            {
                GameObject^ thisGameObject = gameObjectList[entityID]->Item2;
                gameObjectList[entityID] = gcnew Tuple<String^, GameObject^>(toSystemString(newName), thisGameObject);
                return true;
            }
        return false;
        SAFE_NATIVE_CALL_END
            return false;
    }

    System::Collections::Generic::SortedList<TDS::EntityID, Tuple<System::String^, GameObject^>^>^ EngineInterface::GetGameObjectList()
    {
        return gameObjectList;
    }

    /*!*************************************************************************
    * Calls all script Awake function
    ***************************************************************************/
    void EngineInterface::ExecuteAwake()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            if (scripts->ContainsKey(i))
            {
                for each (NameScriptPair ^ script in scripts[i])
                {
                    SAFE_NATIVE_CALL_BEGIN
                        if (!script->Value->getAwakeFlag())
                        {
                            script->Value->Awake();
                            script->Value->setAwakeFlag();
                        }
                    SAFE_NATIVE_CALL_END
                }
            }
        }
    }
    /*!*************************************************************************
    * Calls all script OnEnable function
    ***************************************************************************/
    void EngineInterface::ExecuteOnEnable()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            if (scripts->ContainsKey(i) && TDS::ecs.getEntityIsEnabled(i))
            {
                for each (NameScriptPair ^ script in scripts[i])
                {
                    SAFE_NATIVE_CALL_BEGIN
                        script->Value->OnEnable();
                    SAFE_NATIVE_CALL_END
                }
            }
        }
    }

    bool EngineInterface::ToggleScriptViaName(TDS::EntityID entityId, System::String^ scriptName)
    {
        SAFE_NATIVE_CALL_BEGIN
            if (entityId == TDS::NULLENTITY)
                return false;

        // Remove any whitespaces
        scriptName = scriptName->Trim();

        //// Look for the correct script
        //System::Type^ scriptType = nullptr;
        //for each (System::Type ^ type in scriptTypeList)
        //{
        //    if (type->FullName == scriptName || type->Name == scriptName)
        //    {
        //        scriptType = type;
        //        break;
        //    }
        //}

        //// Failed to get any script
        //if (scriptType == nullptr)
        //{
        //    return false;
        //}

        for each (NameScriptPair ^ script in scripts[entityId])
        {
            if (script->Key == scriptName)
            {
                script->Value->ToggleScript();
            }
        }
        return true;
        SAFE_NATIVE_CALL_END

            return false;
    }

    /*!*************************************************************************
    * Calls all script start function
    ***************************************************************************/
    void EngineInterface::ExecuteStart()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            if (scripts->ContainsKey(i) && TDS::ecs.getEntityIsEnabled(i))
            {
                for each (NameScriptPair ^ script in scripts[i])
                {
                    SAFE_NATIVE_CALL_BEGIN
                        if (!script->Value->getStartFlag() && script->Value->isScriptEnabled())
                        {
                            script->Value->Start();
                            script->Value->setStartFlag();
                        }
                    SAFE_NATIVE_CALL_END
                }
            }
        }
    }

    /*!*************************************************************************
    * Calls the update function for scripts
    ***************************************************************************/
    void EngineInterface::ExecuteUpdate()
    {
        Time::deltaTime = TDS::GetDeltaTime();
        fixedUpdateTimer -= TDS::GetDeltaTime();

        for each (auto i in TDS::ecs.getEntities())
        {
            if (scripts->ContainsKey(i) && TDS::ecs.getEntityIsEnabled(i))
            {
                for each (NameScriptPair ^ script in scripts[i])
                {
                    SAFE_NATIVE_CALL_BEGIN
                        if (script->Value->isScriptEnabled())
                        {
                            //System::Console::WriteLine(script->Key);
                            script->Value->Update();

                            if (fixedUpdateTimer <= 0)
                            {
                                script->Value->FixedUpdate();
                            }
                        }
                    SAFE_NATIVE_CALL_END
                }
            }
        }
        if (fixedUpdateTimer <= 0)
        {
            fixedUpdateTimer = TDS::TimeStep::GetFixedDeltaTime();
        }
        //Input::InputUpdate();
    }

    /*!*************************************************************************
    * Calls all script FixedUpdate function
    ***************************************************************************/
    void EngineInterface::ExecuteFixedUpdate()
    {
        mAccumulatedTime += fixedUpdateTimer;
        while (mAccumulatedTime > fixedUpdateTimer)
        {
            for each (auto i in TDS::ecs.getEntities())
            {
                if (scripts->ContainsKey(i) && TDS::ecs.getEntityIsEnabled(i))
                {
                    for each (NameScriptPair ^ script in scripts[i])
                    {
                        SAFE_NATIVE_CALL_BEGIN
                            if (script->Value->isScriptEnabled())
                            {
                                script->Value->FixedUpdate();
                            }
                        SAFE_NATIVE_CALL_END
                    }
                }
            }
            mAccumulatedTime -= fixedUpdateTimer;
        }
    }

    /*!*************************************************************************
    * Calls the late update function for scripts
    ***************************************************************************/
    void EngineInterface::ExecuteLateUpdate()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            if (scripts->ContainsKey(i) && TDS::ecs.getEntityIsEnabled(i))
            {
                for each (NameScriptPair ^ script in scripts[i])
                {
                    SAFE_NATIVE_CALL_BEGIN
                        if (script->Value->isScriptEnabled())
                        {
                            script->Value->LateUpdate();
                        }
                    SAFE_NATIVE_CALL_END
                }
            }
        }
    }

    /*!*************************************************************************
    * Calls the OnDisable function for scripts
    ***************************************************************************/
    void EngineInterface::ExecuteOnDisable()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            if (!scripts->ContainsKey(i))
            {
                continue;
            }
            for each (NameScriptPair ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->Value->OnDisable();
                SAFE_NATIVE_CALL_END
            }
        }
    }

    /*!*************************************************************************
    * Calls all script OnDestroy function
    ***************************************************************************/
    void EngineInterface::ExecuteOnDestroy()
    {
        for each (auto i in TDS::ecs.getEntities())
        {
            if (!scripts->ContainsKey(i))
            {
                continue;
            }
            for each (NameScriptPair ^ script in scripts[i])
            {
                SAFE_NATIVE_CALL_BEGIN
                    script->Value->OnDestroy();
                SAFE_NATIVE_CALL_END
            }
        }
    }
    /*!*************************************************************************
    * Reloads the script assembly
    ***************************************************************************/
    void EngineInterface::Reload()
    {
        // Clear all references to types in the script assembly we are going to unload
        scripts->Clear();
        gameObjectList->Clear();
        scriptTypeList = nullptr;
        // Unload
        loadContext->Unload();
        loadContext = nullptr;
        // Wait for unloading to finish
        System::GC::Collect();
        System::GC::WaitForPendingFinalizers();
        // Load the assembly again
        Init();
    }

    bool EngineInterface::HasScriptViaName(TDS::EntityID entityId, std::string script)
    {
        SAFE_NATIVE_CALL_BEGIN
        if (entityId == TDS::NULLENTITY)
            return false;

        System::String^ scriptName = toSystemString(script);

        // Remove any whitespaces
        scriptName = scriptName->Trim();

        // Look for the correct script
        System::Type^ scriptType = nullptr;
        for each (NameScriptPair ^ script in scripts[entityId])
        {
            if (script->Key == scriptName)
            {
                return true;
            }
        }

        // Failed to get any script
        return false;

        SAFE_NATIVE_CALL_END
    }

    std::vector<std::string> EngineInterface::GetAllScripts()
    {
        std::vector<std::string> allScripts;

        for each (System::Type ^ type in scriptTypeList)
        {
            allScripts.emplace_back(toStdString(type->FullName));
        }

        return allScripts;
    }

    bool EngineInterface::IsScriptEnabled(TDS::EntityID entityId, std::string script)
    {
        if (entityId == TDS::NULLENTITY)
        {
            return false;
        }

        auto scriptName = toSystemString(script);

        scriptName = scriptName->Trim();

        return scripts[entityId][scriptName]->isScriptEnabled();
    }

    std::vector<TDS::ScriptValues> EngineInterface::GetVariables(TDS::EntityID entityId, std::string script)
    {
        std::vector<TDS::ScriptValues> scriptValues;

        if (entityId == TDS::NULLENTITY)
        {
            return scriptValues;
        }

        // Remove any whitespaces
        auto scriptName = toSystemString(script);
        scriptName = scriptName->Trim();

        Object^ obj = scripts[entityId][scriptName];

        // Failed to get any script
        if (obj == nullptr)
        {
            return scriptValues;
        }

        Type^ type = obj->GetType();
        array<FieldInfo^>^ fields = type->GetFields(BindingFlags::Public | BindingFlags::Instance | BindingFlags::NonPublic);

        for each (FieldInfo ^ field in fields)
        {
            TDS::ScriptValues newScriptValue;

            if (field->GetValue(obj) == nullptr || field->Name == "transform")
            {
                continue;
            }
            
            if (field->Name == "is_Enabled" || field->Name == "is_Awake" || field->Name == "is_Start")
            {
                continue;
            }

            if (field->GetCustomAttributes(DontSerializeFieldAttribute::typeid, true)->Length > 0)
            {
                continue;
            }

            if (field->GetCustomAttributes(SerializeFieldAttribute::typeid, true)->Length > 0 || field->IsPublic) // Either SerializedField or public variables
            {
                newScriptValue = Serialization::GetValue(obj, field);
                scriptValues.emplace_back(newScriptValue);
            }
        }

        return scriptValues;
    }
    std::vector<TDS::ScriptValues> EngineInterface::GetVariablesEditor(TDS::EntityID entityId, std::string script)
    {
        std::vector<TDS::ScriptValues> scriptValues;

        if (entityId == TDS::NULLENTITY)
        {
            return scriptValues;
        }

        // Remove any whitespaces
        auto scriptName = toSystemString(script);
        scriptName = scriptName->Trim();

        Object^ obj = scripts[entityId][scriptName];

        // Failed to get any script
        if (obj == nullptr)
        {
            return scriptValues;
        }

        Type^ type = obj->GetType();
        array<FieldInfo^>^ fields = type->GetFields(BindingFlags::Public | BindingFlags::Instance | BindingFlags::NonPublic);

        for each (FieldInfo ^ field in fields)
        {
            TDS::ScriptValues newScriptValue;

            if (field->GetCustomAttributes(HeaderAttribute::typeid, true)->Length > 0)
            {
                auto headerName = reinterpret_cast<HeaderAttribute^>(field->GetCustomAttributes(HeaderAttribute::typeid, true)[0]);
                newScriptValue.headerString = toStdString(headerName->string);

                scriptValues.emplace_back(newScriptValue);
            }

            if (/*field->GetValue(obj) == nullptr || */field->Name == "transform")
            {
                continue;
            }

            if (field->Name == "is_Enabled" || field->Name == "is_Awake" || field->Name == "is_Start")
            {
                continue;
            }

            if ((field->GetCustomAttributes(SerializeFieldAttribute::typeid, true)->Length > 0 || field->IsPublic) && 
                field->GetCustomAttributes(HideInInspectorAttribute::typeid, true)->Length <= 0) // Either SerializedField or public variables, but not HideInInspector
            {
                newScriptValue = Serialization::GetValue(obj, field);
                scriptValues.emplace_back(newScriptValue);
            }
        }

        return scriptValues;
    }

    void EngineInterface::SetVariable(TDS::EntityID entityId, std::string script, TDS::ScriptValues variableInfo)
    {
        System::Object^ currentObject = scripts[entityId][toSystemString(script)];

        if (currentObject == nullptr)
        {
            return;
        }

        array<FieldInfo^>^ currentFieldArray = currentObject->GetType()->GetFields(BindingFlags::Public | BindingFlags::Instance | BindingFlags::NonPublic);

        if (currentFieldArray == nullptr)
        {
            return;
        }

        for each (FieldInfo ^ field in currentFieldArray)
        {
            if (field->Name == toSystemString(variableInfo.name))
            {
                if (field->Name != "is_Enabled" && field->Name != "is_Awake" && field->Name != "is_Start")
                {
                    Serialization::SetValue(currentObject, field, variableInfo);
                }

                return;
            }
        }
    }
    void EngineInterface::SetVariables(TDS::EntityID entityId, std::string script, std::vector<TDS::ScriptValues>& allVariableInfo)
    {
        System::Object^ currentObject = scripts[entityId][toSystemString(script)];

        if (currentObject == nullptr)
        {
            return;
        }

        array<FieldInfo^>^ currentFieldArray = currentObject->GetType()->GetFields(BindingFlags::Public | BindingFlags::Instance | BindingFlags::NonPublic);

        if (currentFieldArray == nullptr)
        {
            return;
        }

        int currentVariableIndex = 0;
        for each (FieldInfo ^ field in currentFieldArray)
        {
            if (field->Name == toSystemString(allVariableInfo[currentVariableIndex].name))
            {
                if (field->Name != "is_Enabled" && field->Name != "is_Awake" && field->Name != "is_Start")
                {
                    Serialization::SetValue(currentObject, field, allVariableInfo[currentVariableIndex]);
                }

                ++currentVariableIndex;
                if (currentVariableIndex == allVariableInfo.size())
                {
                    return;
                }
            }
        }
    }

    GameObject^ EngineInterface::GetGameObject(TDS::EntityID entityId)
    {
        return (entityId > 0 ? gameObjectList[entityId]->Item2 : nullptr);
    }
    Script^ EngineInterface::GetScriptReference(TDS::EntityID entityId, System::String^ script)
    {
        //for each (NameScriptPair ^ script in scripts[entityId])
        //{
        //    Console::WriteLine(script->Key);
        //}
        //Console::WriteLine("Thats all");
        return (entityId > 0 ? scripts[entityId][script] : nullptr);
    }

    void EngineInterface::RemoveEntity(TDS::EntityID entityId)
    {
        SAFE_NATIVE_CALL_BEGIN
            if (scripts->ContainsKey(entityId))
            {
                scripts->Remove(entityId);
                gameObjectList->RemoveAt(gameObjectList->IndexOfKey(entityId));
            }
        SAFE_NATIVE_CALL_END

        return;
    }

    namespace
    {
        /* Select Many */
        ref struct Pair
        {
            System::Reflection::Assembly^ assembly;
            System::Type^ type;
        };
        System::Collections::Generic::IEnumerable<System::Type^>^ selectorFunc(System::Reflection::Assembly^ assembly)
        {
            return assembly->GetExportedTypes();
        }
        Pair^ resultSelectorFunc(System::Reflection::Assembly^ assembly, System::Type^ type)
        {
            Pair^ p = gcnew Pair();
            p->assembly = assembly;
            p->type = type;
            return p;
        }
        /* Where */
        bool predicateFunc(Pair^ pair)
        {
            return pair->type->IsSubclassOf(Script::typeid) && !pair->type->IsAbstract;
        }
        /* Select */
        System::Type^ selectorFunc(Pair^ pair)
        {
            return pair->type;
        }
    }

    /*!*************************************************************************
    * Updates script type list
    ***************************************************************************/
    void EngineInterface::updateScriptTypeList()
    {
        using namespace System;
        using namespace System::Reflection;
        using namespace System::Linq;
        using namespace System::Collections::Generic;
        /* Select Many: Types in Loaded Assemblies */
        IEnumerable<Assembly^>^ assemblies = AppDomain::CurrentDomain->GetAssemblies();
        Func<Assembly^, IEnumerable<Type^>^>^ collectionSelector = gcnew Func<Assembly^, IEnumerable<Type^>^>(selectorFunc);
        Func<Assembly^, Type^, Pair^>^ resultSelector = gcnew Func<Assembly^, Type^, Pair^>(resultSelectorFunc);
        IEnumerable<Pair^>^ selectManyResult = Enumerable::SelectMany(assemblies, collectionSelector, resultSelector);
        /* Where: Are concrete Scripts */
        Func<Pair^, bool>^ predicate = gcnew Func<Pair^, bool>(predicateFunc);
        IEnumerable<Pair^>^ whereResult = Enumerable::Where(selectManyResult, predicate);
        /* Select: Select them all */
        Func<Pair^, Type^>^ selector = gcnew Func<Pair^, Type^>(selectorFunc);
        scriptTypeList = Enumerable::Select(whereResult, selector);
    }

    System::Collections::Generic::SortedList<TDS::EntityID, EngineInterface::ScriptList^>^ EngineInterface::GetScriptList()
    {
        return scripts;
    }

    // To do
    GameObject^ FindGameObjectViaName(System::String^ name)
    {
        //System::Console::WriteLine("called in engine interfacee");
        for each (auto entityNameID in EngineInterface::GetGameObjectList())
        {
            if (entityNameID.Value->Item1 == name)
            {
                return entityNameID.Value->Item2;
            }
        }

        return nullptr;
    }

    // List<GameObject^>^ FindGameObjectsViaName(System::String^ name)
    // {
        // List<GameObject^>^ list = gcnew List<GameObject^>(2048);
        // //System::Console::WriteLine("called in engine interfacee");
        // for each (auto entityNameID in EngineInterface::GetGameObjectList())
        // {
            // if (entityNameID.Value->Item1 == name)
            // {
                // list->Add(entityNameID.Value->Item2);
            // }
        // }

        // return list;
    // }


    /*!*************************************************************************
    * Calls all script OnTriggerEnter function
    ***************************************************************************/
    void EngineInterface::ExecuteOnTriggerEnter(TDS::EntityID trigger, TDS::EntityID collider)
    {
        ColliderComponent^ colliderComponent;

        if (TDS::GetBoxCollider(collider))
        {
            colliderComponent = gcnew BoxColliderComponent(collider);
        }
        else if (TDS::GetCapsuleCollider(collider))
        {
            colliderComponent = gcnew CapsuleColliderComponent(collider);
        }
        else if (TDS::GetSphereCollider(collider))
        {
            colliderComponent = gcnew SphereColliderComponent(collider);
        }

        for each (NameScriptPair ^ script in scripts[trigger])
        {
            SAFE_NATIVE_CALL_BEGIN
                script->Value->OnTriggerEnter(colliderComponent);
            SAFE_NATIVE_CALL_END
        }
    }
    /*!*************************************************************************
    * Calls all script OnTriggerEnter function
    ***************************************************************************/
    void EngineInterface::ExecuteOnTriggerStay(TDS::EntityID trigger, TDS::EntityID collider)
    {
        ColliderComponent^ colliderComponent;

        if (TDS::GetBoxCollider(collider))
        {
            colliderComponent = gcnew BoxColliderComponent(collider);
        }
        else if (TDS::GetCapsuleCollider(collider))
        {
            colliderComponent = gcnew CapsuleColliderComponent(collider);
        }
        else if (TDS::GetSphereCollider(collider))
        {
            colliderComponent = gcnew SphereColliderComponent(collider);
        }

        for each (NameScriptPair ^ script in scripts[trigger])
        {
            SAFE_NATIVE_CALL_BEGIN
                script->Value->OnTriggerStay(colliderComponent);
            SAFE_NATIVE_CALL_END
        }
    }
    /*!*************************************************************************
    * Calls all script OnTriggerEnter function
    ***************************************************************************/
    void EngineInterface::ExecuteOnTriggerExit(TDS::EntityID trigger, TDS::EntityID collider)
    {
        ColliderComponent^ colliderComponent;

        if (TDS::GetBoxCollider(collider))
        {
            colliderComponent = gcnew BoxColliderComponent(collider);
        }
        else if (TDS::GetCapsuleCollider(collider))
        {
            colliderComponent = gcnew CapsuleColliderComponent(collider);
        }
        else if (TDS::GetSphereCollider(collider))
        {
            colliderComponent = gcnew SphereColliderComponent(collider);
        }

        for each (NameScriptPair ^ script in scripts[trigger])
        {
            SAFE_NATIVE_CALL_BEGIN
                script->Value->OnTriggerExit(colliderComponent);
            SAFE_NATIVE_CALL_END
        }
    }
}