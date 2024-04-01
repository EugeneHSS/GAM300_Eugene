using ScriptAPI;
using System;

public class CinematicScript : Script 
{
	public GameObject cinematicCamera;
	public GameObject playerCamera;
	public GameObject gallerySwitch;
	private bool doOnce = false;
	public override void Awake()
	{

	}
	public override void Update()
	{
		if (GallerySwitch.isGallerySwitchActivated && doOnce == false)
		{
			playerCamera.SetActive(false);
			cinematicCamera.SetActive(true);
			cinematicCamera.transform.SetPosition(new Vector3(400, 100, 370));

			doOnce = true;
		}
        if (doOnce)
        {
			cinematicCamera.transform.SetPosition(new Vector3(Mathf.Lerp(cinematicCamera.transform.GetPosition().X, 2200, 1.0f * Time.deltaTime),
				cinematicCamera.transform.GetPosition().Y, cinematicCamera.transform.GetPosition().Z));
        }
		if (GallerySwitch.isGallerySwitchActivated == false)
		{
			cinematicCamera.SetActive(false);
			playerCamera.SetActive(true);
		}
    }
 }