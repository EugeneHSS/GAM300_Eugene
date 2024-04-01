using ScriptAPI;
using System;
using System.ComponentModel;
using System.Security.Cryptography;

public class FPS_Controller_Script : Script
{
    public RigidBodyComponent rb;
    public string[] footStepSoundEffects;
    string[] crouchstepSound;
    public static String[] backgroundMusic;
    private int currentFootStepPlaying;
    float audioTimer;
    AudioComponent audio;

    #region Camera Movement Variables
    [Header("Camera Movement Variables")]
    public CameraComponent playerCamera;
    public float fov = 60f;
    public bool invertCamera = false;
    public bool cameraCanMove = true;
    public float mouseSensitivity = 1.0f;
    public float maxLookAngle = 50f;

    // Crosshair
    public bool lockCursor = true;
    public bool crosshair = true;
    //public Image crosshairObject;
    //public Sprite crosshairImage;
    //public Color crosshairColor = Color.white;

    // Internal Variables
    private float yaw = 0.0f;
    private float pitch = 0.0f;

    #region Camera Zoom Variables
    public bool enableZoom = true;
    public bool holdToZoom = false;
    public uint zoomKey = Keycode.M2;
    public float zoomFOV = 30f;
    public float zoomStepTime = 5f;

    // Internal Variables
    private bool isZoomed = false;
    #endregion

    #endregion

    #region Movement Variables
    [Header("Movement Variables")]
    public bool playerCanMove = true;
    public float walkSpeed = 3f;
    private float savedWalkSpeed = 3f;
    public float maxVelocityChange = 10f;
    public bool isWalking = false;


    #region Sprint
    public bool enableSprint = true;
    public bool unlimitedSprint = false;
    public uint sprintKey = Keycode.LSHIFT;
    public float sprintSpeed = 3f;
    public float currentSprintSpeed;
    public float sprintDuration = 5f;
    public float sprintCooldown = .5f;
    public float sprintFOV = 80f;
    public float sprintFOVStepTime = 10f;

    // Sprint Bar
    public bool useSprintBar = true;
    public bool hideBarWhenFull = true;
    //public CanvasGroup sprintBarCG;
    //public Image sprintBarBG;
    //public Image sprintBar;
    public float sprintBarWidthPercent = .3f;
    public float sprintBarHeightPercent = .015f;

    // Internal Variables
    public bool isSprinting = false;
    private float sprintRemaining;
    private float sprintBarWidth;
    private float sprintBarHeight;
    private bool isSprintCooldown = false;
    private float sprintCooldownReset;
    #endregion

    #region Jump
    public bool enableJump = true;
    public uint jumpKey = Keycode.SPACE;
    public float jumpPower = 5f;

    // Internal Variables
    private bool isGrounded = false;
    #endregion

    #region Crouch
    public bool enableCrouch;
    public bool holdToCrouch;
    public uint crouchKey = Keycode.CTRL;
    public float crouchHeight = .75f;
    public float speedReduction = .5f;
    public bool isCrouched = false; // Needed for dining event

    // Internal Variables
    private Vector3 originalScale;
    private float originalHeight;
    #endregion

    #region Health
    public int playerHealth = 100;
    #endregion
    #endregion

    #region Head Bob
    [Header("HeadBob")]
    public bool enableHeadBob = true;
    public TransformComponent joint;
    public float bobSpeed = 10f;
    public Vector3 bobAmount = new Vector3(.15f, .05f, 0f);

    // Internal Variables
    private Vector3 jointOriginalPos;
    private float timer = 0;
    #endregion


