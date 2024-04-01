/*!*************************************************************************
****
\file ReturnToMainMenuButton.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 20-1-2024
\brief  Script for pause menu main menu button
****************************************************************************
***/
using System;
using ScriptAPI;

public class ReturnToMainMenuButton : Script
{
    public GameBlackboard? gameBlackboard;
    UISpriteComponent returntoMMButtonSprite;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";
    private bool firstHover = true;
    GameObject arrowObject;
    public override void Awake()
    {
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();
        arrowObject = GameObjectScriptFind("MainMenuArrow");
    }

    public override void Start()
    {
        returntoMMButtonSprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Update()
    {
        //if (PopupUI.isDisplayed)
        if (gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            returntoMMButtonSprite.SetEnabled(true);
        }
        else
        {
            returntoMMButtonSprite.SetEnabled(false);
        }

        if (returntoMMButtonSprite.IsMouseCollided() && firstHover && (gameBlackboard.gameState == GameBlackboard.GameState.Paused))
        {
            firstHover = false;
            buttonSfx.play("buttonhover");
            arrowObject.SetActive(true);
        }

        if (!returntoMMButtonSprite.IsMouseCollided())
        {
            firstHover = true;
            arrowObject.SetActive(false);
        }

        if (Input.GetMouseButtonDown(Keycode.M1) && returntoMMButtonSprite.IsMouseCollided() && gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            buttonSfx.play(buttonSfxName);
            AudioComponent audio = gameObject.GetComponent<AudioComponent>();
            SceneLoader.LoadMainMenu();
        }
    }
}