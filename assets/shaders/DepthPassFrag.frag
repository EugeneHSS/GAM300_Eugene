#version 450

layout(location = 0) in flat uint isRenderable;

layout(location = 0) out vec4 color;

void main() 
{	
	if (isRenderable == 0)
	{
		discard;
	}

	color = vec4(1.0, 0.0, 0.0, 1.0);
}