using ScriptAPI;
using System;

public class EventBathroomSink : Script
{
    public GameObject? battery;
    public bool doOnce = true;
    public GameObject? bathroomLights;
    public GameObject? bathroomCurtains;
    public GameObject? player;
    public GameObject? monster;
    public GameObject? lastMonsterLocation;
    public GameObject? playerFlashlight;

    private bool flicker = false;
    public float flickerTimer = 0.0f;
    public float flickerSpeed = 1.0f;
    public bool flickerBool = true;

    public float ghostTimer = 1.0f;

    public float flickerOffTimer = 1.0f;
    public bool showerEvent = false;

    public bool playerWorks = false;

    public Vector3 lastPlayerRot = new Vector3();

    public float showerCuratinMoveSpeed = 300.0f;
    public AudioComponent audio;

    public override void Update()
    {
        audio = gameObject.GetComponent<AudioComponent>();
        Vector3 listenerpos = gameObject.GetComponent<AudioComponent>().getListenerPos();
        if (!battery.ActiveInHierarchy() && doOnce)
        {
            audio.play("pick up item_metal");
            audio.set3DCoords(listenerpos, "pick up item_metal");
            // Martin (Internal): At least this will help.
            flicker = true;
            lastPlayerRot = player.transform.GetRotation();
        }


        if(flicker)
        {
            doOnce = false;
            showerEvent = true;
            FlickeringLight();
            MoveShowerCurtain();
            flickerOffTimer -= Time.deltaTime;
            if(flickerOffTimer < 0.0f )
            {
                flicker = false;
                bathroomLights.SetActive(flicker);
                audio.stop("shower_running_louderversion");
                // Play light bulb exploding?
            }
        }
        if (showerEvent && !flicker) { 
            if (player.transform.GetRotation().Y >= lastPlayerRot.Y + 120 || player.transform.GetRotation().Y <= lastPlayerRot.Y - 120)
            {
                monster.GetComponent<AnimatedComponent>().SetEnabled(true);
                monster.SetActive(true);
                audio.play("smallscare1");
                audio.set3DCoords(listenerpos, "smallscare1");
                if (player.transform.GetRotation().Y >= lastPlayerRot.Y + 150 || player.transform.GetRotation().Y <= lastPlayerRot.Y - 150)
                {
                    ghostTimer -= Time.deltaTime;
                    if (ghostTimer <= 0.7f)
                    {
                        playerFlashlight.SetActive(false);
                        player.GetComponent<Flashlight_Script>().activateLight = false;
                        audio.play("flashlight off");
                    }
                    if (ghostTimer <= 0.0f )
                    {
                        monster.SetActive(false);
                        showerEvent = false;
                    }
                }
            }
        }
    }

    void FlickeringLight()
    {
        //This chunck of code is not right, by I like the result of the code so yea lul
        flickerTimer += Time.deltaTime;

        if (flickerTimer >= flickerSpeed)
        {
            flickerBool = !flickerBool;
            flickerTimer = 0.0f;
        }

        bathroomLights.SetActive(flickerBool);
    }

    void MoveShowerCurtain()
    {
        audio.play("showerpull");
        audio.set3DCoords(transform.GetPosition(), "showerpull");
        bathroomCurtains.transform.SetPositionX(bathroomCurtains.transform.GetPosition().X + (showerCuratinMoveSpeed * Time.deltaTime));
    }
}