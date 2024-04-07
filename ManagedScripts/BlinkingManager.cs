using ScriptAPI;
using System;

public class BlinkingManager : Script 
{
	public GameObject player;
	public GameObject GhostMovement;
	public GameObject GallerySwitchObject;
	public GameObject Receipt;
	public GameObject FatherLetter;
	public GameObject AntonioLetter;
	public GameObject Battery;

	public GameObject MainHall;
	
	public GameObject RightHallWay; // off (Z < -30)

	public GameObject FatherLetterLight;
	public GameObject ReceiptLight;
	public GameObject BedRoomPainting;
	public GameObject BedroomCloset;

	public GameObject BedroomDoor; // off (Z < -700)

	public GameObject ToiletDoor; // off (X < 2000)
	public GameObject BatteryLight; // off Event Bathroom Sink

    public GameObject LivingRoomPainting; // off (X <= 319.0f)
    public GameObject LivingRoomCloset;

	public GameObject KitchenDoor; // off (X < -900

	public GameObject AntonioLetterLight;
	public GameObject StudyRoomCloset;

	public GameObject GalleryPainting1;
	public GameObject GalleryPainting2;
	public GameObject GalleryPainting3;

	public GameObject StudyRoomDoor;
	public GameObject GallerySwitchLight;

	private int eventCounter = 0;
	public override void Awake()
	{
		RightHallWay.GetComponent<Blinking>().SetEnabled(false);
		FatherLetterLight.GetComponent<Blinking>().SetEnabled(false);
		ReceiptLight.GetComponent<Blinking>().SetEnabled(false);
		BedRoomPainting.GetComponent<Blinking>().SetEnabled(false);
		BedroomCloset.GetComponent<Blinking>().SetEnabled(false);
		BedroomDoor.GetComponent<Blinking>().SetEnabled(false);
		ToiletDoor.GetComponent<Blinking>().SetEnabled(false);
		BatteryLight.GetComponent<Blinking>().SetEnabled(false);
		LivingRoomPainting.GetComponent<Blinking>().SetEnabled(false);
		LivingRoomCloset.GetComponent<Blinking>().SetEnabled(false);
		KitchenDoor.GetComponent<Blinking>().SetEnabled(false);
		AntonioLetterLight.GetComponent<Blinking>().SetEnabled(false);
		StudyRoomCloset.GetComponent<Blinking>().SetEnabled(false);
		GalleryPainting1.GetComponent<Blinking>().SetEnabled(false);
        GalleryPainting2.GetComponent<Blinking>().SetEnabled(false);
        GalleryPainting3.GetComponent<Blinking>().SetEnabled(false);
		StudyRoomDoor.GetComponent<Blinking>().SetEnabled(false);
		GallerySwitchLight.GetComponent<Blinking>().SetEnabled(false);
	}
	public override void Update()
	{
		Vector3 movement = player.transform.GetPosition();
        if (FatherLetter.GetActive())
        {
            FatherLetterLight.GetComponent<Blinking>().SetEnabled(false);
            FatherLetterLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
            FatherLetterLight.SetActive(false);
        }
        if (Receipt.GetActive())
        {
            ReceiptLight.GetComponent<Blinking>().SetEnabled(false);
            ReceiptLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
            ReceiptLight.SetActive(false);
        }
        switch ((int)eventCounter)
        {
            case 0: // Deactivate Main Hall Light, activate Hallway Light
                if (movement.X >= 300.0f)
                {
                    MainHall.GetComponent<Blinking>().SetEnabled(false);
                    MainHall.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    RightHallWay.GetComponent<Blinking>().SetEnabled(true);
                    eventCounter++;
                }
                break;
            case 1: // Deactivate Hallway Light, activate FatherLetter, Receipt, BedRoomPainting, BedroomCloset
                if (movement.Z <= -30)
                {
                    RightHallWay.GetComponent<Blinking>().SetEnabled(false);
                    RightHallWay.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    FatherLetterLight.GetComponent<Blinking>().SetEnabled(true);
                    ReceiptLight.GetComponent<Blinking>().SetEnabled(true);
                    BedRoomPainting.GetComponent<Blinking>().SetEnabled(true);
                    BedroomCloset.GetComponent<Blinking>().SetEnabled(true);
                    eventCounter++;
                }
                break;
            case 2: // Deactivate FatherLetter, Receipt, BedRoomPainting (bedroom event)
                if (GhostMovement.GetComponent<GhostMovement>().currentEvent == global::GhostMovement.GhostEvent.BedroomHidingEvent)
                {
                    FatherLetterLight.GetComponent<Blinking>().SetEnabled(false);
                    ReceiptLight.GetComponent<Blinking>().SetEnabled(false);
                    BedRoomPainting.GetComponent<Blinking>().SetEnabled(false);
                    FatherLetterLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
                    ReceiptLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
                    BedRoomPainting.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    eventCounter++;
                }
                break;
            case 3: // Deactivate BedroomCloset, activate BedroomDoor, ToiletDoor, BatteryLight
                if (GhostMovement.GetComponent<GhostMovement>().currentEvent == global::GhostMovement.GhostEvent.Nothing)
                {
                    BedroomCloset.GetComponent<Blinking>().SetEnabled(false);
                    BedroomCloset.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    BedroomDoor.GetComponent<Blinking>().SetEnabled(true);
                    ToiletDoor.GetComponent<Blinking>().SetEnabled(true);
                    BatteryLight.GetComponent<Blinking>().SetEnabled(true);
                    eventCounter++;
                }
                break;
            case 4: // Prep for jumpscare in toilet
                if (!Battery.GetActive())
                {
                    BatteryLight.GetComponent<Blinking>().SetEnabled(false);
                    BedroomDoor.GetComponent<Blinking>().SetEnabled(false);
                    BatteryLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
                    BedroomDoor.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    eventCounter++;
                }
                break;
            case 5: // Deactivate ToiletDoor, activate LivingRoomPainting, LivingRoomCloset
                if (movement.X <= 2000)
                {
                    ToiletDoor.GetComponent<Blinking>().SetEnabled(false);
                    ToiletDoor.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    LivingRoomPainting.GetComponent<Blinking>().SetEnabled(true);
                    eventCounter++;
                }
                break;
            case 6: // Deactivate LivingRoomPainting, LivingRoomCloset, activate KitchenDoor
                if (movement.X <= 319.0f)
                {
                    LivingRoomPainting.GetComponent<Blinking>().SetEnabled(false);
                    LivingRoomCloset.GetComponent<Blinking>().SetEnabled(false);
                    LivingRoomPainting.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
                    LivingRoomCloset.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    KitchenDoor.GetComponent<Blinking>().SetEnabled(true);
                    eventCounter++;
                }
                break;
            case 7: // Deactivate KitchenDoor, activate AntonioLetterLight, StudyRoomCloset
                if (movement.X <= -900)
                {
                    KitchenDoor.GetComponent<Blinking>().SetEnabled(false);
                    KitchenDoor.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    AntonioLetterLight.GetComponent<Blinking>().SetEnabled(true);
                    StudyRoomCloset.GetComponent<Blinking>().SetEnabled(true);
                    eventCounter++;
                }
                break;
            case 8: // Set AntonioLetterLight to be 1 after interacting with AntonioLetter, enable painting light
                if (AntonioLetter.GetActive())
                {
                    AntonioLetterLight.GetComponent<Blinking>().SetEnabled(false);
                    AntonioLetterLight.GetComponent<PointlightComponent>().SetColorAlpha(1.0f);

                    GalleryPainting1.GetComponent<Blinking>().SetEnabled(true);
                    GalleryPainting2.GetComponent<Blinking>().SetEnabled(true);
                    GalleryPainting3.GetComponent<Blinking>().SetEnabled(true);
                    eventCounter++;
                } // At individual painting script, turn off the gallery painting light after interacting
                break;
            case 9: // GallerySwitch activated, activate studyroomdoor
                if (GallerySwitch.isActivated)
                {
                    GallerySwitchLight.GetComponent<Blinking>().SetEnabled(false);
                    GallerySwitchLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);

                    StudyRoomDoor.GetComponent<Blinking>().SetEnabled(true);

                    eventCounter++;
                }
                break;
            case 10: // chasing event, deactivate all blinking lights and leave the rest to other script to handle
                if (GhostMovement.GetComponent<GhostMovement>().currentEvent == global::GhostMovement.GhostEvent.FinalChasingEvent)
                {
                    deactivateAndDisableAllLights();
                    eventCounter++;
                }
                break;
        }
    }
    void deactivateAndDisableAllLights()
    {
        RightHallWay.GetComponent<Blinking>().SetEnabled(false);
        RightHallWay.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        RightHallWay.SetActive(false);
        FatherLetterLight.GetComponent<Blinking>().SetEnabled(false);
        FatherLetterLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        FatherLetterLight.SetActive(false);
        ReceiptLight.GetComponent<Blinking>().SetEnabled(false);
        ReceiptLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        ReceiptLight.SetActive(false);
        BedRoomPainting.GetComponent<Blinking>().SetEnabled(false);
        BedRoomPainting.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        BedRoomPainting.SetActive(false);
        BedroomCloset.GetComponent<Blinking>().SetEnabled(false);
        BedroomCloset.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        BedroomCloset.SetActive(false);
        BedroomDoor.GetComponent<Blinking>().SetEnabled(false);
        BedroomDoor.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        BedroomDoor.SetActive(false);
        ToiletDoor.GetComponent<Blinking>().SetEnabled(false);
        ToiletDoor.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        ToiletDoor.SetActive(false);
        BatteryLight.GetComponent<Blinking>().SetEnabled(false);
        BatteryLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        BatteryLight.SetActive(false);
        LivingRoomPainting.GetComponent<Blinking>().SetEnabled(false);
        LivingRoomPainting.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        LivingRoomPainting.SetActive(false);
        LivingRoomCloset.GetComponent<Blinking>().SetEnabled(false);
        LivingRoomCloset.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        LivingRoomCloset.SetActive(false);
        KitchenDoor.GetComponent<Blinking>().SetEnabled(false);
        KitchenDoor.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        KitchenDoor.SetActive(false);
        AntonioLetterLight.GetComponent<Blinking>().SetEnabled(false);
        AntonioLetterLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        AntonioLetterLight.SetActive(false);
        StudyRoomCloset.GetComponent<Blinking>().SetEnabled(false);
        StudyRoomCloset.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        StudyRoomCloset.SetActive(false);
        GalleryPainting1.GetComponent<Blinking>().SetEnabled(false);
        GalleryPainting1.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        GalleryPainting1.SetActive(false);
        GalleryPainting2.GetComponent<Blinking>().SetEnabled(false);
        GalleryPainting2.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        GalleryPainting2.SetActive(false);
        GalleryPainting3.GetComponent<Blinking>().SetEnabled(false);
        GalleryPainting3.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        GalleryPainting3.SetActive(false);        
        GallerySwitchLight.GetComponent<Blinking>().SetEnabled(false);
        GallerySwitchLight.GetComponent<PointlightComponent>().SetColorAlpha(0.0f);
        GallerySwitchLight.SetActive(false);
    }

 }