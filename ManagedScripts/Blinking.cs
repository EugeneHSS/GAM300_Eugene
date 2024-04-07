using ScriptAPI;
using System;

public class Blinking : Script 
{ 
	private float timer = 2.0f;
	public override void Awake()
	{

	}
	public override void Update()
	{
		Vector4 color = gameObject.GetComponent<PointlightComponent>().GetColor();
		float alpha = gameObject.GetComponent<PointlightComponent>().GetColor().W;
		timer -= Time.deltaTime;
		// create a blinking effect
		if (timer < 0)
		{
			if (alpha == 0.0f)
			{
				gameObject.GetComponent<PointlightComponent>().SetColor(new Vector4(color.X, color.Y, color.Z, 1.0f));
			}
			else
			{
				gameObject.GetComponent<PointlightComponent>().SetColor(new Vector4(color.X, color.Y, color.Z, 0.0f));
			}
			timer = 2.0f;
		}

	}
 }