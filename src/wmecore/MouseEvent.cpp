// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "MouseEvent.h"
#include "Viewport.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
MouseEvent::MouseEvent(EventType type, MouseButton button, int posX, int posY)
{
	m_Type = type;
	m_Button = button;
	m_PosX = posX;
	m_PosY = posY;
}

//////////////////////////////////////////////////////////////////////////
MouseEvent::~MouseEvent()
{
}

//////////////////////////////////////////////////////////////////////////
void MouseEvent::TranslateToViewport(Viewport* viewport)
{
	viewport->ScreenToViewport(m_PosX, m_PosY);

	m_PosX -= viewport->GetOgreViewport()->getActualLeft();
	m_PosY -= viewport->GetOgreViewport()->getActualTop();
}

//////////////////////////////////////////////////////////////////////////
void MouseEvent::SetPos(int x, int y)
{
	m_PosX = x;
	m_PosY = y;
}


} // namespace Wme
