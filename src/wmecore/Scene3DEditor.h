// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeScene3DEditor_H__
#define __WmeScene3DEditor_H__


#include "StageEditor.h"


namespace Wme
{
	class Scene3DBase;

	class Scene3DEditor : public StageEditor
	{
	public:
		Scene3DEditor(Scene3DBase* stage);
		virtual ~Scene3DEditor();

		virtual void OnSceneNodeCreated(Ogre::SceneNode* node) {};
		virtual void OnSceneNodeDestroyed(Ogre::SceneNode* node) {};
		virtual void OnSceneNodeChanged(Ogre::SceneNode* node) {};
		virtual void OnSceneGraphDirty() {};

		virtual bool Is3DEditor() { return true; }
	};
}

#endif // __WmeScene3DEditor_H__
