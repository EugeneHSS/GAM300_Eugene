/*!*************************************************************************
****
\file ReturnToGame.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 20-1-2024
\brief  Script for pause menu continue game button
****************************************************************************
***/
using System;
using System.Numerics;
using ScriptAPI;

public class ReturnToGame : Script
{
    public GameBlackboard? gameBlackboard;
    UISpriteComponent buttonSprite;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";
    private bool firstHover = true;
    GameObject arrowObject;
    public override void Awake()
    {
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();
        arrowObject = GameObjectScriptFind("ContinueArrow");
    }

    public override void Start()
    {
        buttonSprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Update()
    {
        if(gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            buttonSprite.SetEnabled(true);
        }
        else
        {
            buttonSprite.SetEnabled(false);
        }

        if (buttonSprite.IsMouseCollided() && firstHover && (gameBlackboard.gameState == GameBlackboard.GameState.Paused))
        {
            firstHover = false;
            buttonSfx.play("buttonhover");
            arrowObject.SetActive(true);
        }
        if (!buttonSprite.IsMouseCollided())
        {
            firstHover = true;
            arrowObject.SetActive(false);
        }

        if (Input.GetMouseButtonDown(Keycode.M1) && buttonSprite.IsMouseCollided() && gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            //Console.WriteLine("Continue Button Pressed");
            buttonSfx.play(buttonSfxName);
            PopupUI.changeDisplayed = true;
        }
    }
}