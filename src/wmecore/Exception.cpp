// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "Exception.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
Exception::Exception()
{
}

//////////////////////////////////////////////////////////////////////////
Exception::Exception(const WideString& description)
{
	m_Description = description;
}

Exception::~Exception(void) throw()
{
}


} // namespace Wme
