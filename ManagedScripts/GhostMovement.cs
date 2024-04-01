/*!*************************************************************************
****
\file GhostMovement.cs
\author Thea Sea
\par DP email: thea.sea@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Script for ghost movement
****************************************************************************
***/
using ScriptAPI;

public class GhostMovement : Script
{
    public float speed;

    String[] monsterPatrol;
    String[] monsterAlert;

    public GameBlackboard gameBlackboard;

    #region Monster Sounds

    public float soundSpeed;
    public int walkingSoundCounter = -1;
    public float playSoundTimer;
    float MonsterWalkingTimer;

    public String[] walkingSounds;
    public String voiceClips;
    static public Vector3 GhostTransformPosition;

    #endregion

    #region Chasing Player Variables

    public bool playerMoved;
    public GameObject player;

    #endregion

    #region Moving To Door Variables

    private Vector2 doorPosition;

    #endregion

    #region General Event Variables

    // For initializing events 
    public bool startEvent;
    private int eventStep;


    #endregion

    #region Bedroom Hiding Event Variables

    public GameObject bedroomHidingGameObject; // (Bedroom) Hide Closet > Door_2 > Door_2
    public bool bedroomHideEventDone;
    public GameObject SHDoor; // Small Hallway Door
    private float bedroomMonsterAppearTimer;

    #endregion

    #region Living Dining Room Event Variables

    private float livingDiningRoomStandingTimer;
    public bool livingDiningRoomEventDone;

    #endregion

    #region Living Room Hiding Event

    private float livingRoomHidingTimer;
    public bool livingRoomHideEventDone;
    public GameObject livingRoomHidingGameObject;
    private float livingRoomMonsterAppearTimer;

    #endregion

    #region Dining Room Event

    private float diningRoomTimer;
    private Vector3 playerOriginalPosition;
    public bool diningRoomEventDone;

    #endregion

    #region Gallery Hiding Event

    public bool galleryCorrectPainting;
    public float galleryTimer;
    public bool galleryHideEventDone;
    public bool galleryChaseEventDone;
    public GameObject galleryHidingGameObject;

    #endregion

    #region Final Chase Event

    public bool finalChaseEventDone;
    public GameObject leftWingDoor;
    public GameObject lockGroup;

    #endregion

    public enum GhostEvent
    {
        Nothing,
        PlayingWalkingSound,
        ChasingPlayer,
        MovingToDoor,
        BedroomHidingEvent,
        LivingDiningRoomEvent,
        LivingRoomHidingEvent,
        DiningRoomEvent,
        GalleryHidingEvent,
        FinalChasingEvent
    }

    [DontSerializeField]
    [HideInInspector]
    public GhostEvent currentEvent;
    [DontSerializeField]
    [HideInInspector]
    public GhostEvent previousEvent;

    public Checkpoint checkpoint;
    public bool diningCheckpoint;
    public bool livingRoomHideEventStarted;

    public GameObject jumpscareMonster;
    public GameObject flashlight;

    public float distance;

