/*!*************************************************************************
****
\file DoorState.cs
\author Elton Teo
\par DP email: e.teo@digipen.edu
\par Course: csd3450
\date 10-12-2023
\brief  States for doors
****************************************************************************
***/
using ScriptAPI;
using System;
public class DoorState : Script
{
    public enum State
    {
        Unlocked,
        Locked,
        ForcedLock
    }

    public State[] Doors;
    public bool doorLookedAt = false;
    public GameObject doorText;

    public override void Awake()
    {
        Doors = new State[14];
        for (int i = 0; i < Doors.Length; i++)
        {
            Doors[i] = State.Unlocked;
        }
        Doors[0] = State.Locked;
        Doors[1] = State.Locked;
        Doors[3] = State.Locked;
        Doors[10] = State.Locked;
    }
    public override void LateUpdate()
    {
        if (doorLookedAt)
        {
            doorText.SetActive(true);
        }
        else
        {
            doorText.SetActive(false);
        }
        doorLookedAt = false;
    }
}
