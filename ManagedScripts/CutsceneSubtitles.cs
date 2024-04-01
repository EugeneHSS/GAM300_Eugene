/*!*************************************************************************
****
\file CutsceneSubtitles.cs
\author Thea Sea
\par DP email: thea.sea@digipen.edu
\par Course: csd3450
\date 24-11-2023
\brief  Script for subtitles
****************************************************************************
***/
using ScriptAPI;
using System;

public class CutsceneSubtitle : Script
{
    String[] Audiofiles;
    String BGMfile;
    String[] Subtitles;
    [SerializeField]
    public static int counter;
    public static bool next = true;
    private static bool skip = false;
    public override void Awake()
    {
        BGMfile = "cutscene_music_and_sfx_only";
        Audiofiles = new String[17];
        Subtitles = new String[17];
        GraphicsManagerWrapper.ToggleViewFrom2D(true);
        Subtitles[0] = "Father: My Son, if you are reading this, then I am dead,";
        Subtitles[1] = "Father: ...and I've left you with a terrible debt. But you don't have to be";
        Subtitles[2] = "Father: Your mother, God rest her soul, didn't tell us a lot about her family";
        Subtitles[3] = "Father: ...or their wealth";
        Subtitles[4] = "Father: She never mentioned them at all";

        Subtitles[5] = "Father: So imagine my surprise when I discovered their history";

        Subtitles[6] = "Father: I have seen what wealth they hold";
        Subtitles[7] = "Father: How much they have selfishly kept from us";

        Subtitles[8] = "Father: Believe me, I have tried to take what we were owed when her father died";
        Subtitles[9] = "Father: But I am not of their blood. I was always an outsider";

        Subtitles[10] = "Father: But you have their blood running within you";
        Subtitles[11] = "Father: And you deserve what was kept from you. Your rightful inheritance";

        Subtitles[12] = "Father: So my final gift to you, my son, is simply what should have been given to you before";
        Subtitles[13] = "Father: And if they still would not give, then take it, if you must";

        Subtitles[14] = "Father: By whatever means necessary";

        Subtitles[15] = "Father: Remember, no matter what they tell you";

        Subtitles[16] = "Father: You'll always be part of the family";

        Audiofiles[0] = "intro1_1";
        Audiofiles[1] = "intro1_2";
        Audiofiles[2] = "intro2_1";
        Audiofiles[3] = "intro2_2";
        Audiofiles[4] = "intro2_3";
        Audiofiles[5] = "intro3_1";
        Audiofiles[6] = "intro4_1";
        Audiofiles[7] = "intro4_2";
        Audiofiles[8] = "intro5_1";
        Audiofiles[9] = "intro5_2";
        Audiofiles[10] = "intro6_1";
        Audiofiles[11] = "intro6_2";
        Audiofiles[12] = "intro7_1";
        Audiofiles[13] = "intro8_1";
        Audiofiles[14] = "intro8_2";
        Audiofiles[15] = "intro9_1";
        Audiofiles[16] = "intro9_2";

        counter = 0;
        next = true;
    }

    public override void Update()
    {
        UISpriteComponent Sprite = gameObject.GetComponent<UISpriteComponent>();
        AudioComponent audio = gameObject.GetComponent<AudioComponent>();
        if (counter > 1 && counter < 5)
        {
            Vector4 white = new Vector4(255.0f, 255.0f, 255.0f, 1.0f);
            Sprite.SetFontColour(white);
            Sprite.SetFontBackgroundColor(white);
        }
        if (counter >= 5)
        {
            Vector4 black = new Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            Sprite.SetFontColour(black);
            Sprite.SetFontBackgroundColor(black);

        }

        if (Input.GetKeyDown(Keycode.SPACE))
        {
            audio.stop(Audiofiles[counter]);
            skip = true;
            counter++;
        }

        else
        {
            audio.play(BGMfile);
            //audio.playplay();

            if (counter > 16)//cutscene over
            {
                audio.FadeOut(3, BGMfile);
                GraphicsManagerWrapper.ToggleViewFrom2D(false);
                SceneLoader.LoadMainGame();
            }
            else
            {
                if (next)
                {
                    Sprite.SetFontMessage(Subtitles[counter]);
                    audio.play(Audiofiles[counter]);
                    next = false;
                }
                else if (audio.finished(Audiofiles[counter]))
                {
                    next = true;
                    if (!skip)
                    {
                        ++counter;
                    }
                    skip = false;
                }
            }
        }
    }
}