// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "KeyboardEvent.h"
#include "StringUtil.h"

#ifdef __GNUC__
#include <OIS.h>
#else
#include <ois.h>
#endif

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
KeyboardEvent::KeyboardEvent(EventType type, KeyCode keyCode, const WideString& text)
{
	m_Type = type;
	m_KeyCode = keyCode;
	m_Text = text;
	m_Modifiers = 0;
}

//////////////////////////////////////////////////////////////////////////
KeyboardEvent::KeyboardEvent(EventType type, OIS::Keyboard* oisKeyboard, const OIS::KeyEvent& oisEvent)
{
	m_Type = type;
	FromOis(oisKeyboard, oisEvent);
}

//////////////////////////////////////////////////////////////////////////
KeyboardEvent::~KeyboardEvent()
{
}

//////////////////////////////////////////////////////////////////////////
void KeyboardEvent::FromOis(OIS::Keyboard* oisKeyboard, const OIS::KeyEvent& oisEvent)
{
	m_KeyCode = static_cast<KeyCode>(oisEvent.key);

	m_Modifiers = 0;
	if (oisKeyboard->isModifierDown(OIS::Keyboard::Shift)) m_Modifiers |= Shift;
	if (oisKeyboard->isModifierDown(OIS::Keyboard::Alt)) m_Modifiers |= Alt;
	if (oisKeyboard->isModifierDown(OIS::Keyboard::Ctrl)) m_Modifiers |= Ctrl;
	
	// OIS doesn't handle dead-characters properly
	// use native APIs for text input
	AnsiString str;
	str += static_cast<char>(oisEvent.text);
	m_Text = StringUtil::AnsiToWide(str);	
}


} // namespace Wme
