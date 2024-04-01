/*!*************************************************************************
****
\file JumpscareScript.cs
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\par Course: csd3450
\date 9-2-2024
\brief  Script to toggle map
****************************************************************************
***/
using ScriptAPI;

public class JumpscareScript : Script
{
    public GameObject player;
    public int jumpscareSequenceIndex;
    public float screamTimer = 0;

    private float yRotation = 0.0f;

    private Vector3 originalPosition;
    public GameObject playerCamera;
    public Checkpoint checkpoint;
    public GameBlackboard gameBlackboard;
    public GameObject gameMonster;

    private bool fadeIn = false;
    private bool fadeOut = true;
    private float fadeIncrement = 0.005f;
    private float originalFadeValue;

    private float timer;

    public override void Awake()
    {
        jumpscareSequenceIndex = 0;
        SetEnabled(false);
        screamTimer = 2.5f;
        //playerCamera.transform.SetPosition(new Vector3(0.0f , 90.0f, 0.0f));
        //player.transform.SetPosition(new Vector3(0.0f , 90.0f, 0.0f));

        checkpoint = GameObjectScriptFind("Checkpoint").GetComponent<Checkpoint>();
        gameBlackboard = GameObjectScriptFind("GameBlackboard").GetComponent<GameBlackboard>();
        gameMonster = GameObjectScriptFind("Ghost");

        originalFadeValue = GraphicsManagerWrapper.GetFadeFactor();
    }
    public override void Update()
    {
        switch (jumpscareSequenceIndex)
        {            
            // 1. calculating rotations
            case 0:

                Console.WriteLine("Jumpscare start");
                Console.WriteLine(player.transform.GetPosition().X + "\t" + player.transform.GetPosition().Z);
                Console.WriteLine(transform.GetPosition().X + "\t" + transform.GetPosition().Z);

                GraphicsManagerWrapper.ToggleViewFrom2D(false);

                // Turning towards ghost
                Vector2 lineToNextPosition = new Vector2(transform.GetPosition().X - player.transform.GetPosition().X, transform.GetPosition().Z - player.transform.GetPosition().Z);
                float lineToNextPositionLength = Mathf.Sqrt(lineToNextPosition.X * lineToNextPosition.X + lineToNextPosition.Y * lineToNextPosition.Y);
                float forwardVectorLength = Mathf.Sqrt(Vector3.Forward().X * Vector3.Forward().X + Vector3.Forward().Z * Vector3.Forward().Z);

                Console.WriteLine(lineToNextPosition.X + "\t" + lineToNextPosition.Y);
                Console.WriteLine(lineToNextPositionLength);

                yRotation = Mathf.Acos((lineToNextPosition.X * Vector3.Forward().X + lineToNextPosition.Y * Vector3.Forward().Z) / (lineToNextPositionLength * forwardVectorLength));

                Console.WriteLine(yRotation);

                if (lineToNextPosition.X >= 0)
                    yRotation = 360 - (float)(yRotation / Math.PI * 180.0f);
                else
                    yRotation = (float)(yRotation / Math.PI * 180.0f);

                Console.WriteLine(yRotation);

                ++jumpscareSequenceIndex;
                timer = 0.3f;

                break;

            // 2. turn player camera towards ghost, then up fast
            case 1:

                player.transform.SetRotationY(Mathf.LerpAngle(player.transform.GetRotation().Y, yRotation, Time.deltaTime * 12));
                player.GetComponent<FPS_Controller_Script>().playerCamera.transform.SetRotationX(
                    Mathf.LerpAngle(player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().X, 28, Time.deltaTime * 12));

                //Console.WriteLine(yRotation);

                if (player.transform.GetRotation().Y >= 360)
                {
                    player.transform.SetRotationY(player.transform.GetRotation().Y - 360);
                }

                if (timer <= 0.0f ||
                    (player.transform.GetRotation().Y >= yRotation - 3 && player.transform.GetRotation().Y <= yRotation + 3 &&
                    player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().X >= 28 - 3 && player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().X <= 28 + 3))
                {
                    gameObject.GetComponent<AudioComponent>().stopAll();
                    gameObject.GetComponent<AudioComponent>().play("mon_death");
                    originalPosition = transform.GetPosition();
                    ++jumpscareSequenceIndex;
                }
                timer -= Time.deltaTime;

                //GraphicsManagerWrapper.ToggleViewFrom2D(false);
                //gameObject.GetComponent<AudioComponent>().play("mon_death");
                //++jumpscareSequenceIndex;
                break;

            // 3. (may want to add scream sounds here before next step)
            // Add head shaking
            case 2:

                transform.SetPosition(originalPosition + new Vector3(ScriptAPI.Random.Range(-2.0f, 2.0f), ScriptAPI.Random.Range(-2.0f, 2.0f), 0.0f));

                if (screamTimer <= 0.0f)
                {
                    gameObject.GetComponent<AudioComponent>().stop("mon_death");
                    transform.SetPosition(originalPosition);
                    ++jumpscareSequenceIndex;
                }
                screamTimer -= Time.deltaTime;

                break;

            // 4. move ghost head down to player head (estimate) 
            case 3:

                Vector2 ghostPosition = new Vector2(transform.GetPosition().X, transform.GetPosition().Z);
                Vector2 playerPositon = new Vector2(player.transform.GetPosition().X, player.transform.GetPosition().Z);
                Vector2 nextPosition = Vector2.MoveTowards(ghostPosition, playerPositon, 10);
                transform.SetPosition(new Vector3(nextPosition.X, transform.GetPosition().Y - 4.5f, nextPosition.Y));

                if (transform.GetPosition().X == playerPositon.X && transform.GetPosition().Z == playerPositon.Y)
                {
                    fadeOut = true;
                    fadeIn = false;
                    ++jumpscareSequenceIndex;
                }

                break;

            // 5. lost screen
            case 4:
                // Fade to black
                if (fadeOut)
                {
                    GraphicsManagerWrapper.SetFadeFactor(GraphicsManagerWrapper.GetFadeFactor() - fadeIncrement);

                    if (GraphicsManagerWrapper.GetFadeFactor() <= 0.0f)
                    {
                        fadeOut = false;
                        fadeIn = true;
                    }
                }
                else if (fadeIn)
                {
                    GraphicsManagerWrapper.SetFadeFactor(GraphicsManagerWrapper.GetFadeFactor() + fadeIncrement);

                    if (GraphicsManagerWrapper.GetFadeFactor() >= originalFadeValue)
                    {
                        fadeIn = false;
                        GraphicsManagerWrapper.SetFadeFactor(originalFadeValue);
                    }
                }
                else
                {
                    // Revert to checkpoint
                    checkpoint.RevertToCheckpoint();

                    // Fade back
                    gameMonster.GetComponent<GhostMovement>().SetEnabled(true);
                    player.GetComponent<FPS_Controller_Script>().SetEnabled(true);

                    gameBlackboard.gameState = GameBlackboard.GameState.InGame;
                    jumpscareSequenceIndex = 0;
                    gameMonster.SetActive(true);
                    gameObject.SetActive(false);
                    SetEnabled(false);
                }

                break;
        }
    }
}