    public override void Awake()
    {
        walkingSounds = new string[8];
        walkingSounds[0] = "mon_woodstep1";
        walkingSounds[1] = "mon_woodstep2";
        walkingSounds[2] = "mon_woodstep3";
        walkingSounds[3] = "mon_woodstep4";
        walkingSounds[4] = "mon_woodstep5";
        walkingSounds[5] = "mon_woodstep6";
        walkingSounds[6] = "mon_woodstep7";
        walkingSounds[7] = "mon_woodstep8";

        monsterPatrol = new string[8];
        monsterPatrol[0] = "mon_patrol1";
        monsterPatrol[1] = "mon_patrol2";
        monsterPatrol[2] = "mon_patrol3";
        monsterPatrol[3] = "mon_patrol4";
        monsterPatrol[4] = "mon_patrol5";
        monsterPatrol[5] = "mon_patrol6";
        monsterPatrol[6] = "mon_patrol7";
        monsterPatrol[7] = "mon_patrol8";

        monsterAlert = new string[7];
        monsterAlert[0] = "mon_alerted1";
        monsterAlert[1] = "mon_alerted2";
        monsterAlert[2] = "mon_alerted3";
        monsterAlert[3] = "mon_alerted4";
        monsterAlert[4] = "mon_alerted5";
        monsterAlert[5] = "mon_alerted6";
        monsterAlert[6] = "mon_alerted7";

        voiceClips = "pc_monstergoesaway1";
        GhostTransformPosition = new Vector3();
        MonsterWalkingTimer = 0.0f;

        speed = 3.0f;
        soundSpeed = 1.0f;
        eventStep = 0;

        playerMoved = false;
        bedroomHideEventDone = false;
        livingDiningRoomEventDone = false;
        livingRoomHideEventStarted = false;
        livingRoomHideEventDone = false;
        galleryHideEventDone = false;
        startEvent = false;

        currentEvent = GhostEvent.Nothing;

        transform.SetPositionX(-2840.0f);
        transform.SetPositionZ(-650.0f);

        checkpoint = GameObjectScriptFind("Checkpoint").GetComponent<Checkpoint>();
        jumpscareMonster = GameObjectScriptFind("jumpscareMonster");
        diningCheckpoint = false;
        distance = 40.0f;
    }
    public override void Update()
    {
        if (gameBlackboard.gameState == GameBlackboard.GameState.Paused)
        {
            return;
        }

        Vector2 ghostPosition = new Vector2(transform.GetPosition().X, transform.GetPosition().Z);
        Vector2 playerPosition = new Vector2(player.transform.GetPosition().X, player.transform.GetPosition().Z);
        GhostTransformPosition = transform.GetPosition();

        if (!diningCheckpoint && !diningRoomEventDone && 
            !livingRoomHidingGameObject.GetComponent<Hiding>().hiding &&
            player.transform.GetPosition().X <= 380.0f && livingRoomHideEventStarted) // Trigger dining room event
        {
            checkpoint.OverrideCheckpoint(GhostEvent.DiningRoomEvent);
            diningCheckpoint = true;
        }

        switch (currentEvent)
        {
            case GhostEvent.Nothing:

                gameObject.GetComponent<AnimatedComponent>().StopAnimation();

                if (livingRoomHideEventDone && !diningRoomEventDone) // Trigger dining room event
                {
                    if (player.transform.GetPosition().X <= 319.0f)
                    {
                        currentEvent = GhostEvent.DiningRoomEvent;
                        startEvent = true;
                    }
                }
                previousEvent = GhostEvent.Nothing;
                distance = 40.0f;

                break;

            case GhostEvent.PlayingWalkingSound:

                gameObject.GetComponent<AnimatedComponent>().StopAnimation();

                if (!PlayMonsterWalkingSound())
                {
                    currentEvent = GhostEvent.MovingToDoor; // done playing sound, move to door that player failed the lockpick
                }

                break;

            case GhostEvent.ChasingPlayer:

                gameObject.GetComponent<AnimatedComponent>().PlayAnimation();

                //if (!gameObject.GetComponent<RigidBodyComponent>().IsRayHit()) // If player is not in sight
                //{
                //    // NOTE: May want to add in "not hiding" condition
                //    currentEvent = GhostEvent.Nothing;
                //    break;
                //}

                if ((bedroomHidingGameObject.GetComponent<Hiding>().hiding || 
                    galleryHidingGameObject.GetComponent<Hiding>().hiding ||
                    livingRoomHidingGameObject.GetComponent<Hiding>().hiding) &&
                    !flashlight.GetComponent<Flashlight_Script>().activateLight) // NOTE: Will add in the other hiding boolean variables later
                {
                    currentEvent = previousEvent;
                    return;
                }

                // If touches, loses
                if (Vector2.Distance(ghostPosition, playerPosition) <= distance)
                {
                    playJumpscare();
                    return;
                }
                else
                {
                    // Go straight to player
                    //Vector3 originalPosition = transform.GetPosition();
                    //Vector2 nextPosition = Vector2.MoveTowards(ghostPosition, playerPosition, speed);
                    //transform.SetPosition(new Vector3(nextPosition.X, originalPosition.Y, nextPosition.Y));

                    MoveTo(playerPosition, speed);
                    //Vector2 nextFramePosition = Vector2.MoveTowards(new Vector2(transform.GetPosition().X, transform.GetPosition().Z), playerPosition, speed);
                    //if (Vector2.Distance(nextFramePosition, playerPosition) <= 40.0f)
                    //{
                    //    playJumpscare();
                    //    return;
                    //}
                }
                playerMoved = true;
                speed += 0.001f;

                PlayMonsterWalkingSound();

                break;

            case GhostEvent.MovingToDoor:

                gameObject.GetComponent<AnimatedComponent>().PlayAnimation();

                if (WaypointPathfinder.SameRoom(ghostPosition, playerPosition)) // If player is in the same room as ghost, chase player
                {
                    currentEvent = GhostEvent.ChasingPlayer;
                }
                else // Move to doors
                {
                    if (playerMoved)
                    {
                        WaypointPathfinder.FindPath(ghostPosition, playerPosition);
                    }
                    Vector3 originalPosition = transform.GetPosition();
                    Vector2 nextPosition = WaypointPathfinder.NextStep(new Vector2(originalPosition.X, originalPosition.Z), speed);
                    transform.SetPosition(new Vector3(nextPosition.X, originalPosition.Y, nextPosition.Y));
                    playerMoved = false;

                    if (doorPosition.X == transform.GetPosition().X && doorPosition.Y == transform.GetPosition().Z) // Reached
                    {
                        currentEvent = GhostEvent.Nothing;
                    }
                }

                PlayMonsterWalkingSound();

                break;

            case GhostEvent.BedroomHidingEvent:

                //Console.WriteLine(Vector3.Distance(player.transform.GetPosition(), transform.GetPosition()));
                //if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit()) // If player is in sight
                if (Vector3.Distance(player.transform.GetPosition(), transform.GetPosition()) <= 700.0f &&
                    (!bedroomHidingGameObject.GetComponent<Hiding>().hiding || flashlight.GetComponent<Flashlight_Script>().activateLight)) // If player is in range
                {
                    // NOTE: May want to add in "not hiding" condition
                    currentEvent = GhostEvent.ChasingPlayer;
                    distance = 40.0f;
                    return;
                }

                PlayMonsterWalkingSound();

                BedroomHidingEvent();

                break;

            case GhostEvent.LivingDiningRoomEvent: // 1. LivingDiningRoomEvent => move from dining room - living room door to dining room - main hall door & disappear

                PlayMonsterWalkingSound();

                LivingDiningRoomEvent();

                break;

            case GhostEvent.LivingRoomHidingEvent: // 2. LivingRoomHidingEvent => move from living room - right wing door to left of couch, look around, walk into dining room painting & stay there

                //Console.WriteLine(Vector3.Distance(player.transform.GetPosition(), transform.GetPosition()));
                //if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit()) // If player is in sight
                if (Vector3.Distance(player.transform.GetPosition(), transform.GetPosition()) <= 700.0f &&
                    (!livingRoomHidingGameObject.GetComponent<Hiding>().hiding || flashlight.GetComponent<Flashlight_Script>().activateLight) &&
                    eventStep < 3) // If player is in range
                {
                    // NOTE: May want to add in "not hiding" condition
                    currentEvent = GhostEvent.ChasingPlayer;
                    distance = 40.0f;
                    return;
                }

                PlayMonsterWalkingSound();

                LivingRoomHidingEvent();

                break;

            case GhostEvent.DiningRoomEvent:

                PlayMonsterWalkingSound();

                DiningRoomEvent();

                break;

            case GhostEvent.GalleryHidingEvent:

                //Console.WriteLine(Vector3.Distance(player.transform.GetPosition(), transform.GetPosition()));
                //if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit()) // If player is in sight
                if (Vector3.Distance(player.transform.GetPosition(), transform.GetPosition()) <= 700.0f &&
                    (!galleryHidingGameObject.GetComponent<Hiding>().hiding || flashlight.GetComponent<Flashlight_Script>().activateLight)) // If player is in range
                {
                    // NOTE: May want to add in "not hiding" condition
                    currentEvent = GhostEvent.ChasingPlayer;
                    distance = 40.0f;
                    return;
                }

                PlayMonsterWalkingSound();

                GalleryHidingEvent();

                break;

            case GhostEvent.FinalChasingEvent:

                PlayMonsterWalkingSound();

                FinalChaseEvent();

                break;
        }
    }
    public void playJumpscare()
    {
        GraphicsManagerWrapper.ToggleViewFrom2D(false);

        // Jumpscare scene
        gameBlackboard.gameState = GameBlackboard.GameState.Jumpscare; // Locks everything else, play jumpscare

        if (player.GetComponent<FPS_Controller_Script>().isCrouched)
        {
            player.GetComponent<FPS_Controller_Script>().StandUp();
        }
        player.GetComponent<FPS_Controller_Script>().isCrouched = false;

        SetEnabled(false);
        player.GetComponent<RigidBodyComponent>().SetLinearVelocity(new Vector3(0.0f, 0.0f, 0.0f));
        player.SetActive(true);
        player.GetComponent<FPS_Controller_Script>().SetEnabled(false);
        lockGroup.SetActive(false);

        // Switch with the jumpscare entity
        jumpscareMonster.SetActive(true);
        jumpscareMonster.transform.SetPositionX(transform.GetPosition().X);
        jumpscareMonster.transform.SetPositionY(-100.0f);
        jumpscareMonster.transform.SetPositionZ(transform.GetPosition().Z);
        jumpscareMonster.transform.SetRotation(transform.GetRotation());

        jumpscareMonster.GetComponent<JumpscareScript>().SetEnabled(true);
        jumpscareMonster.GetComponent<JumpscareScript>().jumpscareSequenceIndex = 0;
        jumpscareMonster.GetComponent<JumpscareScript>().screamTimer = 2.5f;

        bedroomHidingGameObject.GetComponent<Hiding>().hiding = false;
        galleryHidingGameObject.GetComponent<Hiding>().hiding = false;
        livingRoomHidingGameObject.GetComponent<Hiding>().hiding = false;

        gameObject.SetActive(false);

        //gameObject.GetComponent<JumpscareScript>().SetEnabled(true); // Jumpscare entity will have the script instead
        //gameObject.GetComponent<JumpscareScript>().jumpscareSequenceIndex = 0;
        //gameObject.GetComponent<JumpscareScript>().screamTimer = 2.5f;

        gameObject.GetComponent<AnimatedComponent>().StopAnimation();
    }

