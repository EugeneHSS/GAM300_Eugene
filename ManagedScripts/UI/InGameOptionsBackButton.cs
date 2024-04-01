/*!*************************************************************************
****
\file InGameOptionsBackButton.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 7-3-2024
\brief  Script for pressing the back button in game options menu
****************************************************************************
***/
using System;
using ScriptAPI;

public class InGameOptionsBackButton : Script
{
    public GameBlackboard? gameBlackboard;
    UISpriteComponent optionsBackButtonSprite;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";
    public override void Awake()
    {
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();
    }

    public override void Start()
    {
        optionsBackButtonSprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Update()
    {
        //if (PopupUI.isDisplayed)
        if (gameBlackboard.gameState == GameBlackboard.GameState.Options)
        {
            optionsBackButtonSprite.SetEnabled(true);
        }
        else
        {
            optionsBackButtonSprite.SetEnabled(false);
        }

        if (Input.GetMouseButtonDown(Keycode.M1) && optionsBackButtonSprite.IsMouseCollided() && gameBlackboard.gameState == GameBlackboard.GameState.Options)
        {
            //Console.WriteLine("Options Back Button Pressed");
            buttonSfx.play(buttonSfxName);
            gameBlackboard.gameState = GameBlackboard.GameState.Paused;
            InGameOptionsButton.openedOptionsMenu = false;
            PopupUI.changeDisplayed = true;
        }
    }
}