// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeLogManager_H__
#define __WmeLogManager_H__

#include "WmeTypes.h"
#include <fstream>
#include <vector>

namespace Wme
{
	class WmeDllExport LogListener
	{
	public:
		virtual ~LogListener() {};
		virtual void AddLine(const WideString& logString, const WideString& file=L"", const int line=-1) = 0;
	};


	class WmeDllExport LogManager
	{
	public:
		LogManager(const WideString& baseFilename=L"");		
		virtual ~LogManager();

		void AddLine(const WideString& logString, const WideString& file=L"", const int line=-1);
		void AddListener(LogListener* listener);
		void RemoveListener(LogListener* listener);

	private:
		std::ofstream m_Stream;

		void WriteHeader();
		void WriteFooter();

		typedef std::vector<LogListener*> LogListeners;
		LogListeners m_Listeners;

	};
}

#endif // __WmeLogManager_H__