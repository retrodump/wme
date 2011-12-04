// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeGizmoBase_H__
#define __WmeGizmoBase_H__


#include "Object.h"


namespace Wme
{
	class WmeDllExport GizmoBase : public Object, public Ogre::MeshSerializerListener
	{
	public:
		GizmoBase();
		virtual ~GizmoBase();

		// Ogre::MeshSerializerListener
		void processMaterialName(Ogre::Mesh* mesh, Ogre::String* name);
		void processSkeletonName(Ogre::Mesh* mesh, Ogre::String* name);

	protected:
		void LoadMesh(const WideString& fileName);
		void LoadMaterial();

		Ogre::MeshPtr m_Mesh;
	};
}

#endif // __WmeGizmoBase_H__
