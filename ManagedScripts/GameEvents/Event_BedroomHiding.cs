/*!*************************************************************************
****
\file Hiding.cs
\author Elton Teo Zhe Wei
\par DP email: e.teo@digipen.edu
\par Course: csd3450
\date 12-12-2023
\brief  Gameplay script for player hiding from the monster
****************************************************************************
***/
using System;
using System.Reflection.Emit;
using System.Text;
using ScriptAPI;

public class EventBedroomHiding : Script
{
    public GameObject player;
    public GameObject enemyPathfinding;
    public GameObject closet;
    public bool hiding;
    [Header("AudioStuff")]
    //public AudioSource playerVOSource;
    public AudioComponent audioPlayer;  //#1
    String[] voClips;
    public String[] subtitles;
    public static bool doOnce = true;

    public int numOfPaintingsTook = 0;
    public DoorState doorState;
	
	public override void Awake()
	{
		voClips = new string[3];
        voClips[0] = "pc_hideinclosetfirst";
        voClips[1] = "pc_wanderingcloset";
        voClips[2] = "pc_monstergoesaway1"; //wth was that
        doorState = GameObjectScriptFind("DoorStates").GetComponent<DoorState>();
	}


    public override void Update()
    {
        hiding = closet.GetComponent<Hiding>().hiding;
        if (hiding)
        {
            //if (enemyPathfinding.GetComponent<GhostMovement>().bedroomHideEventDone == false && numOfPaintingsTook == 1) // triggering bedroom monster event 
            //{
            //    if (enemyPathfinding.GetComponent<GhostMovement>().currentEvent != GhostMovement.GhostEvent.BedroomHidingEvent)
            //    {
            //        enemyPathfinding.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.BedroomHidingEvent;
            //        enemyPathfinding.GetComponent<GhostMovement>().startEvent = true;
            //    }
            //}

            if (enemyPathfinding.GetComponent<GhostMovement>().bedroomHideEventDone)
            {
                if (doOnce)
                {
                    audioPlayer.play(voClips[2]);
                    GameplaySubtitles.counter = 11;
                    doOnce = false;
                }
            }
        }

        audioPlayer.set3DCoords(audioPlayer.getListenerPos(), voClips[2]);
    }
}
