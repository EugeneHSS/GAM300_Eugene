/*!*************************************************************************
****
\file OptionsMouseSensitivity.cs
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\par Course: csd3450
\date 23-03-2024
\brief  Script for settings mouse sensitivity in options menu
****************************************************************************
***/

using ScriptAPI;
using System;

public class OptionsMouseSensitivity : Script
{
    public bool pressedMouseUp;
    private float mouseValue;
    private UISpriteComponent sprite;
    private AudioComponent buttonSfx;
    private string buttonSfxName = "";

    public GameObject mouseText;

    public override void Awake()
    {
        buttonSfxName = "button_press";
        buttonSfx = gameObject.GetComponent<AudioComponent>();
        sprite = gameObject.GetComponent<UISpriteComponent>();
    }

    public override void Update()
    {
        mouseValue = Input.GetSensitivity();
        mouseValue = Math.Clamp(mouseValue, 0.1f, 2.0f);

        if (Input.GetMouseButtonDown(Keycode.M1) && sprite.IsMouseCollided())
        {
            if(pressedMouseUp) 
            {
                mouseValue += 0.1f;
            }
            else 
            {
                mouseValue -= 0.1f;                
            }
            mouseValue = Math.Clamp(mouseValue, 0.1f, 2.0f);
            Input.SetSensitivity(mouseValue);
            buttonSfx.play(buttonSfxName);
        }
        mouseText.GetComponent<UISpriteComponent>().SetFontMessage((Math.Round(mouseValue, 2)).ToString("F2"));
    }
}
