/*!*************************************************************************
****
\file Flashlight_Script.cs
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\par Course: csd3450
\date 9-2-2024
\brief  Script for flashlight
****************************************************************************
***/
using ScriptAPI;
using System;

public class Flashlight_Script : Script
{
    //public Light lightSource;
    public GameObject lightSourceObj;
    public GameObject player;
    public bool activateLight = false;
    public static bool replaceBattery = false;
    public GameDataManager myGameDataManager;
    public string[] flashAudiostr;
    public AudioComponent flashAudio;
    public float followSpeed;
    public static float batteryLife = 100.0f; //start battery at 74% for the flickering light at corridor
    public float batteryTick = 15.0f;
    public GameObject batteryHUD;


    private float tick = 0.0f;
    private float brightness = 1.0f;
    //bool alternate = true;

    private Vector3 lookAmount = new Vector3();

    [SerializeField] private bool flicker = false;
    [SerializeField] private float flickerTimer = 0.0f;
    [SerializeField] public float flickerSpeed = 1.0f;
    [SerializeField] public bool flickerBool = true;

    public override void Awake()
    {
        flashAudio = gameObject.GetComponent<AudioComponent>();
        flashAudiostr = new string[3];
        flashAudiostr[0] = "flashlight on";
        flashAudiostr[1] = "flashlight off";
        flashAudiostr[2] = "flashlight battery restore";
    }

    public override void Start()
    {
        /*if (lightSourceObj.GetComponent<SpotlightComponent>().GetEnabled())
        {
            Vector4 flashlightSettings = new Vector4(5.0f, 0.005f, 0.000f, 0.0f);
            lightSourceObj.GetComponent<SpotlightComponent>().SetAttenuation(flashlightSettings);
        }*/
        flickerTimer = 0.0f;
    }

    public override void Update()
    {
        if (Input.GetKeyDown(Keycode.F))
        {
            activateLight = !activateLight;
            if(activateLight)
            {
                if (flashAudio.finished(flashAudiostr[0]))
                {
                    flashAudio.play(flashAudiostr[0]);
                }
            }
            else
            {
                if (flashAudio.finished(flashAudiostr[1]))
                {
                    flashAudio.play(flashAudiostr[1]);
                }
            }
        }

        if (activateLight)
        {
            lightSourceObj.SetActive(true);
            batteryHUD.SetActive(true);
            BatteryLife();
            if (flicker)
            {
                FlickeringLight();
            }
        }
        else
        {
            lightSourceObj.SetActive(false);
            batteryHUD.SetActive(false);
        }

    }

    public override void LateUpdate()
    {
        if (activateLight)
        {
            Vector3 currentDirection = new Vector3(lightSourceObj.GetComponent<SpotlightComponent>().GetDirection().X, lightSourceObj.GetComponent<SpotlightComponent>().GetDirection().Y, lightSourceObj.GetComponent<SpotlightComponent>().GetDirection().Z);
            Vector3 nextDirection = new Vector3(-player.GetComponent<FPS_Controller_Script>().playerCamera.transform.getForwardVector().X,-player.GetComponent<FPS_Controller_Script>().playerCamera.transform.getForwardVector().Y, player.GetComponent<FPS_Controller_Script>().playerCamera.transform.getForwardVector().Z);
            lookAmount = Vector3.MoveTowards(currentDirection, nextDirection, followSpeed * Time.deltaTime);
            
            Vector4 direction = new Vector4(lookAmount.X, lookAmount.Y, lookAmount.Z, 0.0f);
            lightSourceObj.GetComponent<SpotlightComponent>().SetDirection(direction);
            //lightSourceObj.GetComponent<SpotlightComponent>().GetDirection().Normalize();

        }
    }

    void BatteryLife()
    {
        if (batteryLife <= 0)
        {
            activateLight = false;
        }

        if (activateLight)
        {
            if (batteryLife <= 25)
            {
                flicker = true;
            }
            else
            {
                flicker = false;
            }
            tick += Time.deltaTime;
            if (tick >= batteryTick) 
            {
                batteryLife--;
                tick = 0.0f;
            }
            brightness = 1.0f * (batteryLife / 200.0f);
            Vector4 color = new Vector4(lightSourceObj.GetComponent<SpotlightComponent>().GetColor().X, lightSourceObj.GetComponent<SpotlightComponent>().GetColor().Y, lightSourceObj.GetComponent<SpotlightComponent>().GetColor().Z, brightness);
            lightSourceObj.GetComponent<SpotlightComponent>().SetColor(color);
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

        lightSourceObj.SetActive(flickerBool);
    }
}
