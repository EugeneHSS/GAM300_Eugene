using ScriptAPI;
using System;
using System.Diagnostics.Metrics;

public class Tutorial : Script
{
    AudioComponent audioPlayer;
    String[] Subtitles;
    String[] LockSoundEffects;
    String[] lockVO;
    public static int counter;

    [SerializeField]
    private GameObject sample_lock_group;
    private GameObject sample_lock;
    private GameObject sample_pick;
    private UISpriteComponent mini_screen;


    public override void Awake()
    {
        Subtitles = new String[9];
        Subtitles[0] = "Martin (Internal): Hopefully, I won\'t forget how to do this.";
        Subtitles[1] = "Martin (Internal): Alright, looks like I\'m in.";
        Subtitles[2] = "Martin (Internal): No turning back now.";
        Subtitles[3] = "Martin (Internal): That was too loud... I better not screw up again.";
        Subtitles[4] = "";
        Subtitles[5] = "Move [mouse] to adjust pick";
        Subtitles[6] = "Press [E] to turn lock";
        Subtitles[7] = "Martin (Internal): Find the right spot, and it should click...";
        Subtitles[8] = "Martin (Internal): There, now to turn the lock.";

        LockSoundEffects = new String[3];
        LockSoundEffects[0] = "lockpick_turn";
        LockSoundEffects[1] = "lockpick success";
        LockSoundEffects[2] = "lockpick_failtry";

        lockVO = new String[7];
        lockVO[0] = "pc_lockpickstart";
        lockVO[1] = "pc_lockpicksuccess1";
        lockVO[2] = "pc_lockpicksuccess2";
        lockVO[3] = "pc_lockpickfail";
        lockVO[4] = "pc_findtherightspot";
        lockVO[5] = "pc_turnthelock";
        lockVO[6] = "";
    }

    public override void Start()
    {

    }

    public override void Update()
    {
        
    }

    float toDegree(float radians)
    {
        return radians * (180 / 3.1415926535897931f);
    }
    float toRadians(float degree)
    {
        return degree * (3.1415926535897931f / 180);
    }
}