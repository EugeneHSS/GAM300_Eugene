/*!*************************************************************************
****
\file Painting_Script.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script for player interaction with paintings
****************************************************************************
***/
using Microsoft.VisualBasic;
using ScriptAPI;
using System;

public class InteractUI : Script
{
    public static bool isShow;
    public GameObject interactUIText;

    override public void Awake()
    {
       
    }

    public override void Start()
    {
    }

    // Update is called once per frame
    override public void Update()
    {
    }

    public override void LateUpdate()
    {
        interactUIText.SetActive(isShow);
        isShow = false;
    }
}
