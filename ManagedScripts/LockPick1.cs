/*!*************************************************************************
****
\file LockPick1.cs
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\par Course: csd3450
\date 12-11-2023
\brief  Script lock picking gameplay
****************************************************************************
***/
using System.ComponentModel.DataAnnotations;
using ScriptAPI;

public class LockPick1 : Script
{
    public GameBlackboard? gameBlackboard;

    float toDegree(float radians)
    {
        return radians * (180 / 3.1415926535897931f);
    }
    float toRadians(float degree)
    {
        return degree * (3.1415926535897931f / 180);
    }

    //[Header("Tutorial UI Variables")]
    //public Image _TutorialImage;
    //public Sprite[] _TutorialImgSprites;
    //public Text _Press2Continue;
    //[SerializeField] int _TutorialStep;

    //public Text mySubtitles;
    //public Image mySubtitlesBG;
    String[] Subtitles;
    [SerializeField]
    public static int counter;

    public string startingVOstr;
    public string[] endVOstr;
    public bool _TutorialCompleted;

    [Header("Lockpick Variables")]
    public GameDataManager myGameDataManager;
    public GameObject Door_UI;
    public CameraComponent cam;
    public CameraComponent playerCam;
    public TransformComponent innerLock;
    public TransformComponent pickPosition;
    public GameObject playerController;
    public GameObject lockGroup;
    public string difficultyLvl;
    public float maxAngle = 90;
    public float lockSpeed = 10;
    public bool unlocked;
    //public AudioClip[] lockSoundEffects;
    //public AudioClip[] rattleSoundEffects;
    public String[] lockSoundEffects;
    public String[] rattleSoundEffects;
    float OldeulerAngleDegree;
    public static String[] playerGuideVO;
    float delay = 0.4f;
    public GameObject _NumOfTries;
    public UISpriteComponent _AmtOfTries;

    [Range(1, 25)]
    public float lockRange = 10;

    [SerializeField] private int numOfTries;
    public int savedNumOfTries = 0;
    private float percentage;
    private float eulerAngle;
    private float unlockAngle;
    private Vector2 unlockRange;
    private float keyPressTime;
    private bool movePick;
    private bool deduct;
    private bool displayTutorial;
    private bool next_VO;
    private bool pickWasCloseButYouMovedAway;
    private bool firstTimeTutorial = true;
    private bool playOnce = true;
    [SerializeField] bool played;

    private Vector3 originalPosition = new Vector3(0.0f, 350.0f, 1500.0f);
    private Vector3 originalRotation;
    public static AudioComponent audio;
    public static bool failed;
    public static bool passed;
    public static bool enteredHouse = false;
    float timer;

    public int doorIndex;
    public GameObject doorText;
    public GameObject monster;
    public GameObject doorStates;

    public float lockpickShakeTimer;
    private float lockpickTimerCountdown = 0.4f;
    private bool lockpickFlashing = false;

    // Start is called before the first frame update
    override public void Awake()
    {
        lockSoundEffects = new String[4];
        lockSoundEffects[0] = "lockpick_turn";
        lockSoundEffects[1] = "lockpick success";
        lockSoundEffects[2] = "lockpick_failtry";
        lockSoundEffects[3] = "lockpick_failfull";

        rattleSoundEffects = new String[7];
        rattleSoundEffects[0] = "lockpick_move1";
        rattleSoundEffects[1] = "lockpick_move2";
        rattleSoundEffects[2] = "lockpick_move3";
        rattleSoundEffects[3] = "lockpick_move4";
        rattleSoundEffects[4] = "lockpick_move5";
        rattleSoundEffects[5] = "lockpick_move6";
        rattleSoundEffects[6] = "lockpick_move7";
        OldeulerAngleDegree = 0f;

        playerGuideVO = new String[7];
        playerGuideVO[0] = "pc_lockpickstart";
        playerGuideVO[1] = "pc_lockpicksuccess1";
        playerGuideVO[2] = "pc_lockpicksuccess2";
        playerGuideVO[3] = "pc_lockpickfail";
        playerGuideVO[4] = "pc_findtherightspot";
        playerGuideVO[5] = "pc_turnthelock";
        playerGuideVO[6] = "";

        Subtitles = new String[9];
        Subtitles[0] = "Hopefully, I won\'t forget how to do this.";
        Subtitles[1] = "Alright, looks like I\'m in.";
        Subtitles[2] = "No turning back now.";
        Subtitles[3] = "That was too loud... I better not screw up again.";
        Subtitles[4] = "";
        Subtitles[5] = "Move [mouse] to adjust pick & Hold [E] to turn lock";
        Subtitles[6] = "Move [mouse] to adjust pick & Hold [E] to turn lock";
        Subtitles[7] = "Find the right spot, and it should click...";
        Subtitles[8] = "There, now to turn the lock.";

        counter = 0;
        savedNumOfTries = 0;
        audio = gameObject.GetComponent<AudioComponent>();
        playOnce = true;
        next_VO = true;
        // GameplaySubtitles.counter = 5; //no effect on set gameplay subtitles to be empty

        newLock();
    }

