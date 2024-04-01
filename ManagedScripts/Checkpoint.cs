/*!*************************************************************************
****
\file Checkpoint.cs
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\par Course: csd3450
\date 13-02-2024
\brief  Script for button logic
****************************************************************************
***/

using ScriptAPI;
using static GameBlackboard;

public class Checkpoint : Script
{
    //enum CheckpointState
    //{
    //    BedroomCheckpoint,
    //    LivingRoomCheckpoint,
    //    BeforeDiningRoomCheckpoint,
    //    StudyRoomCheckpoint,
    //    FinalCheckpoint
    //}

    #region Painting

    [DontSerializeField] private bool BedroomPaintingEnabled = true;
    public GameObject BedroomPainting; //p07

    #endregion

    #region Player
    [DontSerializeField] private Vector3 PlayerPositonCheckpoint;
    [DontSerializeField] private Vector3 PlayerRotationCheckpoint;

    public GameObject Player;
    #endregion

    #region Monster
    [DontSerializeField] private Vector3 MonsterPositonCheckpoint;
    [DontSerializeField] private GhostMovement.GhostEvent GhostEventCheckpoint;

    public GameObject Monster;
    #endregion

    public DoorState DoorStates;
    public int FinalChaseDoorIndex;

    public GameObject GalleryOtherPainting1; // p01
    public GameObject GalleryOtherPainting2; //p03
    public GameObject GalleryMainPainting; // p02
    public Vector3 GalleryOtherPainting1Position;
    public Vector3 GalleryOtherPainting2Position;
    public Vector3 GalleryMainPaintingPosition;
    public bool GalleryMainPaintingCollected;
    public bool GalleryOtherPainting1Collected;
    public bool GalleryOtherPainting2Collected;
    public bool SwitchClicked; // From gallery switch
    //public GameObject Switch; // From gallery switch

    public void OverrideCheckpoint(GhostMovement.GhostEvent currentEvent)
    {
        Console.WriteLine("Override Checkpoint");

        // Player
        //Console.WriteLine("Player");
        PlayerPositonCheckpoint = Player.transform.GetPosition();
        PlayerRotationCheckpoint = Player.transform.GetRotation();

        // Monster
        //Console.WriteLine("Monster");
        MonsterPositonCheckpoint = Monster.transform.GetPosition();
        GhostEventCheckpoint = currentEvent;

        // Paintings
        BedroomPaintingEnabled = BedroomPainting.GetActive();

        GalleryOtherPainting1Position = GalleryOtherPainting1.transform.GetPosition();
        GalleryOtherPainting2Position = GalleryOtherPainting2.transform.GetPosition();
        GalleryMainPaintingPosition = GalleryMainPainting.transform.GetPosition();

        GalleryMainPaintingCollected = p02.isPaintingCollected;
        GalleryOtherPainting1Collected = p01.isPaintingCollected;
        GalleryOtherPainting2Collected = p03.isPaintingCollected;
        SwitchClicked = GallerySwitch.isActivated;
    }

    public void RevertToCheckpoint()
    {
        Console.WriteLine("Revert Checkpoint");

        // Player
        //Console.WriteLine("Player");
        Quaternion quat = new Quaternion(PlayerRotationCheckpoint);
        Vector3 newPlayerPosition = new Vector3(PlayerPositonCheckpoint.X, Player.transform.GetPosition().Y, PlayerPositonCheckpoint.Z);
        Player.transform.SetPositionX(PlayerPositonCheckpoint.X);
        Player.transform.SetPositionZ(PlayerPositonCheckpoint.Z);
        Player.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(newPlayerPosition, new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(1, 1, 1).Normalize(), new Vector3(1, 1, 1).Normalize());
        Player.transform.SetRotation(PlayerRotationCheckpoint);
        //Console.WriteLine("Saved position: " + PlayerPositonCheckpoint.X + "\t" + PlayerPositonCheckpoint.Z);
        //Console.WriteLine("Player new position: " + Player.transform.GetPosition().X + "\t" + Player.transform.GetPosition().Z);

        // Monster
        //Console.WriteLine("Monster");
        if (GhostEventCheckpoint == GhostMovement.GhostEvent.DiningRoomEvent)
        {
            Monster.transform.SetPosition(new Vector3(-58.0f, Monster.transform.GetPosition().Y, -1060.0f));
            Monster.transform.SetRotation(new Vector3(0.0f, (float)Math.PI, 0.0f));
        }
        else
        {
            Monster.transform.SetPosition(MonsterPositonCheckpoint);
        }
        Monster.GetComponent<GhostMovement>().currentEvent = GhostEventCheckpoint;
        Monster.GetComponent<GhostMovement>().startEvent = true;

        DoorStates.Doors[FinalChaseDoorIndex] = DoorState.State.Locked;

        // Paintings
        BedroomPainting.SetActive(BedroomPaintingEnabled);

        GalleryOtherPainting1.transform.SetPosition(GalleryOtherPainting1Position);
        GalleryOtherPainting2.transform.SetPosition(GalleryOtherPainting2Position);
        GalleryMainPainting.transform.SetPosition(GalleryMainPaintingPosition);
        GalleryMainPainting.GetComponent<RigidBodyComponent>().SetPosition(GalleryMainPaintingPosition);

        p02.isPaintingCollected = GalleryMainPaintingCollected;
        p01.isPaintingCollected = GalleryOtherPainting1Collected;
        p03.isPaintingCollected = GalleryOtherPainting2Collected;
        GallerySwitch.isActivated = SwitchClicked;

        //if (!GalleryMainPaintingCollected)
        //{
        //    GalleryMainPainting.GetComponent<ColliderComponent>().SetEnabled(true);
        //}
        //if (!SwitchClicked)
        //{
        //    Switch.GetComponent<ColliderComponent>().SetEnabled(true);
        //}
    }
}

