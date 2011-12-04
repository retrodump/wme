// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "DynamicBuffer.h"
#include "Uuid.h"
#include "StringUtil.h"
#include "File.h"


namespace Wme
{

	
//////////////////////////////////////////////////////////////////////////
DynamicBuffer::DynamicBuffer()
{
	SetDefaultValues();
}

//////////////////////////////////////////////////////////////////////////
DynamicBuffer::DynamicBuffer(const byte* buffer, dword size)
{
	SetDefaultValues();
	PutBytes(buffer, size);
	SeekToStart();
}


//////////////////////////////////////////////////////////////////////////
DynamicBuffer::DynamicBuffer(const DynamicBuffer& buffer)
{
	SetDefaultValues();
	PutBytes(buffer.GetData(), buffer.GetSize());
	SeekToStart();
}

//////////////////////////////////////////////////////////////////////////
void DynamicBuffer::SetDefaultValues()
{
	m_Buffer = NULL;
	m_BufferPos = 0;
	m_BufferSize = 0;
	m_BufferCapacity = 0;
	m_BlockSize = 1024;
}

//////////////////////////////////////////////////////////////////////////
DynamicBuffer::~DynamicBuffer()
{
	SAFE_DELETE_ARRAY(m_Buffer);
}

//////////////////////////////////////////////////////////////////////////
void DynamicBuffer::Clear()
{
	SAFE_DELETE_ARRAY(m_Buffer);
	SetDefaultValues();
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::PutBytes(const byte* Buf, size_t Size)
{
	while (m_BufferPos + Size > m_BufferCapacity)
	{
		byte* NewBuf = new byte[m_BufferCapacity + m_BlockSize];
		if (m_Buffer)
		{
			memcpy(NewBuf, m_Buffer, m_BufferCapacity);
			delete [] m_Buffer;
		}
		m_Buffer = NewBuf;
		m_BufferCapacity += m_BlockSize;
	}
	memcpy(m_Buffer + m_BufferPos, Buf, Size);

	m_BufferPos += Size;

	// expand size if we are writing to the end
	if (m_BufferPos > m_BufferSize) m_BufferSize += Size;


	return m_BufferPos;
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::GetPosition() const
{
	return m_BufferPos;
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::SeekToPosition(size_t newPos)
{
	if (newPos >=0 && newPos <= m_BufferSize)
	{
		m_BufferPos = newPos;
		return m_BufferPos;
	}
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::SeekToStart()
{
	m_BufferPos = 0;
	return m_BufferPos;
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::SeekToEnd()
{
	m_BufferPos = m_BufferSize;
	return m_BufferPos;
}

//////////////////////////////////////////////////////////////////////////
bool DynamicBuffer::IsAtEOF() const
{
	return m_BufferPos >= m_BufferSize;
}


//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::Write(const AnsiString& Val)
{
	return PutBytes((byte*)Val.c_str(), (Val.length() + 1) * sizeof(AnsiString::value_type));
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::Write(const WideString& Val)
{
	return PutBytes((byte*)Val.c_str(), (Val.length() + 1) * sizeof(WideString::value_type));
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::Write(byte Val)
{
	return PutBytes(&Val, sizeof(byte));
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::Write(int Val)
{
	return PutBytes((byte*)&Val, sizeof(int));
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::Write(dword Val)
{
	return PutBytes((byte*)&Val, sizeof(dword));
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::Write(double Val)
{
	return PutBytes((byte*)&Val, sizeof(double));
}

//////////////////////////////////////////////////////////////////////////
size_t DynamicBuffer::Write(Uuid& Val)
{
	byte buf[16];
	Val.GetBinary(buf);
	return PutBytes(buf, 16);
}

//////////////////////////////////////////////////////////////////////////
bool DynamicBuffer::GetBytes(byte* buf, size_t size)
{
	if (m_BufferPos + size > m_BufferSize) return false;
	
	memcpy(buf, m_Buffer + m_BufferPos, size);
	m_BufferPos += size;
	return true;
}

//////////////////////////////////////////////////////////////////////////
WideString DynamicBuffer::ReadWideString()
{
	WideString ret = L"";

	wchar_t ch;
	do
	{
		GetBytes((byte*)&ch, sizeof(wchar_t));
		if (ch != L'\0') ret +=  ch;
	}
	while (ch != L'\0');

	return ret;
}

//////////////////////////////////////////////////////////////////////////
AnsiString DynamicBuffer::ReadAnsiString()
{
	AnsiString ret = "";

	char ch;
	do
	{
		GetBytes((byte*)&ch, sizeof(char));
		if (ch != '\0') ret +=  ch;
	}
	while (ch != '\0');

	return ret;
}

//////////////////////////////////////////////////////////////////////////
WideString DynamicBuffer::ReadUtf8String()
{
	Utf8String utfString = ReadAnsiString();
	return StringUtil::Utf8ToWide(utfString);
}

//////////////////////////////////////////////////////////////////////////
byte DynamicBuffer::ReadByte()
{
	byte ret = 0;
	GetBytes(&ret, sizeof(byte));
	return ret;
}

//////////////////////////////////////////////////////////////////////////
int DynamicBuffer::ReadInt()
{
	int ret = 0;
	GetBytes((byte*)&ret, sizeof(int));
	return ret;
}

//////////////////////////////////////////////////////////////////////////
double DynamicBuffer::ReadDouble()
{
	double ret = 0;
	GetBytes((byte*)&ret, sizeof(double));
	return ret;
}

//////////////////////////////////////////////////////////////////////////
dword DynamicBuffer::ReadDword()
{
	dword ret = 0;
	GetBytes((byte*)&ret, sizeof(dword));
	return ret;
}

//////////////////////////////////////////////////////////////////////////
Uuid DynamicBuffer::ReadUuid()
{
	byte buffer[16];
	GetBytes(buffer, 16);
	return Uuid(buffer);
}

//////////////////////////////////////////////////////////////////////////
void DynamicBuffer::DumpToFile(const WideString& fileName) const
{
	std::ofstream stream;
#ifdef WIN32
	stream.open(fileName.c_str(), std::ios::binary);
#else
	stream.open(StringUtil::WideToAnsi(fileName).c_str(), std::ios::binary);
#endif
	if (stream.is_open())
	{
		stream.write((char*)m_Buffer, m_BufferSize);
		stream.close();
	}
}

//////////////////////////////////////////////////////////////////////////
int DynamicBuffer::FillFromFile(File* file)
{
	m_BufferCapacity = file->GetSize();
	if (m_BufferCapacity > 0)
	{
		m_Buffer = new byte[m_BufferCapacity];

		file->Seek(0, File::SEEK_FROM_BEGIN);
		m_BufferSize = file->Read(m_Buffer, m_BufferCapacity);
	}
	return m_BufferSize;
}


} // namespace Wme
