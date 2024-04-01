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

public class p05 : Script
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

    override public void Awake()
    {
        AudioPlayer = gameObject.GetComponent<AudioComponent>();
        isPaintingCollected = false;
    }

    public override void Start()
    {
    }

    // Update is called once per frame
    override public void Update()
    {
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
        {
            Console.WriteLine("Painting");
            InteractUI.isShow = true;

            if (Input.GetKeyDown(Keycode.E))
            {
                Hiding.playOnce = false;
                Console.WriteLine("Picked up painting");
                isPaintingCollected = true;
                InventoryScript.addPaintingIntoInventory(Painting_Name, Painting_Texture);

                // View Object Stuff
                gameObject.GetComponent<GraphicComponent>().SetView2D(true);
                gameObject.transform.SetPosition(new Vector3(-10000.0f, -10000.0f, -10000.0f));
                gameObject.transform.SetRotation(new Vector3(-0.0f, -0.0f, -0.0f));
                gameObject.SetActive(false);

                // Trigger Painting Event
                AudioPlayer.play("gallery_movepainting");

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
    }
}
