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

public class p06 : Script
{
    [SerializeField]
    public string Painting_Name;
    public string Painting_Texture;

    public GameObject _InteractUI;

    [Header("AudioStuff")]
    public AudioComponent AudioPlayer;

    public GameObject hidingGameObject;
    public GameObject ghost;
    public static bool isPaintingCollected;
    public GameObject player;

    public float TestTimer = 1.0f;
    public float position = 0.0f;
    public float positionZ = 0.0f;
    public float playerLook = 0.0f;
    public float paintingRotation = 0.0f;
    public float FlipBackTimer = 1.0f;
    public float contineuDialogue = 2.0f;
    public float dropPaintingTimer = 12.0f;
    public float endingSequenceTimer = 5.0f;
    public float transitionTimerToDropTimer = 1.0f;

    public bool takingDownPainting = false;
    public bool flippingBackPainting = false;
    public bool DropPainting = false;
    public bool endingSequence = false;

    public Checkpoint checkpoint;

    override public void Awake()
    {
        checkpoint = GameObjectScriptFind("Checkpoint").GetComponent<Checkpoint>();
        AudioPlayer = gameObject.GetComponent<AudioComponent>();
        isPaintingCollected = false;
    }

    public override void Start()
    {
    }

    // Update is called once per frame
    override public void Update()
    {
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit()&& !DropPainting)
        {
            Console.WriteLine("Painting");
            if (!isPaintingCollected)
            InteractUI.isShow = true;

            if (Input.GetKeyDown(Keycode.E) && !isPaintingCollected)
            {
                Hiding.playOnce = false;
                Console.WriteLine("Picked up painting");
                isPaintingCollected = true;
                //InventoryScript.addPaintingIntoInventory(Painting_Name, Painting_Texture);

                // View Object Stuff
                /*gameObject.GetComponent<GraphicComponent>().SetView2D(true);
                gameObject.transform.SetPosition(new Vector3(-10000.0f, -10000.0f, -10000.0f));
                gameObject.transform.SetRotation(new Vector3(-0.0f, -0.0f, -0.0f));*/
                //gameObject.SetActive(false);

                // Trigger Painting Event
                GameplaySubtitles.counter = 27; //somethings diff..
                InteractUI.isShow = false;
                AudioPlayer.play("pc_somethingdiff");

                gameObject.GetComponent<ColliderComponent>().SetEnabled(false);
                // hiding event 
                //hidingGameObject.GetComponent<Hiding>().numOfPaintingsTook++;
                //if (hidingGameObject.GetComponent<Hiding>().numOfPaintingsTook == 1)
                //{
                //    ghost.GetComponent<GhostMovement>().PlayMonsterWalkingSoundInitial();
                //}
            }

        }
        else
        {
            //_InteractUI.SetActive(false);
        }

        if (isPaintingCollected && !takingDownPainting)
        {
            player.GetComponent<FPS_Controller_Script>().cameraCanMove = false;
            player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
            player.GetComponent<RigidBodyComponent>().SetLinearVelocity(Vector3.Zero());
            if (TestTimer >= 0.0f)
            {
                position = Mathf.Lerp(gameObject.transform.GetPosition().Y, 100.0f, 1.0f * Time.deltaTime);
                positionZ = Mathf.Lerp(gameObject.transform.GetPosition().Z, gameObject.transform.GetPosition().Z + 50.0f, 1.0f * Time.deltaTime);
                playerLook = Mathf.Lerp(player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().X, 20.0f, 2.0f * Time.deltaTime);
                gameObject.transform.SetPositionY(position);
                gameObject.transform.SetPositionZ(positionZ);
                player.GetComponent<FPS_Controller_Script>().playerCamera.transform.SetRotationX(playerLook);
                TestTimer -= Time.deltaTime;
                    
            }
            else
            {
                takingDownPainting = true;
            }
        }

        if (takingDownPainting && !DropPainting)
        {
            if (FlipBackTimer >= 0.0f)
            {
                paintingRotation = Mathf.Lerp(gameObject.transform.GetRotation().Y, (float)(Math.PI + gameObject.transform.GetRotation().Y), 1.0f * Time.deltaTime);
                gameObject.transform.SetRotationY(paintingRotation);
                FlipBackTimer -= Time.deltaTime;
            }
            if (FlipBackTimer <= 0.0f)
            {
                contineuDialogue -= Time.deltaTime;
                if (contineuDialogue < 0.0f)
                {
                    paintingRotation = Mathf.Lerp(gameObject.transform.GetRotation().Y, (float)(gameObject.transform.GetRotation().Y - Math.PI) , 1.0f * Time.deltaTime);
                    gameObject.transform.SetRotationY(paintingRotation);
                    transitionTimerToDropTimer -= Time.deltaTime;
                    if (transitionTimerToDropTimer <= 0.0f)
                    {
                        DropPainting = true;
                    }
                }
            }
                
        }
        if (DropPainting)
        {
            dropPaintingTimer -= Time.deltaTime;
            if (dropPaintingTimer <= 0.0f)
            {
                endingSequenceTimer -= Time.deltaTime;
                playerLook = Mathf.Lerp(player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().X, -35.0f, 2.0f * Time.deltaTime);
                position = Mathf.Lerp(gameObject.transform.GetPosition().Y, 0.0f, 1.0f * Time.deltaTime);
                gameObject.transform.SetPositionY(position);
                player.GetComponent<FPS_Controller_Script>().playerCamera.transform.SetRotationX(playerLook);
                if (endingSequenceTimer <= 0.0f)
                {
                    checkpoint.OverrideCheckpoint(GhostMovement.GhostEvent.LivingRoomHidingEvent);
                    player.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
                    player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
                    ghost.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.LivingRoomHidingEvent;
                    ghost.GetComponent<GhostMovement>().startEvent = true;
                    endingSequence = true;
                    gameObject.SetActive(false);

                    // reset player height
                    Quaternion quat = new Quaternion(transform.GetRotation());
                    gameObject.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(
                        new Vector3(transform.GetPosition().X, 90.0f, transform.GetPosition().Z),
                        new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(0, 0, 0), new Vector3(0, 0, 0));
                }
            }
        }
    }
}
