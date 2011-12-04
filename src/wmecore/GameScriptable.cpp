// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "GameScriptable.h"
#include "Game.h"
#include "ValueManager.h"
#include "serialize.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
GameScriptable::GameScriptable()
{
}

//////////////////////////////////////////////////////////////////////////
GameScriptable::~GameScriptable()
{
}

//////////////////////////////////////////////////////////////////////////
// script interface
//////////////////////////////////////////////////////////////////////////
void GameScriptable::RegisterScriptInterface()
{
	ScriptableObject::RegisterScriptInterface();
	
	RegisterScriptMethod(L"Log",            (MethodHandler)&GameScriptable::ScLog);
	RegisterScriptMethod(L"CollectGarbage", (MethodHandler)&GameScriptable::ScCollectGarbage);
	RegisterScriptMethod(L"Msg",            (MethodHandler)&GameScriptable::ScMsg);
}

//////////////////////////////////////////////////////////////////////////
bool GameScriptable::ScLog(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	WideString logString = script->GetStack()->Pop()->GetString();

	Game::GetInstance()->Log(logString);

	script->GetStack()->PushNull();
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool GameScriptable::ScCollectGarbage(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(0);
	Game::GetInstance()->GetValueMgr()->CollectGarbage();
	script->GetStack()->PushNull();
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool GameScriptable::ScMsg(Script* script, const WideString& methodName, bool async)
{
	script->GetStack()->CheckParams(1);
	WideString msgString = script->GetStack()->Pop()->GetString();

	Game::GetInstance()->QuickMessage(msgString);

	script->GetStack()->PushNull();
	return true;
}

SERIALIZE(GameScriptable);
//////////////////////////////////////////////////////////////////////////
template <class Archive>void GameScriptable::serialize(Archive& ar, const unsigned int version)
{
	WideString test = L"game scriptable";
	ar & test;
}


} // namespace Wme
