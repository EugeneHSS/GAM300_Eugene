/*!*************************************************************************
****
\file Inventory.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script for inventory logic, when player presses 'I' they 
        can toggle inventory.
****************************************************************************
***/
using ScriptAPI;

public class InventoryScript : Script
{
    public GameBlackboard? gameBlackboard;

    public GameObject InventoryObject;
    public View_Object _ViewObjectScript;

    public static List<string> noteObjsInInventory;
    public static List<string> itemObjsInInventory;
    public static List<string> paintingObjsInInventory;

    public static List<string> notesObjsImg;
    public static List<string> itemsObjsImg;
    public static List<string> paintingsObjsImg;

    public static string currentTab;
    public static string currentObjectName;
    public static int currentBox;
    public static bool InventoryIsOpen;
    public static bool IsUseable;

    public bool LockpickIsOpen;

    //public Dictionary<string, string> ItemTexture;
   
    //buttons
    public GameObject ItemsTab;
    public GameObject NotesTab;
    public GameObject PaintingsTab;
    public GameObject UseButton;
    public GameObject ExamineButton;
    public GameObject hidingGameObject;

    static string boxtexture = "A_Inventory Box.dds";
    
    private AudioComponent audio;
    private string audioOpenName = "inventory open";
    private string audioCloseName = "inventory close";
    public override void Awake()
    {
        LockpickIsOpen = false;
    }

    // MAKE SURE INVENTORYOBJECT IS ACTIVE WHEN STARTING THE GAME SO START() RUNS
    public override void Start()
    {
        Console.WriteLine("Start Invenctory");
        initObjects();
    }

    public override void Update()
    {
        var entityID = gameObject.GetEntityID();
        //gameObject.GetComponent<FPS_Controller_Script>().playerCanMove = !(PopupUI.isDisplayed || InventoryIsOpen || hidingGameObject.GetComponent<Hiding>().hiding);
        //gameObject.GetComponent<FPS_Controller_Script>().cameraCanMove = !(PopupUI.isDisplayed || InventoryIsOpen);

        if (Input.GetKeyDown(Keycode.ESC) && gameBlackboard?.gameState == GameBlackboard.GameState.Inventory)
        {
            toggleInventory();
            gameObject.GetComponent<FPS_Controller_Script>().playerCanMove = true;
            gameObject.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
            gameBlackboard.gameState = GameBlackboard.GameState.InGame;
        }

        if (Input.GetKeyDown(Keycode.I) && gameBlackboard?.gameState != GameBlackboard.GameState.Lockpicking)
        {
            Console.WriteLine("I pressed");
            toggleInventory();

            if (gameBlackboard.gameState == GameBlackboard.GameState.InGame)
            {
                gameObject.GetComponent<FPS_Controller_Script>().playerCanMove = false;
                gameObject.GetComponent<FPS_Controller_Script>().cameraCanMove = false;
                gameBlackboard.gameState = GameBlackboard.GameState.Inventory;
            }
            else
            {
                gameObject.GetComponent<FPS_Controller_Script>().playerCanMove = true;
                gameObject.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
                gameBlackboard.gameState = GameBlackboard.GameState.InGame;
            }
        }

        if (InventoryIsOpen) // Inventory opened
        {
            checkMouseInput();
        }
    }

    public void toggleInventory()
    {
        Console.WriteLine("Toggle Inventory\n");
        InventoryIsOpen = !InventoryIsOpen;
        InventoryObject.SetActive(InventoryIsOpen);

        if (InventoryIsOpen)
        {
            audio.play(audioOpenName);
        }       
        else
        {
            audio.play(audioCloseName);
        }
    }

    public void checkMouseInput()
    {
        //Console.WriteLine("Checking Mouse Input\n");
        if (Input.GetMouseButtonDown(Keycode.M1))
        {
            Console.WriteLine("Mouse clicked Inventory");
            if(withinButton(ItemsTab)) // Slightly off in y-axis
            {
                Console.WriteLine("Collide Items");
                currentTab = "Items";
                //itemsTitle.SetActive(true);
                //notesTitle.SetActive(false);
            }
            if(withinButton(NotesTab)) // Slightly off in y-axis
            {
                Console.WriteLine("Collide Notes");
                currentTab = "Notes";
                //itemsTitle.SetActive(false);
                //notesTitle.SetActive(true);
            }
            if (withinButton(PaintingsTab)) // Slightly off in y-axis
            { 
                Console.WriteLine("Collide Paintings");
                currentTab = "Paintings";
            }
            if (withinButton(UseButton)) // Slightly off in y-axis
            {
                Console.WriteLine("Collide Use Button");
                UseObject();

            }
            if (withinButton(ExamineButton)) // Slightly off in y-axis
            {
                Console.WriteLine("Collide Examine Button");
                // do examine stuff
                ExamineObject();
            }
        }
    }

