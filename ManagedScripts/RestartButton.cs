/*!*************************************************************************
****
\file RestartButton.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 20-1-2024
\brief  Script for pause menu restart game button
****************************************************************************
***/
using System;
using ScriptAPI;

public class RestartButton : Script
{
    public GameBlackboard? gameBlackboard;
    UISpriteComponent restartButtonSprite;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";
    private bool firstHover = true;
    GameObject arrowObject;
    public override void Awake()
    {
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();
        arrowObject = GameObjectScriptFind("RestartArrow");
    }

    public override void Start()
    {
        restartButtonSprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Update()
    {
        //if (PopupUI.isDisplayed)
        if (gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            restartButtonSprite.SetEnabled(true);
        }
        else
        {
            restartButtonSprite.SetEnabled(false);
        }

        if (restartButtonSprite.IsMouseCollided() && firstHover && (gameBlackboard.gameState == GameBlackboard.GameState.Paused))
        {
            firstHover = false;
            buttonSfx.play("buttonhover");
            arrowObject.SetActive(true);
        }
        if (!restartButtonSprite.IsMouseCollided())
        {
            firstHover = true;
            arrowObject.SetActive(false);
        }

        if (Input.GetMouseButtonDown(Keycode.M1) && restartButtonSprite.IsMouseCollided() && gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            buttonSfx.play(buttonSfxName);
            SceneLoader.LoadMainGame();
        }
    }
}