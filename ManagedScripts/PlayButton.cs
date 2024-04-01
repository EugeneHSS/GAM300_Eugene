/*!*************************************************************************
****
\file playButton.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 22-11-2023
\brief  Script for main menu play button
****************************************************************************
***/
using ScriptAPI;
using System;

public class PlayButton : Script
{
    public AudioComponent bgm;
    private AudioComponent buttonSfx;
    public string bgmName;
    private string buttonSfxName;
    private UISpriteComponent sprite;
    public GameObject blackScreen;
    private bool fading = false;
    private bool inMainMenu = false;
    private bool firstHover = true;
    private float incrementFading = Time.deltaTime / 3f;
    bool withinArea(float mouse, float min, float max)
    {
        bool within = false;
        if (mouse > min && mouse < max)
            within = true;
        return within;
    }

    public override void Awake()
    {
        GraphicsManagerWrapper.ToggleViewFrom2D(true);
        bgmName = "Horror_Menu_Finale_Finale";
        buttonSfxName = "button_press";
        bgm = gameObject.GetComponent<AudioComponent>();
        buttonSfx = gameObject.GetComponent<AudioComponent>();
        sprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Start()
    {
        inMainMenu = true;
    }

    public override void Update()
    {
        if (bgm.finished(bgmName) && inMainMenu)
        {
            bgm.FadeIn(3, bgmName);
        }

        if(sprite.IsMouseCollided() && firstHover)
        {
            //set scale logic here
            firstHover = false;
            buttonSfx.play("buttonhover");
        }
        if(!sprite.IsMouseCollided())
        {
            firstHover = true;
        }
        
        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            fading = true;
            inMainMenu = false;
            buttonSfx.play(buttonSfxName);
            bgm.FadeOut(3, bgmName);
        }

        if (fading == true)
        {
            float alpha = blackScreen.GetComponent<UISpriteComponent>().getColourAlpha();
            alpha += incrementFading;
            alpha = Mathf.Clamp(alpha, 0, 1);
            blackScreen.GetComponent<UISpriteComponent>().setColourAlpha(alpha);
            if (alpha >= 1)
            {
                fading = false;
                SceneLoader.LoadStartingCutscene();
            }
        }
    }

    public override void OnDestroy()
    {
        bgm.stop(bgmName);
    }
}