    public override void Awake()
    {
        rb = gameObject.GetComponent<RigidBodyComponent>();
        playerCamera = GameObjectScriptFind("playerCameraObject").GetComponent<CameraComponent>();
        // Set internal variables
        playerCamera.SetFieldOfView(fov);
        originalScale = transform.GetScale();
        originalHeight = transform.GetPosition().Y;
        jointOriginalPos = joint.GetPosition();

        if (!unlimitedSprint)
        {
            sprintRemaining = sprintDuration;
            sprintCooldownReset = sprintCooldown;
        }

        footStepSoundEffects = new string[10];
        footStepSoundEffects[0] = "pc_woodstep1";
        footStepSoundEffects[1] = "pc_woodstep2";
        footStepSoundEffects[2] = "pc_woodstep3";
        footStepSoundEffects[3] = "creak1";
        footStepSoundEffects[4] = "pc_woodstep4";
        footStepSoundEffects[5] = "pc_woodstep5";
        footStepSoundEffects[6] = "pc_woodstep6";
        footStepSoundEffects[7] = "pc_woodstep7";
        footStepSoundEffects[8] = "pc_woodstep8";
        footStepSoundEffects[9] = "creak3";
        crouchstepSound = new string[7];
        crouchstepSound[0] = "pc_crouchstep1";
        crouchstepSound[1] = "pc_crouchstep2";
        crouchstepSound[2] = "pc_crouchstep3";
        crouchstepSound[3] = "pc_crouchstep4";
        crouchstepSound[4] = "pc_crouchstep5";
        crouchstepSound[5] = "pc_crouchstep6";
        crouchstepSound[6] = "pc_crouchstep7";
        currentFootStepPlaying = 0;
        audioTimer = 1.0f;

        backgroundMusic = new String[3];
        backgroundMusic[0] = "outside_ambience";
        savedWalkSpeed = walkSpeed;
    }
    public override void Start()
    {

        #region Setting Cursor & Crosshair
        //if (lockCursor)
        //{
        //    Input.Lock(lockCursor);
        //    Input.HideMouse(lockCursor);
        //}
        #endregion

        #region Setting Sprint Bar
        if (useSprintBar)
        {
            //sprintBarBG.gameObject.SetActive(true);
            //sprintBar.gameObject.SetActive(true);

            //float screenWidth = Screen.width;
            //float screenHeight = Screen.height;

            //sprintBarWidth = screenWidth * sprintBarWidthPercent;
            //sprintBarHeight = screenHeight * sprintBarHeightPercent;

            //sprintBarBG.rectTransform.sizeDelta = new Vector3(sprintBarWidth, sprintBarHeight, 0f);
            //sprintBar.rectTransform.sizeDelta = new Vector3(sprintBarWidth - 2, sprintBarHeight - 2, 0f);

            if (hideBarWhenFull)
            {
                //sprintBarCG.alpha = 0;
            }
        }
        else
        {
            //sprintBarBG.gameObject.SetActive(false);
            //sprintBar.gameObject.SetActive(false);
        }
        #endregion

        enableCrouch = true;
        holdToCrouch = false;
    }
    public override void Update()
    {
        /*if (!InventoryScript.InventoryIsOpen)
        cameraCanMove = !InventoryScript.InventoryIsOpen;
        playerCanMove = !InventoryScript.InventoryIsOpen;*/

        //playerCanMove = !InventoryScript.InventoryIsOpen;
        //cameraCanMove = !PopupUI.isDisplayed;
        //playerCanMove = !PopupUI.isDisplayed;

        #region Sprint

        if (enableSprint)
        {
            if (isSprinting)
            {
                isZoomed = false;
                playerCamera.SetFieldOfView(Mathf.Lerp(playerCamera.GetFieldOfView(), sprintFOV, sprintFOVStepTime * Time.deltaTime));

                // Drain sprint remaining while sprinting
                if (!unlimitedSprint)
                {
                    sprintRemaining -= 1 * Time.deltaTime;
                    if (sprintRemaining <= 0)
                    {
                        isSprinting = false;
                        isSprintCooldown = true;
                    }
                }
            }
            else
            {
                // Regain sprint while not sprinting
                sprintRemaining = Mathf.Clamp(sprintRemaining += 1 * Time.deltaTime, 0, sprintDuration);
            }

            // Handles sprint cooldown 
            // When sprint remaining == 0 stops sprint ability until hitting cooldown
            if (isSprintCooldown)
            {
                sprintCooldown -= 1 * Time.deltaTime;
                if (sprintCooldown <= 0)
                {
                    isSprintCooldown = false;
                }
            }
            else
            {
                sprintCooldown = sprintCooldownReset;
            }

            // Handles sprintBar 
            if (useSprintBar && !unlimitedSprint)
            {
                float sprintRemainingPercent = sprintRemaining / sprintDuration;
                //sprintBar.transform.localScale = new Vector3(sprintRemainingPercent, 1f, 1f);
            }
        }

        #endregion

        //#region Jump

        //// Gets input and calls jump method
        //if (enableJump && Input.GetKeyDown(jumpKey) && isGrounded)
        //{
        //    Jump();
        //}

        //#endregion

        #region Crouch

        if (enableCrouch)
        {
            if (Input.GetKeyDown(crouchKey) && !holdToCrouch)
            {
                if (isCrouched) StandUp();
                else Crouch();

                isCrouched = !isCrouched;
            }

            //else if (Input.GetKeyDown(crouchKey) && holdToCrouch)
            //{
            //    Crouch();
            //}
            //else if (Input.GetKeyUp(crouchKey) && holdToCrouch)
            //{
            //    Crouch();
            //}
        }

        #endregion

        //CheckGround();

        if (enableHeadBob)
        {
            HeadBob();
        }

        Vector3 up_vector = Vector3.Normalize(Vector3.Cross(playerCamera.getRightVector(), playerCamera.getForwardVector()));
        audio.setPlayerCoords(transform.GetPosition(), Vector3.Normalize(playerCamera.getForwardVector()), up_vector);
    }
    public override void FixedUpdate()
    {
        #region Movement
        if (playerCanMove)
        {
            // Calculate how fast we should be moving
            Vector3 targetVelocity = new Vector3(Input.GetHorizontalAxis(), 0, Input.GetVerticalAxis());

            // Checks if player is walking and isGrounded
            // Will allow head bob
            if (targetVelocity.X != 0 || targetVelocity.Z != 0 /*&& isGrounded*/)
            {
                isWalking = true;
            }
            else
            {
                isWalking = false;
            }

            if (Input.GetKey(Keycode.W) || Input.GetKey(Keycode.S) || Input.GetKey(Keycode.A) || Input.GetKey(Keycode.D))
            {
                isWalking = true;
            }
            else
            {
                isWalking = false;
            }

            // All movement calculations shile sprint is active
            if (enableSprint && Input.GetKey(sprintKey) && sprintRemaining > 0f && !isSprintCooldown)
            {
                if (currentSprintSpeed < 8)
                {
                    currentSprintSpeed += 0.02f;
                }

                targetVelocity = transform.TransformDirection(targetVelocity) * currentSprintSpeed;
                // Apply a force that attempts to reach our target velocity
                //Vector3 velocity = rb.GetLinearVelocity();
                Vector3 velocityChange = targetVelocity /*- velocity*/;
                velocityChange.X = Mathf.Clamp(velocityChange.X, -maxVelocityChange, maxVelocityChange);
                velocityChange.Z = Mathf.Clamp(velocityChange.Z, -maxVelocityChange, maxVelocityChange);
                velocityChange.Y = 0;

                // Player is only moving when valocity change != 0
                // Makes sure fov change only happens during movement
                if (velocityChange.X != 0 || velocityChange.Z != 0)
                {
                    isSprinting = true;

                    /*if (isCrouched)
                    {
                        Crouch();
                    }*/

                    if (hideBarWhenFull && !unlimitedSprint)
                    {
                        //sprintBarCG.alpha += 5 * Time.deltaTime;
                    }
                }
                // this is the command to move the object, modify the variable if needed for too slow/fast
                gameObject.GetComponent<RigidBodyComponent>().SetLinearVelocity(velocityChange * 70);
            }
            // All movement calculations while walking
            else
            {
                isSprinting = false;
                currentSprintSpeed = sprintSpeed;

                if (hideBarWhenFull && sprintRemaining == sprintDuration)
                {
                    //sprintBarCG.alpha -= 3 * Time.deltaTime;
                }
                targetVelocity = transform.TransformDirection(targetVelocity) * walkSpeed;

                // Apply a force that attempts to reach our target velocity
                //Vector3 velocity = rb.GetLinearVelocity();
                Vector3 velocityChange = targetVelocity /*- velocity*/;
                velocityChange.X = Mathf.Clamp(velocityChange.X, -maxVelocityChange, maxVelocityChange);
                velocityChange.Z = Mathf.Clamp(velocityChange.Z, -maxVelocityChange, maxVelocityChange);
                velocityChange.Y = 0;

                // this is the command to move the object, modify the variable if needed for too slow/fast
                gameObject.GetComponent<RigidBodyComponent>().SetLinearVelocity(velocityChange * 50);
            }

            footstepAudio();

        }

        //if (isCollided)
        //{
        //    Vector3 targetVelocity = new Vector3(Input.GetAxis("Horizontal"), 0, Input.GetAxis("Vertical"));

        //    if (targetVelocity.X != 0 || targetVelocity.Z != 0 && isGrounded)
        //    {
        //        isWalking = true;
        //    }
        //    else
        //    {
        //        isWalking = false;
        //    }

        //    if (Input.GetKey(Keycode.W) || Input.GetKey(Keycode.S) || Input.GetKey(Keycode.A) || Input.GetKey(Keycode.D))
        //    {
        //        isWalking = true;
        //    }
        //    else
        //    {
        //        isWalking = false;
        //    }
        //    Vector3 velocityChange = targetVelocity /*- velocity*/;
        //    velocityChange.X = Mathf.Clamp(velocityChange.X, -maxVelocityChange, maxVelocityChange);
        //    velocityChange.Z = Mathf.Clamp(velocityChange.Z, -maxVelocityChange, maxVelocityChange);
        //    velocityChange.Y = 0;

        //    collidedEntity = GameObjectScriptFind(gameObject.GetSphereColliderComponent().GetColliderName());

        //    Vector3 direction = (gameObject.GetTransformComponent().GetPosition() - collidedEntity.GetTransformComponent().GetPosition()).normalise();
        //    float dotProduct = Vector3.Dot(direction, velocityChange);
        //    if (dotProduct > 0)
        //    {
        //        Vector3 transformPosition = transform.GetPosition();
        //        transform.SetPosition(new Vector3(transformPosition.X + velocityChange.X, transformPosition.Y, transformPosition.Z + velocityChange.Z));
        //        gameObject.GetSphereColliderComponent().SetIsInteract(false);
        //        gameObject.GetSphereColliderComponent().SetIsTrigger(false);

        //    }

        //    //rb.AddForce(velocityChange/*, ForceMode.VelocityChange*/);



        //}
        #endregion
        if (!LockPick1.enteredHouse)
        {
            audio.play(backgroundMusic[0]);
        }
        else
        {
            if (audio.checkPlaying(backgroundMusic[0]))
            {
                //audio.FadeOut(3, backgroundMusic[0]);
            }
            if (audio.finished(backgroundMusic[0]))
            {
                audio.play("ambientdrone1");
            }
        }
    }