    public void PlayMonsterWalkingSoundInitial()
    {
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();
        walkingSoundCounter = 0;
        audio.set3DCoords(transform.GetPosition(), walkingSounds[walkingSoundCounter]);
        audio.play(walkingSounds[walkingSoundCounter]);
        currentEvent = GhostEvent.PlayingWalkingSound;
    }
    public bool PlayMonsterWalkingSound()
    {
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();

        //Walking counter
        if (walkingSoundCounter >= 7)
        {
            walkingSoundCounter = 0;
        }

        //Timer
        if(playSoundTimer < 0 && currentEvent == GhostEvent.PlayingWalkingSound)
        {
            playSoundTimer = soundSpeed - walkingSoundCounter * 0.005f;
        }
        else
        {
            MonsterWalkingTimer -= Time.deltaTime;
        }

        //Basement or not
        if (MonsterWalkingTimer < 0)
        {
            if (!Door_Script.basementcheck)
            {
                audio.set3DCoords(GhostTransformPosition, walkingSounds[walkingSoundCounter]);
                audio.set3DCoords(GhostTransformPosition, monsterPatrol[walkingSoundCounter]);
                //Console.WriteLine("Ghost position: " + GhostTransformPosition.X + ", " + GhostTransformPosition.Y + ", " + GhostTransformPosition.Z); ;
                //Console.WriteLine("Player position: " + player.transform.GetPosition().X + ", " + player.transform.GetPosition().Y + ", " + player.transform.GetPosition().Z);
                audio.play(walkingSounds[walkingSoundCounter]);
                audio.play(monsterPatrol[walkingSoundCounter]);
                walkingSoundCounter++;
            }
            else
            {
                audio.set3DCoords(GhostTransformPosition, walkingSounds[walkingSoundCounter]);
                audio.set3DCoords(GhostTransformPosition, monsterPatrol[walkingSoundCounter]);
                audio.play(walkingSounds[walkingSoundCounter]);
                audio.play(monsterPatrol[walkingSoundCounter]);
                walkingSoundCounter++;
            }

            MonsterWalkingTimer = 1.0f;
        }
        else
        {
            MonsterWalkingTimer -= Time.deltaTime;
        }

        return true;
    }

