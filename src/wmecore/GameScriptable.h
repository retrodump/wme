// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGameScriptable_H__
#define __WmeGameScriptable_H__


#include "ScriptableObject.h"
#include <boost/serialization/access.hpp>


namespace Wme
{
	class WmeDllExport GameScriptable :	public ScriptableObject
	{
	public:		
		GameScriptable();
		virtual ~GameScriptable();

        // ScriptableObject
		RTTI(GameScriptable);
		virtual WideString ConvertToString() { return L"Game object"; }
		virtual WideString GetTypeName() const { return L"Game"; }

		// script interface
		virtual void RegisterScriptInterface();
		
		bool ScLog(Script* script, const WideString& methodName, bool async);
		bool ScCollectGarbage(Script* script, const WideString& methodName, bool async);
		bool ScMsg(Script* script, const WideString& methodName, bool async);

	private:
		friend class boost::serialization::access; 
		template <class Archive>void serialize(Archive& ar, const unsigned int version);

	};
}

#endif // __WmeGameScriptable_H__