    public override void LateUpdate()
    {
        #region Camera
        if (cameraCanMove)
        {
            if (Input.GetAxisX() != 0 || Input.GetAxisY() != 0)
            {
                yaw = transform.GetRotation().Y + Input.GetAxisX() * Input.GetSensitivity() * mouseSensitivity;
                if (!invertCamera)
                {
                    pitch -= Input.GetAxisY() * Input.GetSensitivity() * mouseSensitivity;
                }
                else
                {
                    // Inverted Y
                    pitch += Input.GetAxisY() * Input.GetSensitivity() * mouseSensitivity;
                }

                // Clamp pitch between lookAngle
                pitch = Mathf.Clamp(pitch, -maxLookAngle, maxLookAngle);

                transform.SetRotation(Vector3.Up() * yaw);
                transform.SetRotationY(transform.GetRotation().Y % 360);
                if (transform.GetRotation().Y < 0)
                {
                    transform.SetRotationY(360 - transform.GetRotation().Y);
                }

                playerCamera.transform.SetRotationX(pitch);
                playerCamera.transform.SetRotationY(transform.GetRotation().Y);
            }
        }


        #region Camera Zoom
        if (enableZoom)
        {
            // Changes isZoomed when key is pressed
            // Behavior for toogle zoom
            if (Input.GetKeyDown(zoomKey) && !holdToZoom && !isSprinting)
            {
                if (!isZoomed)
                {
                    isZoomed = true;
                }
                else
                {
                    isZoomed = false;
                }
            }

            // Changes isZoomed when key is pressed
            // Behavior for hold to zoom
            if (holdToZoom && !isSprinting)
            {
                if (Input.GetKeyDown(zoomKey))
                {
                    isZoomed = true;
                }
                else if (Input.GetKeyUp(zoomKey))
                {
                    isZoomed = false;
                }
            }

            // Lerps camera.fieldOfView to allow for a smooth transistion
            if (isZoomed)
            {
                playerCamera.SetFieldOfView(Mathf.Lerp(playerCamera.GetFieldOfView(), zoomFOV, zoomStepTime * Time.deltaTime));
            }
            else if (!isZoomed && !isSprinting)
            {
                playerCamera.SetFieldOfView(Mathf.Lerp(playerCamera.GetFieldOfView(), fov, zoomStepTime * Time.deltaTime));
            }
        }

        #endregion
        #endregion
    }

