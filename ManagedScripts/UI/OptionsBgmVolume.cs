/*!*************************************************************************
****
\file OptionsBgmVolume.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 5-33-2024
\brief  Script for settings menu BGM volume
****************************************************************************
***/
using ScriptAPI;
using System;

public class OptionsBgmVolume : Script
{
    public bool BGMpressedVolUp;
    private float BgmVolume;
    private UISpriteComponent sprite;
    private AudioComponent bgmVol;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";
    public GameObject bgmText;

    public override void Awake()
    {
        sprite = gameObject.GetComponent<UISpriteComponent>();
        bgmVol = gameObject.GetComponent<AudioComponent>();
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();

    }

    public override void Update()
    {
        BgmVolume = bgmVol.getBGMVol();

        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            if (BGMpressedVolUp)
            {
                BgmVolume += 5.0f;
                bgmVol.setBGMVol(BgmVolume);
            }
            else
            {
                BgmVolume -= 5.0f;
                bgmVol.setBGMVol(BgmVolume);
            }
            //Console.WriteLine(bgmVol.getBGMVol());
            BgmVolume = Math.Clamp(BgmVolume, 0, 100);
            bgmText.GetComponent<UISpriteComponent>().SetFontMessage(((int)BgmVolume).ToString());
            buttonSfx.play(buttonSfxName);
        }
    }

    public override void OnDestroy()
    {
    }
}

