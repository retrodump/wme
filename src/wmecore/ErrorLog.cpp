// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "ErrorLog.h"

namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
ErrorLog::ErrorLog()
{
}


//////////////////////////////////////////////////////////////////////////
ErrorLog::~ErrorLog()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////////
bool ErrorLog::Cleanup()
{
	for (LogEntries::iterator it = m_Entries.begin(); it != m_Entries.end(); it++)
		delete (*it);
	m_Entries.clear();

	return true;
}


//////////////////////////////////////////////////////////////////////////
bool ErrorLog::AddEntry(const WideString& fileName, int line, const char* format, ...)
{
	va_list va;
	char buffer[32768];

	va_start(va, format);
	vsprintf(buffer, format, va);
	va_end(va);

	m_Entries.push_back(new ErrorLogEntry(fileName, line, buffer));
	return true;
}


} // namespace Wme