    //#region Check Ground Method
    //private void CheckGround()
    //{
    //    Vector3 origin = new Vector3(transform.position.x, transform.position.y - (transform.localScale.y * .5f), transform.position.z);
    //    Vector3 direction = transform.TransformDirection(Vector3.down);
    //    float distance = .75f;

    //    if (Physics.Raycast(origin, direction, out RaycastHit hit, distance))
    //    {
    //        Debug.DrawRay(origin, direction * distance, Color.red);
    //        isGrounded = true;
    //    }
    //    else
    //    {
    //        isGrounded = false;
    //    }
    //}
    //#endregion

    //#region Jump Method
    //private void Jump()
    //{
    //    // Adds force to the player rigidbody to jump
    //    if (isGrounded)
    //    {
    //        rb.AddForce(0f, jumpPower, 0f, ForceMode.Impulse);
    //        isGrounded = false;
    //    }

    //    // When crouched and using toggle system, will uncrouch for a jump
    //    if (isCrouched && !holdToCrouch)
    //    {
    //        Crouch();
    //    }
    //}
    //#endregion

    #region Crouch Method
    private void Crouch()
    {
        transform.SetScale(new Vector3(originalScale.X, crouchHeight, originalScale.Z));

        float heightToCrouchTo = originalHeight * crouchHeight;

        //lerping to crouching height
        //if (transform.GetPosition().Y > heightToCrouchTo)
        //{
        //    float heightdecreaseperframe = (originalHeight * (1 - crouchHeight)) * Time.deltaTime;
        //    transform.SetPositionY(transform.GetPosition().Y - heightdecreaseperframe);
        //}
        //transform.SetPositionY(0);
        Quaternion quat = new Quaternion(transform.GetRotation());
        gameObject.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(
            new Vector3(transform.GetPosition().X, transform.GetPosition().Y - heightToCrouchTo, transform.GetPosition().Z),
            new Vector4 (quat.X, quat.Y, quat.Z, quat.W), new Vector3(0, 0, 0), new Vector3(0, 0, 0));

        if (speedReduction != 0 && walkSpeed == savedWalkSpeed) walkSpeed *= speedReduction;
    }