    public void AlertMonster()
    {
        soundSpeed -= 0.1f;
        speed += 0.2f;
        playSoundTimer = soundSpeed;

        // Move monster to player position if lockpick fails
        doorPosition = new Vector2(player.transform.GetPosition().X, player.transform.GetPosition().Z);
        WaypointPathfinder.FindPath(new Vector2(-2840, -650), doorPosition); // NOTE: temp position

        player.transform.SetPosition(transform.GetPosition());

        PlayMonsterWalkingSoundInitial();
    }

    //public bool PlayerInSight()
    //{
    //    Vector2 lineToNextPosition = new Vector2(player.transform.GetPosition().X - transform.GetPosition().X, player.transform.GetPosition().Z - transform.GetPosition().Z);
    //    float lineToNextPositionLength = Mathf.Sqrt(lineToNextPosition.X * lineToNextPosition.X + lineToNextPosition.Y * lineToNextPosition.Y);
    //    float forwardVectorLength = Mathf.Sqrt(Vector3.Forward().X * Vector3.Forward().X + Vector3.Forward().Z * Vector3.Forward().Z);

    //    float rotationOfPlayer = Mathf.Acos((lineToNextPosition.X * Vector3.Forward().X + lineToNextPosition.Y * Vector3.Forward().Z) / (lineToNextPositionLength * forwardVectorLength));

