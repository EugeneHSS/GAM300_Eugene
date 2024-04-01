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
using ScriptAPI;

public class EventGalleryHiding : Script
{
    public GameObject player;
    public GameObject enemyPathfinding;
    public GameObject closet;
    public bool hiding;

    int counter;
    [Header("AudioStuff")]
    //public AudioSource playerVOSource;
    public AudioComponent audioPlayer;
    String[] voClips;
    public String[] subtitles;

    public bool GhostShouldMove = false;
    public bool GhostMoved = false;

    public float timeLimit = 5.0f;

    public override void Start()
    {
        //_flashlight = player.GetComponent<Flashlight_Script>();
        GhostShouldMove = false;
        GhostMoved = false;
        timeLimit = 10.0f;
    }

    public override void Update()
    {
        hiding = closet.GetComponent<Hiding>().hiding;
        if (GhostShouldMove && !closet.GetComponent<Hiding>().hiding)
        {
            timeLimit -= Time.deltaTime;
            if (timeLimit <= 0.0f && !GhostMoved)
            {
                GhostMove();
                GhostMoved = true;
                GhostShouldMove = false;
            }
        }


        if (closet.GetComponent<Hiding>().hiding)
        {
            if (GhostShouldMove)
            {
                GhostMove();
                GhostShouldMove = false;
            }
            if (enemyPathfinding.GetComponent<GhostMovement>().galleryHideEventDone)
            {
                //audioPlayer.play("pc_monstergoesaway1");
                //GameplaySubtitles.counter = 11;
                enemyPathfinding.GetComponent<GhostMovement>().galleryHideEventDone = false; // Reset for multiple hiding
            }
        }
    }

    public void GhostMove()
    {
        if (enemyPathfinding.GetComponent<GhostMovement>().galleryHideEventDone == false)
        {
            if (enemyPathfinding.GetComponent<GhostMovement>().currentEvent != GhostMovement.GhostEvent.GalleryHidingEvent)
            {
                enemyPathfinding.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.GalleryHidingEvent;
                enemyPathfinding.GetComponent<GhostMovement>().startEvent = true;
            }
        }
    }
}
