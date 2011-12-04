// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "InteractiveObject.h"
#include "ContentManager.h"
#include "Viewport.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
InteractiveObject::InteractiveObject()
{
}

//////////////////////////////////////////////////////////////////////////
InteractiveObject::~InteractiveObject()
{
	/* TODO uncomment once dummy is gone in ActiveSpot
	if (HasMouseCapture())
	{
		Game::GetInstance()->GetContentMgr()->ReleaseMouse(this);
	}
	*/
}

//////////////////////////////////////////////////////////////////////////
bool InteractiveObject::HandleMouseEvent(Viewport* viewport, MouseEvent& event)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool InteractiveObject::HasMouseCapture() const
{
	return (this == Game::GetInstance()->GetContentMgr()->GetMouseCapture());
}


//////////////////////////////////////////////////////////////////////////
bool InteractiveObject::InvokeScriptMouseEvent(Viewport* viewport, MouseEvent& event)
{	
	WideString methodName;

	switch (event.GetType())
	{
	
	case MouseEvent::EVENT_PRESS:
		switch (event.GetButton())
		{
		case MouseEvent::BUTTON_LEFT:
			methodName = L"OnLeftClick";
			break;
		case MouseEvent::BUTTON_RIGHT:
			methodName = L"OnRightClick";
			break;
		case MouseEvent::BUTTON_MIDDLE:
			methodName = L"OnMiddleClick";
			break;
		}
		break;
	
	case MouseEvent::EVENT_RELEASE:
		switch (event.GetButton())
		{
		case MouseEvent::BUTTON_LEFT:
			methodName = L"OnLeftRelease";
			break;
		case MouseEvent::BUTTON_RIGHT:
			methodName = L"OnRightRelease";
			break;
		case MouseEvent::BUTTON_MIDDLE:
			methodName = L"OnMiddleRelease";
			break;
		}
		break;
	
	case MouseEvent::EVENT_DOUBLE_CLICK:
		switch (event.GetButton())
		{
		case MouseEvent::BUTTON_LEFT:
			methodName = L"OnLeftDoubleClick";
			break;
		case MouseEvent::BUTTON_RIGHT:
			methodName = L"OnRightDoubleClick";
			break;
		case MouseEvent::BUTTON_MIDDLE:
			methodName = L"OnMiddleDoubleClick";
			break;
		}
		break;
	
	case MouseEvent::EVENT_WHEEL_UP:
		methodName = L"OnMouseWheelUp";
		break;
	
	case MouseEvent::EVENT_WHEEL_DOWN:
		methodName = L"OnMouseWheelDown";
		break;

	}

	if (methodName.empty()) return false;

	Scripts validScripts;
	GetScriptsWithMethod(methodName, validScripts);
	if (validScripts.empty()) return false;

	MouseEvent translEvent = event;
	translEvent.TranslateToViewport(viewport);

	foreach (ScriptPtr script, validScripts)
	{
		Script* handlerThread = CreateEventHandlerThread(script.get(), methodName);
		if (handlerThread)
		{
			Stack* stack = handlerThread->GetStack();
			
			Value* val = Value::Create();
			val->SetProperty(L"X", Value::Create(translEvent.GetPosX()));
			val->SetProperty(L"Y", Value::Create(translEvent.GetPosY()));
			
			stack->Push(val);
			stack->Push(viewport);
			stack->Push(2);
		}
	}
	return true;
}


} // namespace Wme