    //    return false;
    //}

    public bool MoveTo(Vector2 destination, float speed)
    {
        Vector2 ghostPosition = new Vector2(transform.GetPosition().X, transform.GetPosition().Z);
        GhostTransformPosition.X = ghostPosition.X;
        GhostTransformPosition.Z = ghostPosition.Y;
        float step = speed * Time.deltaTime * 60;
        Vector2 nextPosition = Vector2.MoveTowards(ghostPosition, destination, step);
        transform.SetPosition(new Vector3(nextPosition.X, transform.GetPosition().Y, nextPosition.Y));

        #region Turning monster to face where its walking

        Vector2 lineToNextPosition = new Vector2(nextPosition.X - ghostPosition.X, nextPosition.Y - ghostPosition.Y);
        float lineToNextPositionLength = Mathf.Sqrt(lineToNextPosition.X * lineToNextPosition.X + lineToNextPosition.Y * lineToNextPosition.Y);
        float forwardVectorLength = Mathf.Sqrt(Vector3.Forward().X * Vector3.Forward().X + Vector3.Forward().Z * Vector3.Forward().Z);

        float rotationOfMonster = Mathf.Acos((lineToNextPosition.X * Vector3.Forward().X + lineToNextPosition.Y * Vector3.Forward().Z) / (lineToNextPositionLength * forwardVectorLength));

        if (lineToNextPosition.X < 0)
            transform.SetRotation(new Vector3(0.0f, -rotationOfMonster, 0.0f));
        else
            transform.SetRotation(new Vector3(0.0f, rotationOfMonster, 0.0f));

        #endregion

        if (transform.GetPosition().X == destination.X && transform.GetPosition().Z == destination.Y)
        {
            return true;
        }

        return false;
    }

    public void BedroomHidingEvent()
    {
        if (startEvent) // Initialize variables (teleporting monster to starting position etc)
        {
            eventStep = -2;
            //Console.WriteLine("initialized bedroom hiding event");

            startEvent = false;
            bedroomMonsterAppearTimer = 2.0f;

            previousEvent = GhostEvent.BedroomHidingEvent;
            //gameObject.GetComponent<AudioComponent>().set3DCoords(transform.GetPosition(), "door_rattle");
            gameObject.GetComponent<AudioComponent>().play("door_rattle");
        }

        Vector3 originalPosition = transform.GetPosition();
        Vector2 ghostPosition = new Vector2(originalPosition.X, originalPosition.Z);
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();

        switch (eventStep)
        {
            case -2:

                if (gameObject.GetComponent<AudioComponent>().finished("door_rattle"))
                {
                    gameObject.GetComponent<AudioComponent>().stop("door_rattle");
                    ++eventStep;
                }

                break;

            case -1:
            
                if (bedroomMonsterAppearTimer <= 0.0f)
                {
                    transform.SetPosition(new Vector3(1790.0f, transform.GetPosition().Y, -750.0f));
                    gameObject.GetComponent<AnimatedComponent>().PlayAnimation();
                    ++eventStep;
                }
                bedroomMonsterAppearTimer -= Time.deltaTime;

                break;

            case 0: // Moving from SH door to end of bedframe in bed

                if (MoveTo(new Vector2(1790, -323), 2.0f))
                {
                    ++eventStep;
                }

                audio.play("pc_afterscare_breathing");
                audio.setVolume(0.5f, "pc_afterscare_breathing");
                audio.play("pc_afterscare_heartbeat");
                audio.setVolume(0.5f, "pc_afterscare_heartbeat");
                break;

            case 1: // Moving to player / hiding closet

                if (MoveTo(new Vector2(2167, -100), 2.0f))
                {
                    ++eventStep;
                }

                break;

            case 2: // Event done

                bedroomHideEventDone = true;
                currentEvent = GhostEvent.Nothing;
                SHDoor.GetComponent<Door_Script>().forcedLocked = false;

                // Teleporting monster to Living Room - Dining Room door for next event
                transform.SetPositionX(319.0f);
                transform.SetPositionZ(-379.0f);
                transform.SetRotationY(-(float)(0.5 * Math.PI));

                audio.FadeOut(2, "pc_afterscare_breathing");
                audio.FadeOut(2, "pc_afterscare_heartbeat");
                audio.play(voiceClips);
                GameplaySubtitles.counter = 11; // wth was that

                break;
        }

    }

