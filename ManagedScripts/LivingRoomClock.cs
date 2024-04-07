using ScriptAPI;
using System;

public class LivingRoomClock : Script
{
    String sound;
    AudioComponent audio;
    public override void Awake()
    {
        audio = new AudioComponent();

        sound = new String("livingroom_clock");
    }

    public override void Update()
    {
        //audio.play(sound);
        audio.set3DCoords(gameObject.GetComponent<TransformComponent>().GetPosition(), sound);
    }
}
