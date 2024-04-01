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

public class p08 : Script
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
            // AudioPlayer.play(""); // Mom's face is scratched

            //if (Input.GetKeyDown(Keycode.E))
            //{
            //    Console.WriteLine("Picked up painting");

            //    // Trigger Painting Event
            //    AudioPlayer.play("pc_movethissilently");
            //}
        }
        else
        {
            //_InteractUI.SetActive(false);
        }
    }
}
