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

        // Painting
        BedroomPaintingEnabled = BedroomPainting.GetActive();
    }

    public void RevertToCheckpoint()
    {
        Console.WriteLine("Revert Checkpoint");

        // Player
        //Console.WriteLine("Player");
        Quaternion quat = new Quaternion(PlayerRotationCheckpoint);
        Player.transform.SetPosition(PlayerPositonCheckpoint);
        Player.GetComponent<RigidBodyComponent>().SetPositionRotationAndVelocity(PlayerPositonCheckpoint, new Vector4(quat.X, quat.Y, quat.Z, quat.W), new Vector3(1, 1, 1).Normalize(), new Vector3(1, 1, 1).Normalize());
        Player.transform.SetRotation(PlayerRotationCheckpoint);
        Player.GetComponent<FPS_Controller_Script>().StandUp();
        Player.GetComponent<FPS_Controller_Script>().isCrouched = false;

        Console.WriteLine(PlayerPositonCheckpoint.X + "\t\t" + PlayerPositonCheckpoint.Z);

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

        // Painting
        BedroomPainting.SetActive(BedroomPaintingEnabled);
    }
}