    public void LivingDiningRoomEvent()
    {
        if (startEvent) // Initialize variables
        {
            livingDiningRoomStandingTimer = 1.0f;
            eventStep = 0;
            startEvent = false;

            transform.SetPositionX(319.0f);
            transform.SetPositionZ(-379.0f);
            gameObject.GetComponent<AnimatedComponent>().PlayAnimation();

            previousEvent = GhostEvent.LivingDiningRoomEvent;

            //Console.WriteLine("initialized living dining room event");
        }

        Vector2 ghostPosition = new Vector2(transform.GetPosition().X, transform.GetPosition().Z);
        Vector2 nextPosition = new Vector2(0.0f, 0.0f);

        switch (eventStep)
        {
            case 0: // Stay for a while

                if (livingDiningRoomStandingTimer <= 0)
                {
                    ++eventStep;
                }
                livingDiningRoomStandingTimer -= Time.deltaTime;

                break;

            case 1: // Move to beside dining room - main entrance door (making it look a little better)

                if (MoveTo(new Vector2(-272, -300), 2.0f))
                {
                    ++eventStep;
                }

                break;

            case 2: // Move to dining room - main entrance door & disappear

                if (MoveTo(new Vector2(-272, -80), 2.0f))
                {
                    ++eventStep;
                }

                break;

            case 3: // Event done

                livingDiningRoomEventDone = true;
                currentEvent = GhostEvent.Nothing;

                // Teleporting monster away so player wont see it (for now the position is in the gallery)
                transform.SetPositionX(-1920.0f);
                transform.SetPositionZ(175.0f);

                break;
        }
    }

    public void LivingRoomHidingEvent()
    {
        if (startEvent) // Initialize variables
        {
            livingRoomHidingTimer = 1.0f;
            eventStep = -2;

            startEvent = false;
            livingRoomMonsterAppearTimer = 4.0f;

            previousEvent = GhostEvent.LivingRoomHidingEvent;
            //Vector3 door_pos = new Vector3(938.0f, transform.GetPosition().Y, 17.0f);
            //gameObject.GetComponent<AudioComponent>().set3DCoords(door_pos, "door_rattle");
            gameObject.GetComponent<AudioComponent>().play("door_rattle");

            //Console.WriteLine("initialized living room hiding event");
        }

        switch (eventStep)
        {
            case -2:

                if (gameObject.GetComponent<AudioComponent>().finished("door_rattle"))
                {
                    gameObject.GetComponent<AudioComponent>().stop("door_rattle");
                    ++eventStep;
                }

                break;

            case -1:

                if (livingRoomMonsterAppearTimer <= 0.0f)
                {
                    // Teleporting monster to behind the living room - right hall door
                    transform.SetPositionX(938.0f);
                    transform.SetPositionZ(17.0f);
                    gameObject.GetComponent<AnimatedComponent>().PlayAnimation();
                    ++eventStep;
                }
                livingRoomMonsterAppearTimer -= Time.deltaTime;

                break;

            case 0: // Move to couch

                if (MoveTo(new Vector2(938.0f, -379.0f), 2.0f))
                {
                    ++eventStep;
                    gameObject.GetComponent<AnimatedComponent>().StopAnimation();
                }

                break;

            case 1: // Look around / wait

                if (livingRoomHidingTimer <= 0)
                {
                    ++eventStep;
                    gameObject.GetComponent<AnimatedComponent>().PlayAnimation();
                }
                livingRoomHidingTimer -= Time.deltaTime;

                break;

            case 2: // Move to living room - dining room door

                livingRoomHideEventStarted = true;
                //if (MoveTo(new Vector2(156.0f, -379.0f), 2.0f))
                if (MoveTo(new Vector2(200.0f, -379.0f), 2.0f))
                {
                    ++eventStep;
                }

                break;

            case 3: // Move to picture in dining room

                //if (MoveTo(new Vector2(-58.0f, -1060.0f), 2.0f))
                //{
                    ++eventStep;
                //}

                break;

            case 4: // Event done

                livingRoomHideEventDone = true;
                currentEvent = GhostEvent.Nothing;

                transform.SetPositionX(-58.0f);
                transform.SetPositionZ(-1060.0f);

                transform.SetRotationY((float)Math.PI);

                break;
        }
    }

