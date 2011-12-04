// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeErrorLog_H__
#define __WmeErrorLog_H__


#include <vector>
#include "WmeTypes.h"


namespace Wme
{
	class ErrorLog
	{
	public:
		class WmeDllExport ErrorLogEntry
		{
		public:
			ErrorLogEntry(const WideString& fileName, int line, const AnsiString& text)
			{
				m_FileName = fileName;
				m_Line = line;
				m_Text = text;
			}

			~ErrorLogEntry()
			{
			}

			const WideString& GetFileName()
			{
				return m_FileName;
			};

			int GetLine()
			{
				return m_Line;
			};

			const AnsiString& GetText()
			{
				return m_Text;
			};

		private:
			WideString m_FileName;
			AnsiString m_Text;
			int m_Line;
		};

		ErrorLog();
		virtual ~ErrorLog();

		bool Cleanup();
		bool AddEntry(const WideString& fileName, int line, const char* format, ...);

		typedef std::vector<ErrorLogEntry*> LogEntries;

		const LogEntries& GetEntries()
		{
			return m_Entries;
		};

	private:
		LogEntries m_Entries;
	};
}
#endif // __WmeErrorLog_H_
