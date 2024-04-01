/*!*************************************************************************
****
\file InventoryBox.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script for each inventory slot 
****************************************************************************
***/
using ScriptAPI;
using System;
using static System.Net.Mime.MediaTypeNames;

public class InventoryBox : Script
{
    [SerializeField]
    public int BoxNumber;
    public string storedObjName;
    public string storedObjTexture;

    GameObject notesTitle;
    GameObject itemsTitle;
    GameObject objectivesTitle;

    //public Sprite emptyBox;

    public override void Awake()
    {
        //GetComponent<Button>().onClick.AddListener(AssignViewObjectString);
        itemsTitle = GameObjectScriptFind("ItemsTitle");
        notesTitle = GameObjectScriptFind("NotesTitle");
        objectivesTitle = GameObjectScriptFind("ObjectivesTitle");
    }

    public override void Start()
    {
    }

    public override void Update()
    {
        DisplayItem();
        if(Clicked())
        {
            InventoryScript.IsUseable = InventoryScript.currentTab == "Items" ? true : false;
            GameObjectScriptFind("ItemDisplay").GetComponent<UISpriteComponent>().SetTextureName(storedObjTexture);
            GameObjectScriptFind("ItemName").GetComponent<UISpriteComponent>().SetFontMessage(storedObjName);
            InventoryScript.currentObjectName = storedObjName;
            InventoryScript.currentBox = BoxNumber;
        }
    }

    public void DisplayItem()
    {
        if(InventoryScript.currentTab == "Items")
        {
            gameObject.GetComponent<UISpriteComponent>().SetTextureName(InventoryScript.itemsObjsImg[BoxNumber]);
            storedObjName = InventoryScript.itemObjsInInventory[BoxNumber];
            storedObjTexture = InventoryScript.itemsObjsImg[BoxNumber];
            itemsTitle.SetActive(true);
            notesTitle.SetActive(false);
            objectivesTitle.SetActive(false);
        }
        else if (InventoryScript.currentTab == "Paintings")
        {
            gameObject.GetComponent<UISpriteComponent>().SetTextureName(InventoryScript.paintingsObjsImg[BoxNumber]);
            storedObjName = InventoryScript.paintingObjsInInventory[BoxNumber];
            storedObjTexture = InventoryScript.paintingsObjsImg[BoxNumber];
            itemsTitle.SetActive(false);
            notesTitle.SetActive(false);
            objectivesTitle.SetActive(true);
        }
        else if (InventoryScript.currentTab == "Notes")
        {
            gameObject.GetComponent<UISpriteComponent>().SetTextureName(InventoryScript.notesObjsImg[BoxNumber]);
            storedObjName = InventoryScript.noteObjsInInventory[BoxNumber];
            storedObjTexture = InventoryScript.notesObjsImg[BoxNumber];
            itemsTitle.SetActive(false);
            notesTitle.SetActive(true);
            objectivesTitle.SetActive(false);
        }
    }

    public bool Clicked()
    {       
        return Input.GetKeyDown(Keycode.M1) && gameObject.GetComponent<UISpriteComponent>().IsMouseCollided();
    }
}