    public void initObjects()
    {
        Console.WriteLine("Init inventory objects\n");
        InventoryObject = GameObjectScriptFind("InventoryObject");

        ItemsTab        = GameObjectScriptFind("ItemsTab");
        NotesTab        = GameObjectScriptFind("NotesTab");
        PaintingsTab    = GameObjectScriptFind("PaintingsTab");
        UseButton = GameObjectScriptFind("UseButton");
        ExamineButton = GameObjectScriptFind("ExamineButton");



        itemsObjsImg = new List<string>
        {
            "Invnt Battery Img.dds",            "Invnt Battery Img.dds",
            "Invnt Battery Img.dds",            "Invnt Battery Img.dds",
            boxtexture,            boxtexture,
            boxtexture,            boxtexture,
            boxtexture,            boxtexture,
            boxtexture,            boxtexture
        };

        itemObjsInInventory = new List<string>
        {
            "Battery",            "Battery",
            "Battery",            "Battery",
            "",            "",
            "",            "",
            "",            "",
            "",            ""
        };

        notesObjsImg = new List<string>
        {
            boxtexture,            boxtexture,
            boxtexture,            boxtexture,
            boxtexture,            boxtexture,
            boxtexture,            boxtexture,
            boxtexture,            boxtexture,
            boxtexture,            boxtexture
        };

        noteObjsInInventory = new List<string>
        {
            "",           "",
            "",           "",
            "",           "",
            "",           "",
            "",           "",
            "",           ""
        };

        paintingsObjsImg = new List<string>
        {
            boxtexture,           boxtexture,
            boxtexture,           boxtexture,
            boxtexture,           boxtexture,
            boxtexture,           boxtexture,
            boxtexture,           boxtexture,
            boxtexture,           boxtexture
        };

        paintingObjsInInventory = new List<string>
        {
            "",            "",
            "",            "",
            "",            "",
            "",            "",
            "",            "",
            "",            ""
        };

        InventoryObject.SetActive(false);
        InventoryIsOpen = false;
        currentTab = "Items";
    }

    public static void addPaintingIntoInventory(string painting_name, string texture_name)
    {
        Console.WriteLine("Adding painting " + texture_name);
        for (int i = 0; i < 12; ++i)
        {
            if (paintingObjsInInventory[i] == "" && paintingsObjsImg[i] == boxtexture)
            {
                paintingObjsInInventory[i] = painting_name;
                paintingsObjsImg[i] = texture_name;
                return;
            }
        }
    }

    public static void addNoteIntoInventory(string note_name, string texture_name)
    {
        Console.WriteLine("Adding note " + texture_name);
        for (int i = 0; i < 12; ++i)
        {
            if (noteObjsInInventory[i] == "" && notesObjsImg[i] == boxtexture)
            {
                noteObjsInInventory[i] = note_name;
                notesObjsImg[i] = texture_name;
                return;
            }
        }
    }

    public static void addItemIntoInventory(string item_name, string texture_name)
    {
        Console.WriteLine("Adding item " + texture_name);
        for(int i = 0; i < 12; ++i)
        {
            if (itemObjsInInventory[i] == "" && itemsObjsImg[i] == boxtexture)
            {
                itemObjsInInventory[i] = item_name;
                itemsObjsImg[i] = texture_name;
                return;
            }
        }
    }
    
    bool withinButton(GameObject obj)
    {
        return obj.GetComponent<UISpriteComponent>().IsMouseCollided();
    }

    void UseObject()
    {
        if (IsUseable && itemObjsInInventory[currentBox] != "")
        {
            string storedObjName = itemObjsInInventory[currentBox];
            itemObjsInInventory[currentBox] = "";
            itemsObjsImg[currentBox] = "A_Inventory Box.dds";
            GameObjectScriptFind("ItemDisplay").GetComponent<UISpriteComponent>().SetTextureName("A_Item Display.dds");

            // Do stuff based on what item is used
            if (storedObjName == "Battery")
            {
                // Do battery logic
                Flashlight_Script.batteryLife = 100.0f;
                AudioComponent audio = gameObject.GetComponent<AudioComponent>();
                audio.play("flashlight battery restore");
            }
            if (storedObjName == "???")
            {
                // Do ??? logic
            }
            toggleInventory();
            gameObject.GetComponent<FPS_Controller_Script>().playerCanMove = true;
            gameObject.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
            gameBlackboard.gameState = GameBlackboard.GameState.InGame;
        }
    }

    void ExamineObject()
    {
        if (currentObjectName != "" && !IsUseable)
        {
            View_Object.ObjectName = currentObjectName;
            View_Object.OnEnter = true;
            GameObjectScriptFind("ObjectViewer").SetActive(true);
        }
    }
}
