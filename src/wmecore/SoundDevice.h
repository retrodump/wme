// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSoundDevice_H__
#define __WmeSoundDevice_H__


#include "Object.h"


namespace Wme
{
	class WmeDllExport SoundDevice : public Object
	{
	public:
		friend class SoundManager;

		SoundDevice(const AnsiString& name);
		~SoundDevice();

		const AnsiString& GetName() const { return m_Name; }
		bool IsDefault() const { return m_IsDefault; }
		size_t GetMaxSources() const { return m_MaxSources; }
		void GetVersion(int& verMajor, int& verMinor) const;
		bool SupportsExtension(const AnsiString& extension);

	private:
		AnsiString m_Name;
		AnsiStringList m_Extensions;
		bool m_IsDefault;
		size_t m_MaxSources;
		int m_VerMajor;
		int m_VerMinor;
	};
}

#endif // __WmeSoundDevice_H__
