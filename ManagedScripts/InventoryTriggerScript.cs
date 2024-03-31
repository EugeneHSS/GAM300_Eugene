using ScriptAPI;
using System;
public class InventoryTriggerScript : Script 
{
    public GameObject inventoryIntScript;
    public int index = -1; // 0 is receipt, 1 is to father, 2 is to antonio
    public GameObject interectUI;
    public override void Awake()
	{
		
	}
	public override void Update()
	{
		if (gameObject.GetComponent<RigidBodyComponent>().IsRayHit())
		{
            InteractUI.isShow = true;
            if (Input.GetKeyDown(Keycode.E))
            {
                switch (index)
                {
                    case 0:
                        inventoryIntScript.GetComponent<InventoryIntScript>().showReceipt = true;
                        break;
                    case 1:
                        inventoryIntScript.GetComponent<InventoryIntScript>().showToFather = true;
                        break;
                    case 2:
                        inventoryIntScript.GetComponent<InventoryIntScript>().showToAntonio = true;
                        break;
                    default:
                        break;
                }
            }
		}
	}
 }