///*!*************************************************************************
//****
//\file InGameOptions.cs
//\author Matthew Cheung
//\par DP email: j.cheung@digipen.edu
//\par Course: csd3450
//\date 6-3-2024
//\brief  Script for in game options menu
//****************************************************************************
//***/
using ScriptAPI;
using System;

public class InGameOptions : Script
{
    public GameBlackboard? gameBlackboard;
    UISpriteComponent optionsSprite;
    GameObject OptionsObject;

    public override void Awake()
    {
       
    }

    public override void Start()
    {
        optionsSprite = gameObject.GetComponent<UISpriteComponent>();
        OptionsObject = GameObjectScriptFind("OptionsMenu");
        OptionsObject.SetActive(false);
    }
    public override void Update()
    {
        if (gameBlackboard.gameState == GameBlackboard.GameState.Options)
        {
            OptionsObject.SetActive(true);
        }
        if(InGameOptionsButton.openedOptionsMenu == false)
        {
            OptionsObject.SetActive(false);
        }
    }

}
