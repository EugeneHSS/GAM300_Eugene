/*!*************************************************************************
****
\file MapScript.cs
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\par Course: csd3450
\date 9-2-2024
\brief  Script to toggle map
****************************************************************************
***/
using ScriptAPI;
using System;
using System.Numerics;

public class MapScript : Script
{
    public GameBlackboard? gameBlackboard;
    public GameObject player;
    public AudioComponent audio;


    public override void Awake()
    {
        gameBlackboard = GameObjectScriptFind("GameBlackboard")?.GetComponent<GameBlackboard>();
        gameObject.GetComponent<UISpriteComponent>().SetEnabled(false);
    }
    public override void Update()
    {
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();
        if (gameBlackboard != null)
        {
            if (gameBlackboard.gameState == GameBlackboard.GameState.Map && (Input.GetKeyDown(Keycode.ESC) || Input.GetKeyDown(Keycode.TAB)))
            {
                player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
                player.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
                gameBlackboard.gameState = GameBlackboard.GameState.InGame;
                audio.play("map close");
                GraphicsManagerWrapper.ToggleViewFrom2D(false);
                gameObject.GetComponent<UISpriteComponent>().SetEnabled(false);
            }
            else if (gameBlackboard.gameState == GameBlackboard.GameState.InGame && Input.GetKeyDown(Keycode.TAB))
            {
                player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
                player.GetComponent<FPS_Controller_Script>().cameraCanMove = false;
                gameBlackboard.gameState = GameBlackboard.GameState.Map;
                audio.play("map open");
                GraphicsManagerWrapper.ToggleViewFrom2D(true);
                gameObject.GetComponent<UISpriteComponent>().SetEnabled(true);
            }
        }
    }
}