    public override void Start()
    {
        //audio.play(startingVOstr);
        movePick = false;
        next_VO = true;
        pickWasCloseButYouMovedAway = false;
    }

    // Update is called once per frame
    override public void Update()
    {
        //Input.Lock(false);
        doorText.SetActive(false);

        if (Input.GetKeyDown(Keycode.ESC))
        {
            playerController.SetActive(true);
            gameBlackboard.gameState = GameBlackboard.GameState.InGame;
            //Input.Lock(true);
            //    playerCam.SetEnabled(true);
            //    Door_UI.SetActive(false);
            lockGroup.SetActive(false);
            GraphicsManagerWrapper.ToggleViewFrom2D(false);
            doorStates.SetActive(true);

            savedNumOfTries = numOfTries;
        }

        if (Input.GetKeyDown(Keycode.L))
        {
            //cheatcode for presentation
            timer = 0.0f;
            passed = true;
        }

        //if (!_TutorialCompleted)
        //{
        //    // NOTE: Audio
        //    //if (!myVOSource.isPlaying() && !played)
        //    //{
        //    //    myVOSource.Play();
        //    //    mySubtitlesBG.enabled = true;
        //    //    mySubtitles.text = "Martin (Internal): Hopefully, I won’t forget how to do this.";
        //    //    played = true;

        //    //}
        //    //else if (!myVOSource.isPlaying() && played)
        //    //{
        //    //    mySubtitles.text = "";
        //    //    mySubtitlesBG.enabled = false;
        //    //    _NumOfTries.SetActive(true);
        //    //    movePick = true;
        //    //    _TutorialCompleted = true;
        //    //}
        //}
        //else
        //{
        //_NumOfTries.SetActive(true);

        //subtitles
        UISpriteComponent Sprite = GameObjectScriptFind("VOSubtitles").GetComponent<UISpriteComponent>();
        UISpriteComponent ClosedSub = GameObjectScriptFind("Subtitles").GetComponent<UISpriteComponent>();
        //UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();

        if (counter < 5 && next_VO && playOnce)
        {            
            audio.Queue(playerGuideVO[counter]);
            next_VO = false;
        }
        if (audio.finished(playerGuideVO[0]))
            counter = 6;

        #region Move Pick
        float eulerAngleDegree = toDegree(eulerAngle);
        percentage = Mathf.Round(100 - Mathf.Abs(((eulerAngleDegree - unlockAngle) / 100) * 100));
        float maxRotation = (percentage / 100) * maxAngle;
        float lockRotation = maxRotation * keyPressTime;

        if (movePick)
        {
            //Vector3 dir = Input.mousePosition - cam.WorldToScreenPoint(transform.position);
            Vector3 dir = Input.GetLocalMousePos() - new Vector3(Screen.width / 2, Screen.height / 2, 0);  //cam.WorldToScreenPoint(transform.GetPosition());

            //eulerAngle = Vector3.Angle(dir, Vector3.Up());
            eulerAngle = Vector3.Angle(dir, Vector3.Down());

            Vector3 cross = Vector3.Cross(Vector3.Up(), dir);
            if (cross.Z < 0) { eulerAngle = -eulerAngle; }
            eulerAngle = Mathf.Clamp(eulerAngle, toRadians(-maxAngle), toRadians(maxAngle));

            Vector3 originalRotation = transform.GetRotation();
            transform.SetRotation(new Vector3(originalRotation.X, originalRotation.Y, -eulerAngle));

            Vector2 newPosition = new Vector2(originalPosition.X * Mathf.Cos(-eulerAngle) - originalPosition.Y * Mathf.Sin(-eulerAngle),
                                                originalPosition.X * Mathf.Sin(-eulerAngle) + originalPosition.Y * Mathf.Cos(-eulerAngle));
            transform.SetPosition(new Vector3(newPosition.X, newPosition.Y, originalPosition.Z));


            if (eulerAngleDegree < unlockRange.Y && eulerAngleDegree > unlockRange.X)
            {
                if (lockpickShakeTimer > 0)
                {
                    // Shake the lockpick for a while if its in the correct range
                    float randomRotation = ScriptAPI.Random.Range(-2.0f, 2.0f);
                    transform.SetRotationZ(transform.GetRotation().Z + (float)(randomRotation / 180.0 * Math.PI));
                    lockpickShakeTimer -= Time.deltaTime;
                }
            }
            else
            {
                lockpickShakeTimer = 1.0f;
            }

            //do rattling sounds based on pick angle
            #region Rattle Sounds

            //note on ranges (the wider the range in if statement, the further the pick is)
            // STRICTEST if statement first (small range to wide range)
            //perfect! 1 is when eulerAngleDegree < unlockRange.Y && eulerAngleDegree > unlockRange.X
            //VERY close! 2 is when eulerAngleDegree < unlockRange.Y + 10 && eulerAngleDegree > unlockRange.X -10
            //almost close 3 is when eulerAngleDegree < unlockRange.Y + 20 && eulerAngleDegree > unlockRange.X - 20
            //close 4 is when eulerAngleDegree < unlockRange.Y + 30 && eulerAngleDegree > unlockRange.X - 30
            //not very close 5 is when eulerAngleDegree < unlockRange.Y + 40 && eulerAngleDegree > unlockRange.X - 40
            //not close 6 is when eulerAngleDegree < unlockRange.Y + 50 && eulerAngleDegree > unlockRange.X - 50
            //7 is if u were in 2 previously but are in range 3 to 6 now
            //(else) no sound when none of the cases above

            //range 1
            if (eulerAngleDegree < unlockRange.Y && eulerAngleDegree > unlockRange.X)
            {
                if (eulerAngleDegree != OldeulerAngleDegree)
                {
                    audio.play(rattleSoundEffects[0]);
                    OldeulerAngleDegree = eulerAngleDegree;
                }

                //play VO
                if (playOnce && audio.finished("pc_findtherightspot") && audio.finished(playerGuideVO[0]) && !passed)
                {
                    audio.play("pc_turnthelock");
                    
                    playOnce = false;
                }
                if (audio.finished("pc_turnthelock"))
                {
                    audio.stop("pc_turnthelock");
                }

                pickWasCloseButYouMovedAway = true;
            }
            //range 2
            else if (eulerAngleDegree < unlockRange.Y + 15 && eulerAngleDegree > unlockRange.X - 15)
            {
                if (eulerAngleDegree != OldeulerAngleDegree)
                {
                    audio.play(rattleSoundEffects[1]);
                    OldeulerAngleDegree = eulerAngleDegree;
                }
            }
            //range 3
            else if (eulerAngleDegree < unlockRange.Y + 30 && eulerAngleDegree > unlockRange.X - 30)
            {
                if (eulerAngleDegree != OldeulerAngleDegree)
                {
                    audio.play(rattleSoundEffects[2]);
                    OldeulerAngleDegree = eulerAngleDegree;
                }
            }
            //range 4
            else if (eulerAngleDegree < unlockRange.Y + 45 && eulerAngleDegree > unlockRange.X - 45)
            {
                if (eulerAngleDegree != OldeulerAngleDegree)
                {
                    audio.play(rattleSoundEffects[3]);
                    OldeulerAngleDegree = eulerAngleDegree;
                }
            }
            //range 5
            else if (eulerAngleDegree < unlockRange.Y + 60 && eulerAngleDegree > unlockRange.X - 60)
            {
                if (eulerAngleDegree != OldeulerAngleDegree)
                {
                    audio.play(rattleSoundEffects[4]);
                    OldeulerAngleDegree = eulerAngleDegree;
                }
            }
            //range 6
            else if (eulerAngleDegree < unlockRange.Y + 75 && eulerAngleDegree > unlockRange.X - 75)
            {
                if (eulerAngleDegree != OldeulerAngleDegree)
                {
                    audio.play(rattleSoundEffects[5]);
                    OldeulerAngleDegree = eulerAngleDegree;
                }
            }
            //range 7
            else if (pickWasCloseButYouMovedAway) 
            {
                if (eulerAngleDegree != OldeulerAngleDegree)
                {
                    audio.play(rattleSoundEffects[6]);
                    OldeulerAngleDegree = eulerAngleDegree;
                }
                if (audio.finished(rattleSoundEffects[6]) && audio.finished(playerGuideVO[0]) && audio.finished("pc_turnthelock") && !passed) //prevent multiple VO playing
                {
                    audio.stop(rattleSoundEffects[0]);

                    audio.play("pc_findtherightspot");
                }
            }

            if (audio.finished("pc_findtherightspot"))
            {
                audio.stop("pc_findtherightspot");
            }

            #endregion
        }

        if (!failed && !passed && Input.GetKeyDown(Keycode.E)) //lock turns
        {
            originalRotation = transform.GetRotation();
            movePick = false;
            keyPressTime = 1;
            audio.play(lockSoundEffects[0]); //lock turning sound

        }
        if (Input.GetKey(Keycode.E))
        {
            counter = 5; //"Move [mouse] to adjust pick";
            next_VO = true;

        }
        if (Input.GetKeyUp(Keycode.E)) //lock not turning
        {
            movePick = true;
            keyPressTime = 0;
            deduct = true;
            if (audio.finished(playerGuideVO[0]))
            {
                //;
                //wait for "Hopefully I won't forget how to 
                //do this".. to finish playing before showing ui instructions
                counter = 6; //"Hold [E] to turn lock";
                next_VO = true;
            }
        }
        #endregion

        #region Check if pick is at correct position
        float lockLerp = Mathf.LerpAngle(toDegree(innerLock.GetRotation().Z), lockRotation, Time.deltaTime * lockSpeed);
        innerLock.SetRotation(new Vector3(0, 0, toRadians(lockLerp)));

        if (!movePick && (lockLerp >= maxRotation - 1) && !failed)
        {
            // If you pick correct
            if (eulerAngleDegree < unlockRange.Y && eulerAngleDegree > unlockRange.X)
            {
                audio.stop(lockSoundEffects[0]);
                movePick = true;
                keyPressTime = 0;
                // NOTE: Audio
                audio.play(lockSoundEffects[1]);
                timer = 1.2f;
                passed = true;

            }
            // You pick but it wasnt correct
            else
            {
                // Shake inner lock depending on how far away the correct angle is
                float randomRotation = ScriptAPI.Random.Range(-1.5f, 1.5f) * ((unlockAngle - eulerAngleDegree) / unlockAngle); // NOTE: Not sure if insideUnitCircle keeps changing or is a fixed Vector2 that is reset on Start
                innerLock.SetRotationZ(toRadians(lockLerp + randomRotation));

                if (deduct == true)
                {
                    numOfTries -= 1;
                    audio.play(lockSoundEffects[2]);
                    deduct = false;
                    lockpickFlashing = true;
                }


                if (numOfTries <= 0)
                {
                    // NOTE: Audio
                    audio.play(lockSoundEffects[3]);
                    movePick = false;
                    timer = 1.0f;
                    failed = true;
                }
            }
        }
        #endregion

        _AmtOfTries.SetFontMessage("Number of tries left: " + numOfTries.ToString());
        if (lockpickFlashing)
        {
            lockpickTimerCountdown -= Time.deltaTime;
            _AmtOfTries.SetFontColour(new Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // red
            if (lockpickTimerCountdown <= 0)
            {
                lockpickFlashing = false;
                lockpickTimerCountdown = 0.4f;
                _AmtOfTries.SetFontColour(new Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // white
            }
        }
        if (numOfTries <= 1)
        {
            _AmtOfTries.SetFontColour(new Vector4(1.0f, 0.0f, 0.0f, 1.0f)); // red
        }

        if (passed)
        {
            //audio.FadeOut(3, "outside_ambience");
            movePick = false;
            audio.stop(rattleSoundEffects[0]);
            counter = 1;
            firstTimeTutorial = false;
            if (timer <= 0 && audio.finished(playerGuideVO[1]))
            {
                playerController.SetActive(true);
                Vector3 rotation = playerController.transform.GetRotation();
                Quaternion quat = new Quaternion(rotation);
                Vector3 rotationToVector = new Vector3(-Mathf.Sin(toRadians(rotation.Y)), 0.0f, Mathf.Cos(toRadians(rotation.Y))) * 200;
                playerController.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(playerController.transform.GetPosition() + rotationToVector, new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(1, 1, 1).Normalize(), new Vector3(1, 1, 1).Normalize());

                gameBlackboard.gameState = GameBlackboard.GameState.InGame;
                //Input.Lock(true);
                //    playerCam.SetEnabled(true);
                //    Door_UI.SetActive(false);
                unlocked = true;
                doorStates.SetActive(true);
                doorStates.GetComponent<DoorState>().Doors[doorIndex] = DoorState.State.Unlocked;
                lockGroup.SetActive(false);
                GraphicsManagerWrapper.ToggleViewFrom2D(false);
                savedNumOfTries = 0;

                //no turning back now
                //ClosedSub.SetFontMessage(Subtitles[1]); no effect

                if (doorIndex == 0)
                {
                    counter = 2;
                    audio.Queue(playerGuideVO[2]); //aite looks like im in
                    next_VO = true;
                    GameplaySubtitles.counter = 7;
                    Flashlight_Script.batteryLife = 49;
                    enteredHouse = true;
                    playOnce = false;
                }
                if (doorIndex == 1)
                {
                    Flashlight_Script.batteryLife = 24;
                    audio.play("lightshut2");

                    GameplaySubtitles.counter = 23;
                    audio.play("pc_runningoutofjuice");

                    //"is someone here?" event:
                   // GameplaySubtitles.counter = 21;
                    //audio.play("pc_approachbedroom"); //placeholder
                }

                if (doorIndex == 3)
                {
                    //bathroom is near

                }
                if (doorIndex == 4) //you are just outside bathroom
                {
                    

                }
                if (doorIndex == 5)
                {
                    //audio.play("shower_running");
                    //audio.play("drowningloop");
                    //audio.play("splashing");
                }

                // if (audio.finished(playerGuideVO[2]))
                // {
                //     GameplaySubtitles.counter = 5; //no effect
                // }


                // if (audio.finished(playerGuideVO[1])) //also no effect, doont do this
                // {
                //     audio.stop(playerGuideVO[1]);
                //     counter = 2;
                //     audio.play(playerGuideVO[2]);

                // }

            }
            else
            {
                timer -= Time.deltaTime;
            }
        }

        if (failed)
        {
            movePick = false;
            counter = 3;
            if (timer <= 0 && audio.finished(playerGuideVO[3]))
            {
                //audio.stop(playerGuideVO[3]);
                playerController.SetActive(true);
                gameBlackboard.gameState = GameBlackboard.GameState.InGame;
                //Input.Lock(true);
                //    playerCam.SetEnabled(true);
                //    Door_UI.SetActive(false);
                lockGroup.SetActive(false);
                GraphicsManagerWrapper.ToggleViewFrom2D(false);
                doorStates.SetActive(true);
                savedNumOfTries = 0;

                if (doorIndex != 0)
                {
                    monster.GetComponent<GhostMovement>().AlertMonster();
                }
                counter = 5; //move mouse to adjust pick
                next_VO = true;
            }
            else
            {
                timer -= Time.deltaTime;
            }
        }

        Sprite.SetFontMessage(Subtitles[counter]); //update last
    }

    public void newLock()
    {
        playerController.GetComponent<RigidBodyComponent>().SetLinearVelocity(new Vector3(0.0f, 0.0f, 0.0f));

        gameBlackboard.gameState = GameBlackboard.GameState.Lockpicking;
        originalPosition = new Vector3(0.0f, 600.0f, 2500.0f);
        originalRotation = transform.GetRotation();

        audio.stop(lockSoundEffects[1]);
        audio.stop(lockSoundEffects[2]);
        //popupUI.GetComponent<PopupUI>().lockpickDisplayed = true;
        //Input.Lock(false);

        failed = false;
        passed = false;

        //Door_UI.SetActive(true);
        _AmtOfTries.SetFontColour(new Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // white
        keyPressTime = 0;
        unlocked = false;
        deduct = true;
        //playerController.SetEnabled(false);
        //playerCam.SetEnabled(false);
        //cam.transform.SetRotation(new Vector3(0, 0, 0));
        unlockAngle = ScriptAPI.Random.Range(-maxAngle + lockRange, maxAngle - lockRange);
        unlockRange = new Vector2(unlockAngle - lockRange, unlockAngle + lockRange);

        if (savedNumOfTries == 0)
        {
            numOfTries = 5;
            if (difficultyLvl == "Easy")
            {
                numOfTries = 10;
            }
            else if (difficultyLvl == "Normal")
            {
                numOfTries = 5;
            }
            else if (difficultyLvl == "Hard")
            {
                numOfTries = 3;
            }
        }
        else
        {
            numOfTries = savedNumOfTries;
        }

        if (_TutorialCompleted)
        {
            movePick = true;
        }
        //start with subtitle press E to turn lock

        lockpickShakeTimer = 1.0f;
    }
}
