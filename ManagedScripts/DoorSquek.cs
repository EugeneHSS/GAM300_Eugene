using ScriptAPI;
using System;

public class DoorSquek : Script
{
    readonly string doorSquekSFX = "door creak";
    readonly string monsterAlert = "mon_alerted3";
    readonly string monsterChase = "monster chase music";
    AudioComponent audio;
    private bool alerted = false;
    private float elapsedTime = 0f;
    //public GameObject eventChasing;
    public GameObject monster;

    public Checkpoint checkpoint;

    public override void Awake()
    {
        audio = gameObject.GetComponent<AudioComponent>();
        checkpoint = GameObjectScriptFind("Checkpoint").GetComponent<Checkpoint>();
    }

    public override void Start()
    {

    }
    public override void Update()
    {
        if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit()&& Input.GetKeyDown(Keycode.E))
        {
            checkpoint.OverrideCheckpoint(GhostMovement.GhostEvent.FinalChasingEvent);
            audio.play(doorSquekSFX);
            monster.GetComponent<GhostMovement>().currentEvent = GhostMovement.GhostEvent.FinalChasingEvent;
            monster.GetComponent<GhostMovement>().startEvent = true;
            alerted = true;
        }
        if (alerted == true)
        {
            elapsedTime += Time.deltaTime;
            if (elapsedTime >= 1f)
            {
                audio.play(monsterAlert);
                audio.set3DCoords(transform.GetPosition(), monsterAlert);
                audio.play(monsterChase);
                alerted = false;
                elapsedTime = 0f;

                //eventChasing.GetComponent<EventChasing>().eventStarted = true;
            }
        }
    }
    public override void LateUpdate()
    {

    }
}