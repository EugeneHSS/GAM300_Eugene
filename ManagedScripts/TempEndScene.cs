using ScriptAPI;
using System;
public class TempEndScene : Script
{
    private bool fadeIn = false;
    private bool fadeOut = true;
    private bool once = true;
    private float incrementFading = Time.deltaTime / 5f;
    readonly string runningSFX = "end_grassrunning";
    readonly string monsterSFX = "mon_death";
    AudioComponent audio;
    private float waitTimer;
    String[] sfxNames;
    String[] Textures;
    String[] changeSlideSfx;
    int sfxIndex;

    public override void Awake()
    {
        audio = new AudioComponent();
        audio = gameObject.GetComponent<AudioComponent>();
        audio.stop("ambientdrone1");
        audio.play("end_cutscene_bgm");
        //audio.FadeOut(3, monsterSFX);
        waitTimer = 5.0f; //duration of end cutscene
        sfxIndex = 0;
        sfxNames = new String[5];
        sfxNames[0] = "end_scene1";
        sfxNames[1] = "end_scene2";
        sfxNames[2] = "end_scene3";
        sfxNames[3] = "end_scene4";
        sfxNames[4] = "end_scene5";
        audio.play(sfxNames[sfxIndex]);

        Textures = new string[5];
        Textures[0] = "End_cutscene1.dds";
        Textures[1] = "End_cutscene2.dds";
        Textures[2] = "End_cutscene3.dds";
        Textures[3] = "End_cutscene4.dds";
        Textures[4] = "End_cutscene5.dds";

        changeSlideSfx = new string[5];
        changeSlideSfx[0] = "slidechange1";
        changeSlideSfx[1] = "slidechange2";
        changeSlideSfx[2] = "slidechange3";
        changeSlideSfx[3] = "slidechange4";
        changeSlideSfx[4] = "slidechange5";

    }
    public override void Start()
    {

    }
    public override void Update()
    {
        audio = gameObject.GetComponent<AudioComponent>();
        UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();


        if (audio.finished(sfxNames[sfxIndex])&& sfxIndex < 4)
        {
            sfxIndex++;
            audio.play(sfxNames[sfxIndex]);
            audio.play(changeSlideSfx[sfxIndex]);
            Sprite.SetTextureName(Textures[sfxIndex]);//change texture

        }

        if (once)
        {
            audio.play(runningSFX);
            once = false;
            Input.HideMouse(true);
            Input.Lock(true);
        }

        if (fadeIn == true)
        {
            float alpha = gameObject.GetComponent<UISpriteComponent>().getColourAlpha();
            alpha -= incrementFading;
            alpha = Mathf.Clamp(alpha, 0, 1);
            gameObject.GetComponent<UISpriteComponent>().setColourAlpha(alpha);
            if (alpha <= 0)
            {
                if (waitTimer <= 0.0f)
                {
                    fadeIn = false;
                    //SceneLoader.LoadTempJumpscare();
                    SceneLoader.LoadEndGameCredits();
                    audio.stop("end_cutscene_bgm");
                }
                waitTimer -= Time.deltaTime;
            }
        }
        if (fadeOut == true)
        {            
            float alpha = gameObject.GetComponent<UISpriteComponent>().getColourAlpha();
            alpha += incrementFading;
            alpha = Mathf.Clamp(alpha, 0, 1);
            gameObject.GetComponent<UISpriteComponent>().setColourAlpha(alpha);
            if (alpha >= 1)
            {
                fadeOut = false;
                
                audio.FadeOut(3, runningSFX);
            }
        }
        if (sfxIndex == 4 && audio.finished(sfxNames[4]))
        {
            fadeIn = true;

        }
    }
    public override void LateUpdate()
    {

    }

}