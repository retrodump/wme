// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeFileContent_H__
#define __WmeFileContent_H__

#include "WmeTypes.h"

namespace Wme
{
	class WmeDllExport FileContent
	{
	public:
		FileContent();
		FileContent(const WideString& fileName);
		virtual ~FileContent();

		WideString GetLine(int lineNum) const;
		bool LoadFile(const WideString& fileName);

		int GetNumLines() const
		{
			return (int)m_Lines.size();
		};

	private:
		typedef std::vector<WideString> LineVector;
		LineVector m_Lines;
	};
}

#endif // _WmeFileContent_H__