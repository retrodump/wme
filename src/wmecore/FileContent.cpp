// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "FileContent.h"
#include "StringUtil.h"
#include "Game.h"
#include "FileManager.h"

namespace Wme
{


//////////////////////////////////////////////////////////////////////////
FileContent::FileContent()
{
}

//////////////////////////////////////////////////////////////////////////
FileContent::FileContent(const WideString& fileName)
{
	LoadFile(fileName);
}

//////////////////////////////////////////////////////////////////////////
FileContent::~FileContent()
{
}

//////////////////////////////////////////////////////////////////////////
WideString FileContent::GetLine(int lineNum) const
{
	if (lineNum < (int)m_Lines.size()) return m_Lines[lineNum];
	else return L"";
}

//////////////////////////////////////////////////////////////////////////
bool FileContent::LoadFile(const WideString& fileName)
{
	m_Lines.clear();

	WideString fullPath = Game::GetInstance()->GetFileMgr()->FindDiskFile(fileName);
	if (fullPath.empty()) return false;


	std::ifstream stream;
	
#ifdef WIN32
	stream.open(fullPath.c_str());
#else
	stream.open(StringUtil::WideToAnsi(fullPath).c_str());
#endif
	if (!stream.is_open()) return false;

	// TODO: get rid of the 1024 limit
	char buf[1024];
	bool firstLine = true;
	bool utf8 = false;

	while (!stream.eof())
	{
		AnsiString line;

		stream.getline(buf, 1024);
		if (firstLine)
		{
			utf8 = StringUtil::IsUtf8BOM(reinterpret_cast<byte*>(buf), 1024);
			firstLine = false;
			if (utf8) line = AnsiString(buf + 3);
			else line = AnsiString(buf);
		}
		else line = AnsiString(buf);

		if (utf8) m_Lines.push_back(StringUtil::Utf8ToWide(line));
		else m_Lines.push_back(StringUtil::AnsiToWide(line));
	}
	stream.close();

	return true;
}


} // namespace Wme
