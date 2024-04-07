/*!*************************************************************************
****
\file Note_Script.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script for player interaction with notes
****************************************************************************
***/
using ScriptAPI;
using System;
using System.Threading.Tasks.Dataflow;

public class GalleryLetter : Script
{
    //RigidBodyComponent rigidBodyComponent; //for raycast?
    AudioComponent clip;
    //bool once;
    String[] voClip;

    [SerializeField]
    public string Note_Name;
    public string Note_Texture;
    public GameObject? _InteractUI;
    public static bool isNotePicked = false;

    public Checkpoint checkpoint;

    public override void Awake()
    {
        checkpoint = GameObjectScriptFind("Checkpoint").GetComponent<Checkpoint>();
    }

    public override void Start()
    {
        //rigidBodyComponent = gameObject.GetComponent<RigidBodyComponent>();
        clip = gameObject.GetComponent<AudioComponent>();
    }

    public override void Update()
    {
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
        {
            Console.WriteLine("Gallery Letter");
            InteractUI.isShow = true;

            if (Input.GetKeyDown(Keycode.E))
            {
                Console.WriteLine("Picked up gallery letter");

                // Add to Inventory
                InventoryScript.addNoteIntoInventory(Note_Name, Note_Texture);

                // ViewObject Stuff
                //gameObject.GetComponent<GraphicComponent>().SetView2D(true);
                gameObject.transform.SetPosition(new Vector3(-10000.0f, -10000.0f, -10000.0f));
                gameObject.transform.SetRotation(new Vector3(-0.0f, -0.0f, -0.0f));
                gameObject.SetActive(false);

                // Trigger Note Event
                clip.play("pc_moreinthebasement");
                isNotePicked = true;
                GameplaySubtitles.counter = 41;

                checkpoint.OverrideCheckpoint(GhostMovement.GhostEvent.Nothing);
            }
        }
        else
        {
            //_InteractUI.SetActive(false);
        }
        clip.set3DCoords(clip.getListenerPos(), "pc_moreinthebasement");
    }
}