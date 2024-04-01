using ScriptAPI;
using System;

public class InventoryIntScript : Script 
{
	
	public GameObject receipt;
	public GameObject ToFather;
	public GameObject ToAntonio;
	public GameObject player;
	public GameObject playerCamera;
    public GameBlackboard gameBlackboard;
	public GameObject interectCam;
	private bool cameraDunMove;
	private Vector2 offset;
	private float PI = 3.14159f;
	private float PI_2 = 1.5708f;
	private bool doOnce = false;
	private float mouseDeltaX;
	private float mouseDeltaY;

	public bool showReceipt = false;
	public bool showToFather = false;
	public bool showToAntonio = false;

    public override void Awake()
	{

	}
	public override void Update()
	{
		//if (Input.GetKeyDown(Keycode.NUM1))
		//{
		//	bool status = receipt.GetActive();
		//	receipt.SetActive(!status);
		//	receipt.GetComponent<TransformComponent>().SetPosition(player.GetComponent<TransformComponent>().GetPosition());
		//}
		if (doOnce)
		{
            player.GetComponent<FPS_Controller_Script>().playerCanMove = false;
            player.GetComponent<FPS_Controller_Script>().cameraCanMove = false;
            doOnce = false;
		}
        if (cameraDunMove)
		{
			// use WASD to move the object
			offset = new Vector2(0.0f, 0.0f);
			if (Input.GetKey(Keycode.W))
			{
				offset.Y += 0.1f;
			}
			if (Input.GetKey(Keycode.S))
			{
                offset.Y -= 0.1f;
            }
			if (Input.GetKey(Keycode.A))
			{
				offset.X += 0.1f;
            }
			if (Input.GetKey(Keycode.D))
			{
                offset.X -= 0.1f;
			}

			// too lazy to optimize
            ToFather.GetComponent<TransformComponent>().SetPosition(ToFather.GetComponent<TransformComponent>().GetPosition() - new Vector3(0f, offset.Y, 0f));
            ToFather.GetComponent<TransformComponent>().SetRotation(ToFather.GetComponent<TransformComponent>().GetRotation() - new Vector3(0f, offset.X, 0f));
            ToAntonio.GetComponent<TransformComponent>().SetPosition(ToAntonio.GetComponent<TransformComponent>().GetPosition() - new Vector3(0f, offset.Y, 0f));
            ToAntonio.GetComponent<TransformComponent>().SetRotation(ToAntonio.GetComponent<TransformComponent>().GetRotation() - new Vector3(0f, offset.X, 0f));
            receipt.GetComponent<TransformComponent>().SetPosition(receipt.GetComponent<TransformComponent>().GetPosition() - new Vector3(0f, offset.Y, 0f));
            receipt.GetComponent<TransformComponent>().SetRotation(receipt.GetComponent<TransformComponent>().GetRotation() - new Vector3(0f, offset.X, 0f));

        }
        if (Input.GetKeyDown(Keycode.ESC) && gameBlackboard.gameState == GameBlackboard.GameState.InventoryInteract)
		{
            GraphicsManagerWrapper.ToggleViewFrom2D(false);
			gameBlackboard.gameState = GameBlackboard.GameState.InGame;

			interectCam.SetActive(false);
			playerCamera.SetActive(true);

            ToFather.SetActive(false);
			ToAntonio.SetActive(false);
			receipt.SetActive(false);

            // reset
            player.GetComponent<FPS_Controller_Script>().playerCanMove = true;
            player.GetComponent<FPS_Controller_Script>().cameraCanMove = true;
            cameraDunMove = false;
			offset = new Vector2(0.0f, 0.0f);
			showReceipt = false;
			showToFather = false;
			showToAntonio = false;

			// safety net
			if (player.transform.GetPosition().Y != 90)
			{
				player.transform.SetPositionY(90);
				player.GetComponent<RigidBodyComponent>().SetPosition(player.transform.GetPosition());
			}

        }
		if (showToFather && cameraDunMove == false)
		{
			GraphicsManagerWrapper.ToggleViewFrom2D(true);
			gameBlackboard.gameState = GameBlackboard.GameState.InventoryInteract;
			playerCamera.SetActive(false);
			interectCam.SetActive(true);

            //ToFather.GetComponent<GraphicComponent>().SetShowMesh(true);
			ToFather.SetActive(true);
			ToFather.GetComponent<TransformComponent>().SetPosition(new Vector3(0, -1, 1));
			ToFather.GetComponent<TransformComponent>().SetRotation(new Vector3(PI_2, PI, 0));

			cameraDunMove = true;
			doOnce = true;
		}
		else if (showToAntonio && cameraDunMove == false)
		{
			GraphicsManagerWrapper.ToggleViewFrom2D(true);
			gameBlackboard.gameState = GameBlackboard.GameState.InventoryInteract;
			playerCamera.SetActive(false);
			interectCam.SetActive(true);

            ToAntonio.SetActive(true);
            ToAntonio.GetComponent<TransformComponent>().SetPosition(new Vector3(0, -1, 1));
			ToAntonio.GetComponent<TransformComponent>().SetRotation(new Vector3(PI_2, PI, 0));

			cameraDunMove = true;
			doOnce = true;
		}
		else if (showReceipt && cameraDunMove == false)
		{
			GraphicsManagerWrapper.ToggleViewFrom2D(true);
			gameBlackboard.gameState = GameBlackboard.GameState.InventoryInteract;
			playerCamera.SetActive(false);
			interectCam.SetActive(true);

			receipt.SetActive(true);
			receipt.GetComponent<TransformComponent>().SetPosition(new Vector3(0, -1, 1));
			receipt.GetComponent<TransformComponent>().SetRotation(new Vector3(PI_2, PI, 0));

			cameraDunMove = true;
			doOnce = true;
		}
		if (Input.GetMouseButton(Keycode.M1))
        {
			if (mouseDeltaX != Input.GetMouseDeltaX() && mouseDeltaY != Input.GetMouseDeltaY())
			{
				float rotationAngleXRadian = ToFather.GetComponent<TransformComponent>().GetRotation().X;
                float rotationAngleYRadian = ToFather.GetComponent<TransformComponent>().GetRotation().Y;
                rotationAngleXRadian += Input.GetMouseDeltaY() * PI / 180.0f;
                rotationAngleYRadian += Input.GetMouseDeltaX() * PI / 180.0f;

				ToFather.GetComponent<TransformComponent>().SetRotationX(rotationAngleXRadian);
                ToFather.GetComponent<TransformComponent>().SetRotationY(rotationAngleYRadian);
            }
            mouseDeltaX = Input.GetMouseDeltaX();
			mouseDeltaY = Input.GetMouseDeltaY();
        }

    }


 }