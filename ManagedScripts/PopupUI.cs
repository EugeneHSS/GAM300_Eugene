/*!*************************************************************************
****
\file PopupUI.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 20-1-2024
\brief  Script for pause menu
****************************************************************************
***/
using ScriptAPI;
using System;

public class PopupUI : Script
{
    public GameBlackboard? gameBlackboard;

    [SerializeField]
    public static bool changeDisplayed = false;
    //public static bool isDisplayed = false;
    public bool lockpickDisplayed;
    public UISpriteComponent popUpScreen;

    private AudioComponent pauseAudio;
    private string audioOpenName = "inventory open";
    private string audioCloseName = "inventory close";

    public GameObject player;

    public override void Awake()
    {
        lockpickDisplayed = false;
        pauseAudio = gameObject.GetComponent<AudioComponent>();
    }

    public override void Start()
    {
        popUpScreen = gameObject.GetComponent<UISpriteComponent>();
    }
    public override void Update()
    {

        if (changeDisplayed || ((Input.GetKeyDown(Keycode.ESC) || Input.GetKeyDown(Keycode.P)) && 
            (  gameBlackboard.previousGameState == GameBlackboard.GameState.InGame 
            || gameBlackboard.previousGameState == GameBlackboard.GameState.Paused
            || gameBlackboard.previousGameState == GameBlackboard.GameState.Options)))
        {
            if (gameBlackboard.gameState == GameBlackboard.GameState.InGame)
            {
                popUpScreen.SetEnabled(true);
                pauseAudio.play(audioOpenName);
                player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
                player.GetComponent<FPS_Controller_Script>().cameraCanMove = false;
                gameBlackboard.gameState = GameBlackboard.GameState.Paused;
            }

            else if (gameBlackboard.gameState == GameBlackboard.GameState.Options)
            {
                popUpScreen.SetEnabled(false);
                player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
                player.GetComponent<FPS_Controller_Script>().cameraCanMove = false;
            }

            else //if in pause menu close and continue game
            {
                popUpScreen.SetEnabled(false);
                pauseAudio.play(audioCloseName);
                player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
                player.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
                gameBlackboard.gameState = GameBlackboard.GameState.InGame;
            }

            changeDisplayed = false;
            //isDisplayed = !isDisplayed;
        }

        //if (isDisplayed)
        //if (gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        //{
        //    //Console.WriteLine("game paused");
        //    popUpScreen.SetEnabled(true);
        //    player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
        //    player.GetComponent<FPS_Controller_Script>().cameraCanMove = false;

        //}
        ////else if(!InventoryScript.InventoryIsOpen && !lockpickDisplayed)
        //else
        //{
        //    //Console.WriteLine("game unpaused");
        //    popUpScreen.SetEnabled(false);
        //    //player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
        //    //player.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
        //}
    }

}
