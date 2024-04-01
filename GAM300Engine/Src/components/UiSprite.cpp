#include "components/UiSprite.h"
#include "Input/InputSystem.h"
#include "Rendering/UiSystem.h"
namespace TDS
{

	RTTR_REGISTRATION
	{
		using namespace TDS;

	rttr::registration::class_<UISprite>("UI Sprite")
		.property("Texture", &UISprite::m_TextureName)
		.property("EnableSprite", &UISprite::m_EnableSprite)
		.property("LayerID", &UISprite::m_LayerID)
		.property("is Font", &UISprite::m_IsFont)
		.property("message", &UISprite::m_Message)
		.property("Font Texture", &UISprite::m_FontName)
		.property("ForeGroundColour", &UISprite::m_ForeGroundColour)
		.property("Sprite Color", &UISprite::m_Color)
		.property("BackGroundColour", &UISprite::m_BackGroundColour)
		.property("BoundingBoxMin", &UISprite::m_BoundingBoxMin)
		.property("BoundingBoxMax", &UISprite::m_BoundingBoxMax)
		.method("SetLayerID", &UISprite::SetLayerID)
		.method("SetColour", &UISprite::SetColour)
		.method("IsMouseCollided", &UISprite::IsMouseCollided)
		.method("SetTextureName", &UISprite::SetTextureName)
		.method("IsFont", &UISprite::IsFont)
		.method("SetFontName", &UISprite::SetFontName)
		.method("SetFontMessage", &UISprite::SetFontMessage)
		.method("SetFontColour", &UISprite::SetFontColour)
		.method("SetFontBackgroundColour", &UISprite::SetFontBackgroundColour)
		.method("SetEnableSprite", &UISprite::SetEnableSprite);
	}
		UISprite::UISprite() :
		m_TextureReference(),
		m_LayerID(-1),
		m_Color(0.1f, 0.1f, 0.1f, 1.f),//color range from 0 to 1
		m_EnableSprite(true),
		m_TextureName(""),
		m_Message(""),
		m_BackGroundColour(0.f, 0.f, 0.f, 1.f),
		m_ForeGroundColour(0.f, 0.f, 0.f, 0.f),
		m_BoundingBoxMin(-0.5f, -0.5f),
		m_BoundingBoxMax(0.5f, 0.5f)

	{
	}
	UISprite::UISprite(UISprite&& toMove) noexcept :
		m_TextureReference(toMove.m_TextureReference),
		m_LayerID(toMove.m_LayerID),
		m_Color(toMove.m_Color),
		m_EnableSprite(toMove.m_EnableSprite),
		m_TextureName(toMove.m_TextureName),
		m_Message(toMove.m_Message),
		m_BackGroundColour(toMove.m_BackGroundColour),
		m_ForeGroundColour(toMove.m_ForeGroundColour),
		m_BoundingBoxMin(toMove.m_BoundingBoxMin),
		m_BoundingBoxMax(toMove.m_BoundingBoxMax),
		m_IsFont(toMove.m_IsFont),
		m_FontName(toMove.m_FontName)
	{
		
	}

	UISprite* GetUISprite(EntityID entityID)
	{
		return ecs.getComponent<UISprite>(entityID);
	}


	void UISprite::SetLayerID(int layerID)
	{
		m_LayerID = layerID;
	}

	void UISprite::SetColour(Vec4 color)
	{
		this->m_Color = color;
	}

	void TDS::UISprite::SetColourViaFloat(float x, float y, float z, float w)
	{
		m_Color = Vec4(x, y, z, w);
	}

	bool UISprite::IsMouseCollided()
	{
		Vec2 mousePos = UiSystem::getNDCMousePos();
		if (mousePos.x >= m_BoundingBoxMin.x && mousePos.x <= m_BoundingBoxMax.x &&
			mousePos.y >= m_BoundingBoxMin.y && mousePos.y <= m_BoundingBoxMax.y)
			return true; 
	
		return false;

	}

	void UISprite::SetTextureName(std::string textureName)
	{
		m_TextureName = textureName;
	}

	std::string DLL_API TDS::UISprite::GetTextureName()
	{
		return m_TextureName;
	}

	bool UISprite::IsFont()
	{
		return m_IsFont;
	}

	void UISprite::SetIsFont(bool condition)
	{
		m_IsFont = condition;
	}

	void UISprite::SetFontName(std::string fontName)
	{
		m_FontName = fontName;
	}

	void UISprite::SetFontMessage(std::string msg)
	{
		m_Message = msg;
	}

	void UISprite::SetFontColour(Vec4 Color)
	{
		m_ForeGroundColour = Color;
	}

	void TDS::UISprite::SetFontColourViaFloat(float x, float y, float z, float w)
	{
		m_ForeGroundColour = Vec4(x, y, z, w);
	}

	void UISprite::SetFontBackgroundColour(Vec4 Color)
	{
		m_BackGroundColour = Color;
	}

	void TDS::UISprite::SetFontBackgroundColourViaFloat(float x, float y, float z, float w)
	{
		m_BackGroundColour = Vec4(x, y, z, w);
	}

	void TDS::UISprite::SetEnableSprite(bool status)
	{
		m_EnableSprite = status;
	}

}