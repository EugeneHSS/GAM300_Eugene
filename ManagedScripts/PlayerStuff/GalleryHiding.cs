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

public class GalleryHiding : Script
{
    public bool interactable;
    public bool hiding;
    public Vector3 hidingPos;
    public Vector3 nonHidingPos;
    public float _RotationAngle;
    public GameObject player;
    public GameObject enemyPathfinding;
    public GameObject _flashlight;
    public GameObject _InteractUI;
    public GameObject closet;

    [Header("AudioStuff")]
    //public AudioSource playerVOSource;
    public AudioComponent audioPlayer;
    String[] voClips;
    public String[] subtitles;
    int counter;
    public static bool GhostShouldMove = false;
    public static bool GhostMoved = false;

    public static float timeLimit = 5.0f;

    public float hidingTimer = 0.5f;
    public float maxHidingTime = 0.5f;
    private bool fadeOut = false;
    private bool fadeIn = false;
    private float originalFadeValue;
    private float fadeValueIncrement = 0.05f;

    public override void Awake()
    {
        audioPlayer = gameObject.GetComponent<AudioComponent>();
        voClips = new string[3];
        voClips[0] = "pc_hideinclosetfirst";
        voClips[1] = "pc_wanderingcloset";
        //voClips[2] = "pc_monstergoesaway2"; //NOT i have to watch my back
        subtitles = new String[2];
        subtitles[0] = "Nothing inside";
        subtitles[1] = "But I could hide in here in case someone shows up";
    }

    public override void Start()
    {
        //_flashlight = player.GetComponent<Flashlight_Script>();
        GhostShouldMove = false;
        GhostMoved = false;
        hidingPos = closet.transform.GetPosition();
        _RotationAngle = 180.0f;
        timeLimit = 10.0f;
    }

    public override void Update()
    {
        if(GhostShouldMove && !hiding)
        {
            timeLimit -= Time.deltaTime;
            if (timeLimit <= 0.0f && !GhostMoved)
            {
                GhostMove();
                GhostMoved = true;
                GhostShouldMove = false;
            }
        }
        
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
        {
            //_InteractUI.SetActive(true);

            if (Input.GetKeyDown(Keycode.E) && hiding == false)
            {
                Hide();
                if(GhostShouldMove)
                {
                    GhostMove();
                    GhostShouldMove = false;
                }
            }
        }
        else if (hiding)
        {
            //_InteractUI.SetActive(false);

            if (Input.GetKey(Keycode.E))
            {
                hidingTimer -= Time.deltaTime;
                hiding = false;
                interactable = true;

                Vector3 rotation = player.transform.GetRotation();
                Quaternion quat = new Quaternion(rotation);
                player.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(nonHidingPos, new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(1, 1, 1).Normalize(), new Vector3(1, 1, 1).Normalize());

                // add if monster is finished
                player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
                player.GetComponent<FPS_Controller_Script>().enableHeadBob = true;
                _flashlight.SetActive(true);
                if (enemyPathfinding.GetComponent<GhostMovement>().galleryHideEventDone)
                {
                    //audioPlayer.play(voClips[2]); // soap: don't repeat this again! 
                    //GameplaySubtitles.counter = 15;
                    enemyPathfinding.GetComponent<GhostMovement>().galleryHideEventDone = false; // Reset for multiple hiding
                }
            }

        }
        else
        {
            //_InteractUI.SetActive(false);
        }
    }

    public void Hide()
    {
        hiding = true;
        interactable = false;
        nonHidingPos = player.transform.GetPosition();

        Vector3 rotation = new Vector3(0, 0, 0);
        Quaternion quat = new Quaternion(rotation);
        Vector3 newPosition = new Vector3(closet.transform.GetPosition().X, player.transform.GetPosition().Y, closet.transform.GetPosition().Z);
        player.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(newPosition, new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(0, 0, 0), new Vector3(0, 0, 0));
        player.transform.SetRotation(new Vector3(0, -_RotationAngle, 0));

        player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
        player.GetComponent<FPS_Controller_Script>().enableHeadBob = false;
        if (player.GetComponent<Flashlight_Script>().flashAudio.finished(player.GetComponent<Flashlight_Script>().flashAudiostr[0]) && player.GetComponent<Flashlight_Script>().activateLight)
        {
            player.GetComponent<Flashlight_Script>().flashAudio.play(player.GetComponent<Flashlight_Script>().flashAudiostr[1]);
        }
        player.GetComponent<Flashlight_Script>().activateLight = false;
        _flashlight.SetActive(false);
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
