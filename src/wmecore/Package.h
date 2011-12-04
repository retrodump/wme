// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePackage_H__
#define __WmePackage_H__

#include "Object.h"
#include "WmePackage.h"


namespace Wme
{
	class Package;

	class WmeDllExport Package :	public Object
	{
	public:
		Package();
		virtual ~Package();
		
		bool Load(WideString fileName);

		int GetPriority() const { return m_Priority; }
		const WideString& GetFileName() const { return m_FileName; }
		const WideString& GetDescription() const { return m_Description; }
		time_t GetCreationTime() const { return m_CreationTime; }
		std::ifstream& GetStream() { return m_Stream; }
	
	private:
		void ReadDirectoryEntry();

		WideString m_FileName;
		WideString m_Description;
		int m_Priority;
		std::ifstream m_Stream;
		time_t m_CreationTime;
	};	
}

#endif // __WmePackage_H__