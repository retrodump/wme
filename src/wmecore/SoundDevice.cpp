// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SoundDevice.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SoundDevice::SoundDevice(const AnsiString& name)
{
	m_Name = name;
	m_IsDefault = false;
	m_VerMajor = m_VerMinor = 0;
	m_MaxSources = 0;
}

//////////////////////////////////////////////////////////////////////////
SoundDevice::~SoundDevice()
{
}

//////////////////////////////////////////////////////////////////////////
void SoundDevice::GetVersion(int& verMajor, int& verMinor) const
{
	verMajor = m_VerMajor;
	verMinor = m_VerMinor;
}

//////////////////////////////////////////////////////////////////////////
bool SoundDevice::SupportsExtension(const AnsiString& extension)
{
	return std::find(m_Extensions.begin(), m_Extensions.end(), extension) != m_Extensions.end();
}


} // namespace Wme
