using ScriptAPI;
using System;

public class AmbientSound : Script
{
    private String bgm;
    private AudioComponent play;
    private GameObject ob;

    public override void Awake()
    {
        ob = gameObject;
        bgm = new String("window_ambience");
    }

    public override void Start()
    {
        play = gameObject.GetComponent<AudioComponent>();
    }

    public override void Update()
    {
        play.set3DCoords(ob.GetComponent<TransformComponent>().GetPosition(), bgm);
        play.play(bgm);
        play.setVolume(0.5f, bgm);
    }


}
