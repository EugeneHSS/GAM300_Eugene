/*!*************************************************************************
****
\file Painting_Script.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script for player interaction with paintings
****************************************************************************
***/
using Microsoft.VisualBasic;
using ScriptAPI;
using System;

// Bedroom painting

public class p07 : Script
{
    //RigidBodyComponent rigidBodyComponent; //for raycast?
    [SerializeField]
    public string Painting_Name;
    public string Painting_Texture;
    public bool opened;
    public GameObject _InteractUI;

    //public Animator _PaintingAnimator;
    //public Flashlight_Script _FlashlightScript;
    //private GraphicComponent _color;

    [Header("AudioStuff")]
    public AudioComponent AudioPlayer;

    public GameObject hidingGameObject;
    public GameObject ghost;
    public static bool isPaintingCollected;
    bool once = true;
    bool onceAgain = true;

    public Checkpoint checkpoint;

    override public void Awake()
    {
        checkpoint = GameObjectScriptFind("Checkpoint").GetComponent<Checkpoint>();
        AudioPlayer = gameObject.GetComponent<AudioComponent>();
        isPaintingCollected = false;
    }

    public override void Start()
    {
        //rigidBodyComponent = gameObject.GetComponent<RigidBodyComponent>();
    }

    // Update is called once per frame
    override public void Update()
    {
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
        {
            Console.WriteLine("p07");
            InteractUI.isShow = true;

            if (once && Receipt.isNotePicked)
            {
               
                AudioPlayer.play("pc_shinelightafterreceipt"); // Looks like the receipt was right.
                GameplaySubtitles.counter = 40;
                once = false;

                checkpoint.OverrideCheckpoint(GhostMovement.GhostEvent.Nothing);
            }
            else if (onceAgain && !Receipt.isNotePicked)
            {
                AudioPlayer.play("pc_shinelightbeforereceipt"); // Looks like the receipt was right.
                GameplaySubtitles.counter = 20;
                onceAgain = false;
            }


            if (Input.GetKeyDown(Keycode.E))
            {
                Hiding.playOnce = false;
                Console.WriteLine("Picked up p07");
                isPaintingCollected = true;
                InventoryScript.addPaintingIntoInventory(Painting_Name, Painting_Texture);

                // View Object Stuff
                gameObject.GetComponent<GraphicComponent>().SetView2D(true);
                //gameObject.transform.SetPosition(new Vector3(-10000.0f, -10000.0f, -10000.0f));
                //gameObject.transform.SetRotation(new Vector3(-0.0f, -0.0f, -0.0f));
                gameObject.SetActive(false);

                // Trigger Painting Event
                AudioPlayer.play("gallery_movepainting");
                AudioPlayer.play("pc_stealpainting1");
                AudioPlayer.set3DCoords(AudioPlayer.getListenerPos(), "pc_stealpainting1");
                GameplaySubtitles.counter = 13;

                // hiding event 
                //hidingGameObject.GetComponent<EventBedroomHiding>().numOfPaintingsTook++;
                //if (hidingGameObject.GetComponent<EventBedroomHiding>().numOfPaintingsTook == 1)
                //{
                //    ghost.GetComponent<GhostMovement>().PlayMonsterWalkingSoundInitial();
                //    ghost.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.BedroomHidingEvent;
                //    ghost.GetComponent<GhostMovement>().startEvent = true;
                //}
                ghost.GetComponent<GhostMovement>().PlayMonsterWalkingSoundInitial();
                ghost.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.BedroomHidingEvent;
                ghost.GetComponent<GhostMovement>().startEvent = true;
            }
        }
        else
        {
            //_InteractUI.SetActive(false);
        }
    }
}
