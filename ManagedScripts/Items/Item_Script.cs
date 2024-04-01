/*!*************************************************************************
****
\file Item_Script.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script for player interaction with items
****************************************************************************
***/

using ScriptAPI;
using System;

public class Item_Script : Script
{
    private GameObject playerObject;
    //RigidBodyComponent rigidBodyComponent; //for raycast?

    AudioComponent audioPlayer;

    [SerializeField]
    public string Item_Name;
    public string Item_Texture;
    public GameObject? _InteractUI;

    public override void Awake()
    {
        audioPlayer = gameObject.GetComponent<AudioComponent>();
    }

    public override void Start()
    {
        playerObject = GameObjectScriptFind("player");
        //rigidBodyComponent = gameObject.GetComponent<RigidBodyComponent>();
    }

    public override void Update()
    {
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
        {
            InteractUI.isShow = true;
            if (Input.GetKeyDown(Keycode.E) /*&& isWithinRange() && rigidBodyComponent.IsRayHit() */) // Maybe add 1 more condition to check if its within player's view
            {
                Console.WriteLine("Picked up item");
                InventoryScript.addItemIntoInventory(Item_Name, Item_Texture);
                gameObject.SetActive(false);
            }
        }
        else
        {
            //_InteractUI.SetActive(false);
        }
    }
}