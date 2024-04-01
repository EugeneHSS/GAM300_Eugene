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

public class p03 : Script
{
    [SerializeField]
    public string Painting_Name;
    public string Painting_Texture;
    bool doOnce = true;

    public GameObject _InteractUI;

    [Header("AudioStuff")]
    public AudioComponent AudioPlayer;

    public GameObject hidingGameObject;
    public GameObject ghost;
    public static bool isPaintingCollected;

    float timer = 2.0f;
    bool paintingMoved = false;
 

    override public void Awake()
    {
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
        //if (GalleryLetter.isNotePicked) // Don't allow player to proceed with puzzle before getting the hint.
        {
            if (!isPaintingCollected && gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
            {
                //if (doOnce)
                //{
                //    gameObject.GetComponent<AudioComponent>().play("pc_movethissilently");
                //    GameplaySubtitles.counter = 46;
                //    doOnce = false;
                //}
                Console.WriteLine("p03");
                InteractUI.isShow = true;
                if (Input.GetKeyDown(Keycode.E))
                {
                    Console.WriteLine("Picked up p03");
                    isPaintingCollected = true;

                    // Trigger Painting Event
                    AudioPlayer.play("gallery_movepainting");
                    AudioPlayer.play("mon_patrol4");

                    GameplaySubtitles.counter = 8;

                    // hiding event 
                    /*GalleryHiding.GhostShouldMove = true;
                    GalleryHiding.timeLimit = 10.0f;
                    GalleryHiding.GhostMoved = false;*/

                    // hiding event 
                    hidingGameObject.GetComponent<EventGalleryHiding>().GhostShouldMove = true;
                    hidingGameObject.GetComponent<EventGalleryHiding>().timeLimit = 10.0f;
                    hidingGameObject.GetComponent<EventGalleryHiding>().GhostMoved = false;

                    ghost.GetComponent<GhostMovement>().PlayMonsterWalkingSoundInitial();
                    paintingMoved = false;
                    timer = 2.0f;
                }
            }
            else
            {
                //_InteractUI.SetActive(false);
            }
            if (isPaintingCollected && !paintingMoved)
            {
                if (timer >= 0.0f)
                {
                    Console.WriteLine("Moving p03");
                    //gameObject.transform.SetPosition(gameObject.transform.GetPosition() + gameObject.transform.getRightVector() * 50.0f * Time.deltaTime); // Right Vector is moving backwards instead
                    gameObject.transform.SetPosition(gameObject.transform.GetPosition() + new ScriptAPI.Vector3(75, 0, 0) * Time.deltaTime);
                    timer -= Time.deltaTime;
                }
                else
                {
                    Console.WriteLine("Moving done p03");
                    paintingMoved = true;
                }
            }
        }
    }
}