    public void StandUp()
    {
        transform.SetScale(new Vector3(originalScale.X, originalScale.Y, originalScale.Z));
        //transform.SetPositionY(originalHeight);
        Quaternion quat = new Quaternion(transform.GetRotation());
        gameObject.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(
            new Vector3(transform.GetPosition().X, transform.GetPosition().Y + (originalHeight * crouchHeight), transform.GetPosition().Z),
            new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(0, 0, 0), new Vector3(0, 0, 0));
        if (speedReduction != 0 && walkSpeed < savedWalkSpeed) walkSpeed /= speedReduction;
    }
    #endregion

    #region HeadBob Method
    private void HeadBob()
    {
        if (isWalking)
        {
            // Calculates HeadBob speed during sprint
            if (isSprinting)
            {
                timer += Time.deltaTime * (bobSpeed + sprintSpeed);
            }
            // Calculates HeadBob speed during crouched movement
            else if (isCrouched)
            {
                timer += Time.deltaTime * (bobSpeed * speedReduction);
            }
            // Calculates HeadBob speed during walking
            else
            {
                timer += Time.deltaTime * bobSpeed;
            }
            // Applies HeadBob movement
            joint.SetPosition(new Vector3(jointOriginalPos.X + Mathf.Sin(timer) * bobAmount.X, jointOriginalPos.Y + Mathf.Sin(timer) * bobAmount.Y, jointOriginalPos.Z + Mathf.Sin(timer) * bobAmount.Z));
        }
        else
        {
            // Resets when play stops moving
            timer = 0;
            joint.SetPosition(new Vector3(Mathf.Lerp(joint.GetPosition().X, jointOriginalPos.X, Time.deltaTime * bobSpeed), Mathf.Lerp(joint.GetPosition().Y, jointOriginalPos.Y, Time.deltaTime * bobSpeed), Mathf.Lerp(joint.GetPosition().Z, jointOriginalPos.Z, Time.deltaTime * bobSpeed)));
        }
    }
    #endregion

    void footstepAudio()
    {
        audio = gameObject.GetComponent<AudioComponent>();
        
        if(isWalking) //no footsteps sfx when crouching
        {
            if (audioTimer < 0.0f)
            {
                if (!isCrouched)
                {
                    currentFootStepPlaying = (currentFootStepPlaying > 10 ? 0 : currentFootStepPlaying + 1);
                    audio.play(footStepSoundEffects[currentFootStepPlaying]);
                    audioTimer = 1.0f;
                }
                else
                {
                    currentFootStepPlaying = (currentFootStepPlaying > 7 ? 0 : currentFootStepPlaying + 1);
                    audio.play(crouchstepSound[currentFootStepPlaying]);
                    audioTimer = 1.0f;
                }
            }
            else
            {
                audioTimer -= Time.deltaTime;
            }
        }
    }
}