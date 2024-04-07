/*!*************************************************************************
****
\file AfterGameCredits.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 20-1-2024
\brief  Script for after game credits
****************************************************************************
***/
using ScriptAPI;
using System;

public class AfterGameCredits : Script
{
    string[] textures;
    float counter;
    AudioComponent audio;

    public override void Awake()
    {
        GraphicsManagerWrapper.ToggleViewFrom2D(true);
        textures = new string[9];
        textures[0] = "TeamNameLogo.dds";
        textures[1] = "EndCredit1.dds";
        textures[2] = "EndCredit2.dds";
        textures[3] = "EndCredit3.dds";
        textures[4] = "EndCredit4.dds";
        textures[5] = "EndCredit5.dds";
        textures[6] = "EndCredit6.dds";
        textures[7] = "EndCredit7.dds";
        textures[8] = "EndCredit8.dds";
        counter = 0;
    }

    public override void Update()
    {
        UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();
        counter += Time.deltaTime;
        audio = new AudioComponent();
        audio = gameObject.GetComponent<AudioComponent>();
        audio.play("Horror_Menu_Finale_Finale");

        //Console.WriteLine(counter.ToString());

        if (Input.GetKeyDown(Keycode.SPACE))
        {
            SceneLoader.LoadMainMenu();
        }
        else
        {
            if (counter <= 2.5f)
            {
                Sprite.setColourAlpha(1);
                Sprite.SetTextureName(textures[0]);
            }
            if (counter > 2.5f && counter <= 5.0f)
            {
                Sprite.SetTextureName(textures[1]);
            }
            if (counter > 5.0f && counter <= 7.5f)
            {
                Sprite.SetTextureName(textures[2]);
            }
            if (counter > 7.5f && counter <= 10.0f)
            {
                Sprite.SetTextureName(textures[3]);
            }
            if (counter > 10.0f && counter <= 12.5f)
            {
                Sprite.SetTextureName(textures[4]);
            }
            if (counter > 12.5f && counter <= 15.0f)
            {
                Sprite.SetTextureName(textures[5]);
            }
            if (counter > 15.0f && counter <= 17.5f)
            {
                Sprite.SetTextureName(textures[6]);
            }
            if (counter > 17.5f && counter <= 20.0f)
            {
                Sprite.SetTextureName(textures[7]);
            }
            if (counter > 20.0f && counter <= 22.5f)
            {
                Sprite.SetTextureName(textures[8]);
            }
            if (counter > 22.5f)
            {
                //Console.WriteLine("Switching to mainMenu");
                SceneLoader.LoadMainMenu();
                audio.stop("Horror_Menu_Finale_Finale");
            }

        }
    }
}

