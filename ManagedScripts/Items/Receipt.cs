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

public class Receipt : Script
{
    //RigidBodyComponent rigidBodyComponent; //for raycast?
    AudioComponent clip;
    bool once = true;

    [SerializeField]
    public string Note_Name;
    public string Note_Texture;
    public GameObject? _InteractUI;
    public static bool isNotePicked = false;

    public override void Awake()
    {
        once = true;
        isNotePicked = false;
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
            Console.WriteLine("Receipt");
            InteractUI.isShow = true;
            if (Input.GetKeyDown(Keycode.E))
            {
                Console.WriteLine("Picked up receipt");
                InventoryScript.addNoteIntoInventory(Note_Name, Note_Texture);

                gameObject.GetComponent<GraphicComponent>().SetView2D(true);
                gameObject.transform.SetPosition(new Vector3(-10000.0f, -10000.0f, -10000.0f));
                gameObject.transform.SetRotation(new Vector3(-0.0f, -0.0f, -0.0f));
                gameObject.SetActive(false);

                clip.play("pc_checkreceipt");
                GameplaySubtitles.counter = 14;
                isNotePicked = true;
            }
        }
        else
        {
            //_InteractUI.SetActive(false);
        }
    }
}