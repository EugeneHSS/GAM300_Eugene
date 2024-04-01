/*!*************************************************************************
****
\file OptionsMasterVolume.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 5-03-2024
\brief  Script for settings menu master volume
****************************************************************************
***/
using ScriptAPI;
using System;

public class OptionsMasterVolume : Script
{
    public bool pressedVolumeUp;       // if press the volume up button
    private float MasterVolume;         // float to hold volume value
    private UISpriteComponent sprite;
    private string optionsSoundName;
    private AudioComponent masterVol;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";

    public GameObject masterText;
    public override void Awake()
    {
       // GraphicsManagerWrapper.ToggleViewFrom2D(true);
        optionsSoundName = "basement_music";
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();
        masterVol = gameObject.GetComponent<AudioComponent>();
        sprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Update()
    {
        MasterVolume = masterVol.getMasterVol();

        if (masterVol.finished(optionsSoundName) && MainMenuOptions.isOpened)
        {
            masterVol.play(optionsSoundName);
            Console.WriteLine("playing master vol audio");
        }

        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            if (pressedVolumeUp)
            {
                MasterVolume += 5.0f;
                masterVol.setMasterVol(MasterVolume);
            }
            else
            {
                MasterVolume -= 5.0f;
                masterVol.setMasterVol(MasterVolume);
            }

            MasterVolume = Math.Clamp(MasterVolume, 0, 100);
            masterText.GetComponent<UISpriteComponent>().SetFontMessage(((int)MasterVolume).ToString());
            buttonSfx.play(buttonSfxName);
        }


    }

    public override void OnDestroy()
    {
    }
}

