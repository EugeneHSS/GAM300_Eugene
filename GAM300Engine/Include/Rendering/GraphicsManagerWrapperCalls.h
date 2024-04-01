#pragma once
#include "dotnet/ImportExport.h"

namespace TDS
{

	class DLL_API GraphicsManagerWrapperCalls
	{
	public:
		static void				ToggleViewFrom2D(bool condition);

		static bool				IsViewingFrom2D();

		static void				SetLayerToRender(int ID);

		static int				CurrentRenderedLayer();
		static unsigned int		GetPickedObject();

		static void				SetClearColor(float x, float y, float z, float w);
		static void				SetFadeFactor(float value);
		static float			GetFadeFactor();
		static bool				IsNormalizedView();
		static void				SetNormalizedView(bool condition);
		static int				GetWinPosX();
		static int				GetWinPosY();
	};
}