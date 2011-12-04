// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "LogManager.h"
#include "PathUtil.h"
#include "StringUtil.h"
#include <sstream>

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
LogManager::LogManager(const WideString& baseFilename)
{
	WideString logFilename = baseFilename;
	if (logFilename == L"") logFilename = L".\\wme.log";
#ifdef WIN32
	m_Stream.open(logFilename.c_str(), std::ios::app);
#else
	m_Stream.open(StringUtil::WideToAnsi(logFilename).c_str(), std::ios::app);
#endif
	if (!m_Stream.is_open())
	{
		m_Stream.clear();
		logFilename = PathUtil::GetSafeLogFileName();
#ifdef WIN32
		m_Stream.open(logFilename.c_str(), std::ios::app);
#else
		m_Stream.open(StringUtil::WideToAnsi(logFilename).c_str(), std::ios::app);
#endif
	}
	WriteHeader();
}

//////////////////////////////////////////////////////////////////////////
LogManager::~LogManager()
{
	WriteFooter();
	if (m_Stream.is_open()) m_Stream.close();
}

//////////////////////////////////////////////////////////////////////////
void LogManager::AddLine(const WideString& logString, const WideString& file, const int line)
{
	// handle listeners
	LogListeners::iterator it;
	for (it = m_Listeners.begin(); it != m_Listeners.end(); it++)
		(*it)->AddLine(logString, file, line);


	if (m_Stream.is_open())
	{		
		time_t ctTime;
		time(&ctTime);		
		struct tm* currentTime = localtime(&ctTime);

		// current time
		m_Stream
			<< std::setw(2) << std::setfill('0') << currentTime->tm_hour << ':'
			<< std::setw(2) << std::setfill('0') << currentTime->tm_min << ':'
			<< std::setw(2) << std::setfill('0') << currentTime->tm_sec << ": ";
		
		// message
		m_Stream << StringUtil::WideToUtf8(logString);

		if (!file.empty())
		{
			m_Stream << " (" << StringUtil::WideToUtf8(file);
			if (line >= 0) m_Stream << ", line " << line;
			m_Stream << ')';
		}
		
		// end and flush
		m_Stream << std::endl;		
		m_Stream.flush();
	}
}

//////////////////////////////////////////////////////////////////////////
void LogManager::AddListener(LogListener* listener)
{
	m_Listeners.push_back(listener);
}

//////////////////////////////////////////////////////////////////////////
void LogManager::RemoveListener(LogListener* listener)
{
	m_Listeners.erase(std::find(m_Listeners.begin(), m_Listeners.end(), listener));
}

//////////////////////////////////////////////////////////////////////////
void LogManager::WriteHeader()
{
	if (!m_Stream.is_open()) return;

	time_t ctTime;
	time(&ctTime);		
	struct tm* currentTime = localtime(&ctTime);

	std::wostringstream str;
	str << L"********** DEBUG LOG OPENED ";
	str
		<< std::setw(2) << std::setfill(L'0') << currentTime->tm_mday << '-'
		<< std::setw(2) << std::setfill(L'0') << (currentTime->tm_mon + 1) << '-'
		<< std::setw(4) << std::setfill(L'0') << (1900 + currentTime->tm_year) << ' ';
#ifdef _DEBUG
	str << L"(Debug build)  ";
#else
	str << L"(Release build)";
#endif
	str << L" **************" << std::ends;

	m_Stream << std::endl;
	AddLine(str.str());
}

//////////////////////////////////////////////////////////////////////////
void LogManager::WriteFooter()
{
	AddLine(L"********** DEBUG LOG CLOSED *****************************************");
}


} // namespace Wme
