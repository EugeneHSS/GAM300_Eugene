/*!*************************************************************************
****
\file ViewObject.cs
\author Celine Leong
\par DP email: jiayiceline.leong@digipen.edu
\par Course: csd3450
\date 15-1-2024
\brief  Gameplay script inspecting objects
****************************************************************************
***/
using ScriptAPI;
using System;
using System.Diagnostics;

public class View_Object : Script
{
    GameObject ViewingModel;
    CameraComponent ObjectViewerCam;
    
    //public static string ModelName;
    public static string ObjectName;
    public static bool OnEnter;
    public static bool isExamining;
    
    float root_y;
    float rotateValueY, rotateValueX;
    Vector3 originalPos, originalRot;
    bool isZoomed;

    public override void Awake()
    {
        ObjectViewerCam = gameObject.GetComponent<CameraComponent>();
    }

    public override void Start()
    {
        Console.WriteLine("Init ObjectViewer\n");
        gameObject.SetActive(false);
    }
    public override void Update()
    {
        if (OnEnter)
        {
            Console.WriteLine("On Enter");
            GameObjectScriptFind("InventoryObject").SetActive(false);
            InventoryScript.InventoryIsOpen = false;

            //ObjectViewerModel.GetComponent<GraphicComponent>().SetModelName("cube_Bin.bin"); // Not working.. probably becaus some models have many entities
            ViewingModel = GameObjectScriptFind(ObjectName);
            //ViewingModel.transform.SetPosition(ObjectViewerModel.transform.GetPosition());
            ViewingModel.SetActive(true);

            ObjectViewerCam.SetFieldOfView(60f);
            Vector3 objectSize = ViewingModel.GetComponent<BoxColliderComponent>().GetSize();
            Vector3 offsetScale = ViewingModel.GetComponent<BoxColliderComponent>().GetOffsetScale();
            objectSize.X /= (offsetScale.X + 1);
            objectSize.Y /= (offsetScale.Y + 1);
            objectSize.Z /= (offsetScale.Z + 1);
            float biggestAxis = Mathf.Max(Mathf.Max(objectSize.X, objectSize.Y), objectSize.Z);
            float cameraView = 2.0f * Mathf.Tan(0.5f * ObjectViewerCam.GetFieldOfView() * MathF.PI / 180.0f);
            float distance = 4.0f * biggestAxis / cameraView;
            distance += 0.5f * biggestAxis;
            ObjectViewerCam.transform.SetPosition(ViewingModel.transform.GetPosition() - distance * ObjectViewerCam.transform.getForwardVector());
            
            //ObjectViewerCam.setForwardVector();
            //ObjectViewerCam.setForwardVector();
            rotateValueX = 0.0f;
            rotateValueY = 0.0f;

            root_y = 0.0f;

            originalPos = ViewingModel.transform.GetPosition();
            originalRot = ViewingModel.transform.GetRotation();

            isExamining = true;
            isZoomed = false;
            OnEnter = false;
        }
        
        if(isExamining)
        {
            Console.WriteLine("Examining");
            //GraphicsManagerWrapper.ToggleViewFrom2D(true);
            CheckMouseInput();
            CheckKeyboardInput();
            UpdateCamera();
        }
    }

    public void CheckKeyboardInput() // Keyboard Controls

    {
        //Console.WriteLine("Check Keyboard Input ViewObject");
        if (Input.GetKey(Keycode.LEFT))
        {
            Console.WriteLine("LEFT pressed");

            rotateValueY += 0.01f;
            //ObjectViewerModel.transform.SetRotation(new Vector3(0, rotateValue, 0));
            //ViewingModel.transform.SetRotation(new Vector3(0, rotateValue, 0));
        }
        else if (Input.GetKey(Keycode.RIGHT))
        {
            Console.WriteLine("RIGHT pressed");

            rotateValueY -= 0.01f;
            //ObjectViewerModel.transform.SetRotation(new Vector3(0, rotateValue, 0));
            //ViewingModel.transform.SetRotation(new Vector3(0, rotateValue, 0));
        }
        else if (Input.GetKey(Keycode.UP))
        {
            Console.WriteLine("UP pressed");

            rotateValueX += 0.01f;
            //ObjectViewerModel.transform.SetRotation(new Vector3(0, rotateValue, 0));
            //ViewingModel.transform.SetRotation(new Vector3(0, 0 , rotateValue));
        }
        else if (Input.GetKey(Keycode.DOWN))
        {
            Console.WriteLine("DOWN pressed");

            rotateValueX -= 0.01f;
            //ObjectViewerModel.transform.SetRotation(new Vector3(0, rotateValue, 0));
            //ViewingModel.transform.SetRotation(new Vector3(0, rotateValueY, 0));
        }
        ViewingModel.transform.SetRotation(new Vector3(rotateValueX, rotateValueY, 0));

        if (Input.GetKey(Keycode.S))
        {
            Console.WriteLine("S pressed");

            if (root_y > -20.0f)
            {
                root_y -= 1.0f;
                //ObjectViewerModel.transform.SetPositionY(root_y);
            }
        }
        else if (Input.GetKey(Keycode.W))
        {
            Console.WriteLine("W pressed");

            if (root_y < 20.0f)
            {
                root_y += 1.0f;
                //ObjectViewerModel.transform.SetPositionY(root_y);
            }
        }
        ViewingModel.transform.SetPositionY(originalPos.Y + root_y);

    }

    public void CheckMouseInput()  // Mouse Controls
    {
        //Console.WriteLine("Check Mouse Input ViewObject");
        if (Input.GetMouseButtonDown(Keycode.M1)) //left click to zoom
        {
            Console.WriteLine("Left click pressed");

            isZoomed = !isZoomed;
        }
        if (Input.GetMouseButtonDown(Keycode.M2)) //right click to exit
        {
            Console.WriteLine("Right click pressed");

            isExamining = false;
            isZoomed = false;
            GraphicsManagerWrapper.ToggleViewFrom2D(false);
            InventoryScript.InventoryIsOpen = true;
            ViewingModel.transform.SetPosition(originalPos);
            ViewingModel.transform.SetRotation(originalRot);
            GameObjectScriptFind("InventoryObject").SetActive(true);
            ViewingModel.SetActive(false);
            gameObject.SetActive(false);
        }
    }

    public void UpdateCamera()
    {
        Console.WriteLine("UpdateCam");

        if (isZoomed)
        {
            ObjectViewerCam.SetFieldOfView(30f);
        }
        else
        {
            ObjectViewerCam.SetFieldOfView(120f);
        }
    }
}