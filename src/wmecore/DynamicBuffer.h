// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeDynamicBuffer_H__
#define __WmeDynamicBuffer_H__

#include "WmeTypes.h"

namespace Wme
{
	class Uuid;
	class File;

	class WmeDllExport DynamicBuffer
	{
	public:
		DynamicBuffer();
		DynamicBuffer(const byte* buffer, dword size);
		DynamicBuffer(const DynamicBuffer& buffer);
		virtual ~DynamicBuffer();

		void Clear();
		
		size_t PutBytes(const byte* Buf, size_t Size);
		size_t Write(const AnsiString& Val);
		size_t Write(const WideString& Val);
		size_t Write(byte Val);
		size_t Write(int Val);
		size_t Write(double Val);
		size_t Write(dword Val);
		size_t Write(Uuid& Val);

		bool GetBytes(byte* buf, size_t size);
		WideString ReadWideString();
		AnsiString ReadAnsiString();
		WideString ReadUtf8String();
		byte ReadByte();
		int ReadInt();
		double ReadDouble();
		dword ReadDword();
		Uuid ReadUuid();

		int FillFromFile(File* file);

		size_t GetPosition() const;
		size_t SeekToPosition(size_t newPos);
		size_t SeekToStart();
		size_t SeekToEnd();
		bool IsAtEOF() const;

		size_t GetSize() const
		{
			return m_BufferSize;
		};

		byte* GetData() const
		{
			return m_Buffer;
		};

		void DumpToFile(const WideString& fileName) const;

	private:
		void SetDefaultValues();

		byte* m_Buffer;
		size_t m_BufferSize;
		size_t m_BufferCapacity;
		size_t m_BufferPos;
		size_t m_BlockSize;
	};

}

#endif // __WmeDynamicBuffer_H__