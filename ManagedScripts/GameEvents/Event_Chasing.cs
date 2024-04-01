using ScriptAPI;
using System;
using System.Threading;

public class EventChasing : Script
{

    public GameObject monster;
    public GameObject eventChasingTrigger;
    public GameObject LeftWingLight;
    public GameObject MainHallLight;
    public GameObject BedRoomLight;
    public GameObject BasementLight;

    private float timer = 0.0f;
    private bool doOnce = true;
    private bool blinking = false;
    private bool eventStarted = false;

    public override void Update()
    {
        if (eventStarted == true)
        {
            if (doOnce == true)
            {
                BedRoomLight.SetActive(false);
                doOnce = false;
            }

            // Set a timer for the lights to blink on and off every 1 second with setActive
            timer += Time.deltaTime;
            if (timer >= 1.0f)
            {
                if (blinking == false)
                {
                    LeftWingLight.SetActive(false);
                    MainHallLight.SetActive(false);
                    BasementLight.SetActive(false);
                    blinking = true;
                }
                else
                {
                    LeftWingLight.SetActive(true);
                    MainHallLight.SetActive(true);
                    BasementLight.SetActive(true);
                    blinking = false;
                }
                timer = 0.0f;
            }
            if (eventChasingTrigger.GetComponent<RigidBodyComponent>().IsSensorActivated())
            {
                LeftWingLight.SetActive(false);
                MainHallLight.SetActive(false);
                BasementLight.SetActive(false);
                eventStarted = false;
                eventChasingTrigger.GetComponent<ColliderComponent>().SetEnabled(false);
            }
        }


    }

    public override void OnTriggerEnter(ColliderComponent collider)
    {
        //monster.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.FinalChasingEvent;
        eventStarted = true;      
        gameObject.GetComponent<ColliderComponent>().SetEnabled(false);

    }
}