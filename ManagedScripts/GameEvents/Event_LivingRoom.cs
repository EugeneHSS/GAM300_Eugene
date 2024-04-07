using ScriptAPI;
using System;

public class EventLivingRoom : Script
{
    public GameObject? focusedObj;
    public GameObject? enemyPathfinding;
    public GameObject? player;
    private Vector3 lookAmount = new Vector3();
    public float timer = 1.0f;
    public static bool doOnce = true;
    public bool timerStart = false;

    public override void Update()
    {
        if (timerStart) {

            #region PlayerCamera
            timer -= Time.deltaTime;
            doOnce = false;
            player.GetComponent<FPS_Controller_Script>().cameraCanMove = false;
            player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
            player.GetComponent<RigidBodyComponent>().SetLinearVelocity(Vector3.Zero());
            lookAmount = new Vector3(Mathf.Lerp(player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().X, Vector3.Normalize(focusedObj.transform.GetPosition() - player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetPosition()).X, Input.GetSensitivity() * Time.deltaTime),
                Mathf.Lerp(player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().Y, Vector3.Normalize(focusedObj.transform.GetPosition() - player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetPosition()).Y, Input.GetSensitivity() * Time.deltaTime),
                Mathf.Lerp(player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetRotation().Z, Vector3.Normalize(focusedObj.transform.GetPosition() - player.GetComponent<FPS_Controller_Script>().playerCamera.transform.GetPosition()).Z, Input.GetSensitivity() * Time.deltaTime));
            //lookAmount = Vector3.MoveTowards(player.GetComponent<FPS_Controller_Script>().transform.GetRotation(), Vector3.Normalize(focusedObj.transform.getForwardVector() - player.GetComponent<FPS_Controller_Script>().playerCamera.transform.getForwardVector()), 100.0f * Time.deltaTime);
            player.GetComponent<FPS_Controller_Script>().playerCamera.transform.SetRotationY(lookAmount.Y);
            #endregion

            #region EnemyMovement
            enemyPathfinding.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.LivingDiningRoomEvent;
            #endregion
        }
        if (timer <= 0.0f && timerStart)
        {
            #region PlayerCamera
            timerStart = false;
            player.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
            player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
            #endregion
        }
    }

    public override void OnTriggerEnter(ColliderComponent collider)
    {
        if (doOnce)
        {
            timerStart = true;
            player.GetComponent<FPS_Controller_Script>().playerCamera.transform.SetRotationX(0.0f);
            player.GetComponent<FPS_Controller_Script>().playerCamera.transform.SetRotationZ(0.0f);
            gameObject.GetComponent<ColliderComponent>().SetEnabled(false);
        }
        //Maybe Play Monster Audio here
    }
}