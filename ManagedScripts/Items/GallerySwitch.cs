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

public class GallerySwitch : Script
{
    AudioComponent audioPlayer;

    public static bool isActivated = false;
    [SerializeField]
    public GameObject? _InteractUI;
    private bool isInteractUIActive = false;
    public GameObject? hidingGameObject;
    public static bool isGallerySwitchActivated = false;
    private bool mightHaveOpened = false;
    private bool openedsomething = false;
    public override void Awake()
    {
        audioPlayer = gameObject.GetComponent<AudioComponent>();
    }

    public override void Start()
    {

    }

    public override void Update()
    {
        if (!isActivated && p02.isPaintingCollected && isInteractUIActive)
        {
            Console.WriteLine("Gallery Switch");
            InteractUI.isShow = true;

            if (Input.GetKeyDown(Keycode.E))
            {
                isActivated = true;
                isInteractUIActive = false;
                hidingGameObject.GetComponent<EventGalleryHiding>().GhostShouldMove = true;
                hidingGameObject.GetComponent<EventGalleryHiding>().timeLimit = 10.0f;
                hidingGameObject.GetComponent<EventGalleryHiding>().GhostMoved = false;
                if (GalleryLetter.isNotePicked)
                {
                    isGallerySwitchActivated = true;
                    mightHaveOpened = true;
                    audioPlayer.play("pc_mighthaveopened");
                    GameplaySubtitles.counter = 43;
                }
                else
                {
                    isGallerySwitchActivated = true;
                    openedsomething = true;
                    audioPlayer.play("pc_openedsomething");

                    GameplaySubtitles.counter = 44;
                    //gameObject.GetComponent<ColliderComponent>().SetEnabled(false);
                }
            }
        }
        if (mightHaveOpened && audioPlayer.finished("pc_mighthaveopened"))
        {
            mightHaveOpened = false;
            isGallerySwitchActivated = false;
        }
        if (openedsomething && audioPlayer.finished("pc_openedsomething"))
        {
            openedsomething = false;
            isGallerySwitchActivated = false;
        }

    }

    public override void OnTriggerEnter(ColliderComponent collider)
    {
        if (p02.isPaintingCollected)
        {
            isInteractUIActive = true;
            InteractUI.isShow = true;
        }

    }

    public override void OnTriggerExit(ColliderComponent collider)
    {
        isInteractUIActive = false;
        //InteractUI.isShow = false;
    }
}