    public void DiningRoomEvent()
    {
        if (startEvent) // Initialize variables
        {
            diningRoomTimer = ScriptAPI.Random.Range(1.0f, 3.0f);
            eventStep = 0;

            if (player.transform.GetPosition().X <= 319.0f)
            {
                gameObject.GetComponent<AudioComponent>().play("pc_movethissilently");
                GameplaySubtitles.counter = 46;
                startEvent = false;
            }

            previousEvent = GhostEvent.DiningRoomEvent;

            //Console.WriteLine("initialized dining event (red light green light)");
        }
        float rotationSpeedDegreePerSec = 300.0f;
        float rotationThisFrame = rotationSpeedDegreePerSec * Time.deltaTime;

        switch (eventStep)
        {
            case 0: // Turns every once in a while 

                if (diningRoomTimer <= 0)
                {
                    gameObject.GetComponent<AudioComponent>().play("mon_alerted1");
                    ++eventStep;
                }
                diningRoomTimer -= Time.deltaTime;

                break;

            case 1: // Turning towards player (5 degrees per frame)

                transform.SetRotationY(transform.GetRotation().Y - rotationThisFrame * (float)(Math.PI / 180.0f));

                // Finished turning
                if (transform.GetRotation().Y <= 0.0f)
                {
                    gameObject.GetComponent<AudioComponent>().stop("mon_alerted1");
                    ++eventStep;
                    diningRoomTimer = 0.5f;
                    playerOriginalPosition = player.transform.GetPosition();
                }

                break;

            case 2: // "Looking" around for player (just made it wait for a while for now)

                // Looking for player
                if ((playerOriginalPosition - player.transform.GetPosition() != new Vector3(0.0f, 0.0f, 0.0f)) || // Player moved
                    !player.GetComponent<FPS_Controller_Script>().isCrouched)
                {
                    currentEvent = GhostEvent.ChasingPlayer;
                    speed = 15.0f;
                    distance = 60.0f;
                }

                if (diningRoomTimer <= 0)
                {
                    ++eventStep;
                }
                diningRoomTimer -= Time.deltaTime;

                break;

            case 3: // Turn back and go back to the first eventStep

                transform.SetRotationY(transform.GetRotation().Y + rotationThisFrame * (float)(Math.PI / 180.0f));

                // Finished turning
                if (transform.GetRotation().Y >= (float)(Math.PI))
                {
                    eventStep = 0;
                    diningRoomTimer = ScriptAPI.Random.Range(1.0f, 3.0f);
                }

                break;
        }

        // When player enters kitchen, event ends
        if (player.transform.GetPosition().X < -873.0f)
        {
            currentEvent = GhostEvent.Nothing;
            diningRoomEventDone = true;

            //the below doesnt work?
            //AudioComponent audio = gameObject.GetComponent<AudioComponent>();
            ////play kitchen ambience
            //if (player.transform.GetPosition().X > -1073.0f)
            //{
            //    audio.play("kitchen_ambience");

            //}
            //else
            //{
            //    audio.stop("kitchen_ambience");
            //}
        }

        // When player enters gallery
        if (player.transform.GetPosition().X < -1073.0f)
        {
            
        }
    }

