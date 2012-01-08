// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeLineElement2D_H__
#define __WmeLineElement2D_H__


#include "Element2D.h"


namespace Wme
{
	class WmeDllExport LineElement2D : public Element2D
	{
	public:
		LineElement2D();
		virtual ~LineElement2D();

		void AddGeometry();

	private:
		Ogre::MaterialPtr m_Material;
	};
}

#endif // __WmeLineElement2D_H__