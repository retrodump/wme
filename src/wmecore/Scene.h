// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScene_H__
#define __WmeScene_H__


#include "Stage.h"
#include "DocumentAwareObject.h"


namespace Wme
{
	class Scene;
	class ElementCollection;
	class SpriteSubFrame;
	class TextElement;
	class Viewport;
	class Actor;


	class WmeDllExport Scene : public Stage, public DocumentAwareObject
	{
	public:
		Scene();
		virtual ~Scene();

		// DocumentAwareObject
		virtual bool LoadFromXml(TiXmlElement* rootNode);
		virtual bool SaveToXml(TiXmlElement* rootNode);

	private:

	};
}

#endif // __WmeScene_H__
