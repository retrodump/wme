// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeHybridCamera_H__
#define __WmeHybridCamera_H__


#include "Camera.h"


namespace Wme
{
	class Viewport;

	class WmeDllExport HybridCamera : public Camera
	{
	public:
		HybridCamera();
		virtual ~HybridCamera();

		virtual void AdjustToViewport(Viewport* viewport);
	};
}

#endif // __WmeHybridCamera_H__
