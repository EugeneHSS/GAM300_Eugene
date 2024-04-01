/*!*************************************************************************
****
\file MainMenuCredits.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 25-03-2024
\brief  Script for credits page for game
****************************************************************************
***/

using ScriptAPI;
using System;

public class MainMenuCredits : Script
{
    private UISpriteComponent sprite;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";
    private bool firstHover = true;

    public override void Awake()
    {
        sprite = gameObject.GetComponent<UISpriteComponent>();
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();
    }
    public override void Update()
    {
        if (sprite.IsMouseCollided() && firstHover)
        {
            firstHover = false;
            buttonSfx.play("buttonhover");
        }
        if (!sprite.IsMouseCollided())
        {
            firstHover = true;
        }

        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            buttonSfx.play(buttonSfxName);
            SceneLoader.LoadEndGameCredits();
        }

    }
}