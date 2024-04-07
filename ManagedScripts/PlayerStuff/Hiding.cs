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
using ScriptAPI;

public class Hiding : Script
{
    public bool interactable = true;
    public bool hiding;
    public Vector3 hidingPos;
    public Vector3 nonHidingPos;
    public float _RotationAngle;
    public GameObject player;
    public GameObject enemyPathfinding;
    public GameObject _flashlight;
    public GameObject _InteractUI;
    public GameObject _ExitTimerUI;
    //public CheckGameState myGameState;
    public GameObject closet;
    public GameObject doorStates;
    public GameObject doorText;
    [Header("AudioStuff")]
    //public AudioSource playerVOSource;
    public AudioComponent audioPlayer;  //#1
    String[] voClips;
    public String[] subtitles;
    int counter;
    public static bool playOnce = true;
    GameObject textmachine;

    public float hidingTimer = 0.5f;
    public float maxHidingTime = 0.5f;

    private bool fadeOut = false;
    private bool fadeIn = false;
    private float originalFadeValue;
    private float fadeValueIncrement = 0.05f;

    private float originalUIScale;

    private float timer = 1.0f;

    public float turnSpeed = 0.01f;
    private bool dialogueStarted = false;


    public override void Awake()
    {
        //counter = 0;
        audioPlayer = gameObject.GetComponent<AudioComponent>();    //#2
        voClips = new string[3];
        voClips[0] = "pc_hideinclosetfirst";
        voClips[1] = "pc_wanderingcloset";
        voClips[2] = "pc_monstergoesaway2"; //i have to watch my back
        subtitles = new String[2];
        subtitles[0] = "Nothing inside";
        subtitles[1] = "But I could hide in here in case someone shows up";
        doorText = GameObjectScriptFind("DoorText");    // Hate this please change after milestone
        doorStates = GameObjectScriptFind("DoorStates");    // Hate this please change after milestone
    }

    public override void Start()
    {
        //_flashlight = player.GetComponent<Flashlight_Script>();
        hidingPos = closet.transform.GetPosition();
        _RotationAngle = 180.0f;
        originalFadeValue = GraphicsManagerWrapper.GetFadeFactor();
        originalUIScale = _ExitTimerUI.transform.GetScale().X;
        timer = 0.8f;
    }

    public override void Update()
    {
        if (dialogueStarted) { timer -= Time.deltaTime; }
        if (timer <= 0.0f && !audioPlayer.finished(voClips[0])) //make sure text goes away even when u are not looking at closet
        {
            //counter = 1;
            GameplaySubtitles.counter = 10; //but i could hide
            playOnce = false;
        }
        if (interactable && gameObject.GetComponent<RigidBodyComponent>().IsRayHit()&& !hiding)
        {
            //_InteractUI.SetActive(true);
            doorStates.GetComponent<DoorState>().doorLookedAt = true;
            //doorText.SetActive(true);
            doorText.GetComponent<UISpriteComponent>().SetFontMessage("Press E to Hide");


            if (playOnce && !p07.isPaintingCollected && timer > 0.0f)
            {
                //textmachine.SetActive(false);
                //UISpriteComponent Sprite = GameObjectScriptFind("VOSubtitles").GetComponent<UISpriteComponent>();
                dialogueStarted = true;
                
                GameplaySubtitles.counter = 9; //nothing inside
                audioPlayer.play(voClips[0]);
                
            }


            //Sprite.SetFontMessage(subtitles[counter]); //no effect



            if (Input.GetKeyDown(Keycode.E) && hiding == false) // Player hides to trigger the monster event
            {



                hiding = true;
                //interactable = false;
                nonHidingPos = player.transform.GetPosition();
                //player.transform.SetPosition(closet.transform.GetPosition());
                //counter = 1;

                Vector3 rotation = new Vector3(0, 0, 0);
                //player.transform.SetRotation(rotation);

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
                audioPlayer.play("hiding enter exit");
            }
        }
        else if (hiding)
        {
            doorText.GetComponent<UISpriteComponent>().SetFontMessage("Hold E to Leave Closet");

            //_InteractUI.SetActive(false);
            doorStates.GetComponent<DoorState>().doorLookedAt = true;

            if (Input.GetKey(Keycode.E) || Input.GetKeyDown(Keycode.E))
            {
                hidingTimer -= Time.deltaTime;
                _ExitTimerUI.SetActive(true);
                _ExitTimerUI.transform.SetScaleX((hidingTimer / maxHidingTime) * originalUIScale);
                
                if (hidingTimer <= 0.0f)
                {
                    //Console.WriteLine("There");
                    _ExitTimerUI.SetActive(false);
                    audioPlayer.play("hiding enter exit");
                    hiding = false;
                    interactable = true;
                    //player.transform.SetPosition(nonHidingPos);

                    Vector3 rotation = player.transform.GetRotation();
                    Quaternion quat = new Quaternion(rotation);
                    player.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(nonHidingPos, new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(1, 1, 1).Normalize(), new Vector3(1, 1, 1).Normalize());

                    // add if monster is finished
                    player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
                    player.GetComponent<FPS_Controller_Script>().enableHeadBob = true;
                    _flashlight.SetActive(true);

                    if (EventBedroomHiding.doOnce == false)
                    {
                        if (EventLivingRoom.doOnce) //bobo fix to check that the bedroom hiding has passed
                        {
                            audioPlayer.play("pc_monstergoesaway2");
                            GameplaySubtitles.counter = 15;

                        }// else dont keep repeating this audio for every hide closet!
                    }
                }
            }
            else
            {
                hidingTimer = maxHidingTime;
                _ExitTimerUI.SetActive(false);
            }
        }
        audioPlayer.set3DCoords(audioPlayer.getListenerPos(), "pc_monstergoesaway2");
    }

    void FadeInFadeOut()
    {
        if (fadeOut && hiding)  //Start of hiding
        {
            float fadeValue = GraphicsManagerWrapper.GetFadeFactor();
            fadeValue -= fadeValueIncrement;
            GraphicsManagerWrapper.SetFadeFactor(fadeValue);
            if (fadeValue <= 0.0f && !fadeIn)
            {
                fadeIn = true;
                fadeOut = false;
            }
        }
        if (fadeIn && hiding) //In the Closet
        {
            float fadeValue = GraphicsManagerWrapper.GetFadeFactor();
            fadeValue += fadeValueIncrement;
            GraphicsManagerWrapper.SetFadeFactor(fadeValue);
            if (fadeValue >= originalFadeValue + 4.0f)
            {
                fadeIn = false;
                fadeOut = false;
            }
        }
        if (hidingTimer <= 0.0f)
        {
            fadeOut = true;
        }
        if (fadeOut && !hiding)
        {
            float fadeValue = GraphicsManagerWrapper.GetFadeFactor();
            fadeValue -= fadeValueIncrement;
            GraphicsManagerWrapper.SetFadeFactor(fadeValue);
            if (fadeValue <= 0.0f && !fadeIn)
            {
                fadeIn = true;
                fadeOut = false;
            }
        }
        if (fadeIn && !hiding) //In the Closet
        {
            float fadeValue = GraphicsManagerWrapper.GetFadeFactor();
            fadeValue += fadeValueIncrement;
            GraphicsManagerWrapper.SetFadeFactor(fadeValue);
            if (fadeValue >= originalFadeValue)
            {
                fadeIn = false;
                fadeOut = false;
            }
        }
    }
}