    public void GalleryHidingEvent()
    {
        if (startEvent) // Initialize variables (teleporting monster to starting position etc)
        {
            if(GallerySwitch.isActivated)
            {
                eventStep = 4;
            }
            else
            {
                eventStep = 0;
            }

            transform.SetPosition(new Vector3(-1920.0f, transform.GetPosition().Y, -300.0f)); // Gallery entrance
            startEvent = false;

            gameObject.GetComponent<AnimatedComponent>().PlayAnimation();

            previousEvent = GhostEvent.GalleryHidingEvent;
        }
        

        if (!galleryHidingGameObject.GetComponent<Hiding>().hiding || flashlight.GetComponent<Flashlight_Script>().activateLight) // If player comes out of hiding, monster will chase player
        {
            previousEvent = GhostEvent.GalleryHidingEvent;
            currentEvent = GhostEvent.ChasingPlayer;
        }

        AudioComponent audio = gameObject.GetComponent<AudioComponent>();

        switch (eventStep)
        {
            case 0: // Walk to Study Room Door

                if (MoveTo(new Vector2(-2850, -300), 10.0f))
                {
                    ++eventStep;
                }

                audio.play("pc_afterscare_breathing");
                audio.setVolume(0.5f, "pc_afterscare_breathing");
                audio.play("pc_afterscare_heartbeat");
                audio.setVolume(0.5f, "pc_afterscare_heartbeat");
                break;

            case 1: // Walk into Study Room

                if (MoveTo(new Vector2(-2850, 175), 5.0f))
                {
                    ++eventStep;
                }

                break;

            case 2: // Walk out of Study Room

                if (MoveTo(new Vector2(-1920, 175), 5.0f))
                {
                    ++eventStep;
                }

                break;

            case 3: // Event done

                galleryHideEventDone = true;
                currentEvent = GhostEvent.Nothing;
                galleryHidingGameObject.GetComponent<EventGalleryHiding>().GhostShouldMove = false;
                galleryHidingGameObject.GetComponent<EventGalleryHiding>().GhostMoved = false;

                audio.FadeOut(2, "pc_afterscare_breathing");
                audio.FadeOut(2, "pc_afterscare_heartbeat");

                break;

            case 4: // Walk to Family Painting
               
                if (MoveTo(new Vector2(-3000, -666), 5.0f))
                {
                    ++eventStep;
                    leftWingDoor.GetComponent<Door_Script>().forcedLocked = false;      // Unlock door
                    audio.play("pc_timetogetout");                                      // Player audio
                    GameplaySubtitles.counter = 45;                                     // Subtitles
                }

                audio.play("pc_afterscare_breathing");
                audio.setVolume(0.5f, "pc_afterscare_breathing");
                audio.play("pc_afterscare_heartbeat");
                audio.setVolume(0.5f, "pc_afterscare_heartbeat");

                break;

            case 5:
                Console.WriteLine("Reached");
                
                galleryHideEventDone = true;
                galleryChaseEventDone = false;

                break;


           
        }
    }

    public void FinalChaseEvent()
    {
        if (startEvent) // Initialize variables
        {
            transform.SetPositionX(-2840);
            transform.SetPositionZ(-300);

            // Change forced lock to locked
            leftWingDoor.GetComponent<Door_Script>().forcedLocked = false;

            eventStep = 0;
            startEvent = false;
            gameObject.GetComponent<AnimatedComponent>().PlayAnimation();

            previousEvent = GhostEvent.FinalChasingEvent;

            //Console.WriteLine("initialized final chasing event");
        }

        // Monster starts in the gallery
        // Chases player across the mansion
        // Fast but slow enough for player to lockpick left wing - main entrance door

        switch (eventStep)
        {
            case 0: // Make noise

                ++eventStep;

                break;

            case 1: // Move into study room

                if (MoveTo(new Vector2(-2840, 100), speed))
                {
                    ++eventStep;
                }

                break;

            case 2: // Move to before study room - left wing door

                if (MoveTo(new Vector2(-2380, 178), speed))
                {
                    ++eventStep;
                }

                break;

            case 3: // Chase player across mansion

                Vector2 ghostPosition = new Vector2(transform.GetPosition().X, transform.GetPosition().Z);
                Vector2 playerPosition = new Vector2(player.transform.GetPosition().X, player.transform.GetPosition().Z);

                if (playerPosition.X >= 2600) // If player goes to basement (NOTE: to change)
                {
                    ++eventStep;
                }

                // If touches, loses
                if (Vector2.Distance(ghostPosition, playerPosition) <= 40.0f)
                {
                    playJumpscare(); // For presentation
                    return;
                }
                else
                {
                    float finalChaseSpeed = 1.0f;

                    if (player.transform.GetPosition().X >= -730)
                    {
                        finalChaseSpeed = 5.0f;
                    }
                    
                    MoveTo(playerPosition, finalChaseSpeed);
                }

                break;

            case 4: // Event done

                currentEvent = GhostEvent.Nothing;
                finalChaseEventDone = true;

                break;

        }
    }
}
