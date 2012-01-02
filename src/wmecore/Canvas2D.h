// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeCanvas2D_H__
#define __WmeCanvas2D_H__


#include "Transform2D.h"


namespace Wme
{
	class WmeDllExport Canvas2D : public Ogre::MovableObject
	{
	public:
		Canvas2D();
		virtual ~Canvas2D();

		void SetTransform(const Transform2D& transform);
		void ResetTransform();
		const Transform2D& GetTransform() const;

	private:
		Transform2D m_Transform;

	};
}

#endif // __WmeCanvas2D_H__
