// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMeshManager_H__
#define __WmeMeshManager_H__


#include "Object.h"


namespace Wme
{
	class Mesh;
	class MeshEntity;

	class WmeDllExport MeshManager : public Object
	{
	public:
		MeshManager();
		~MeshManager();

		bool Initialize();

		Mesh* GetMesh(const WideString& fileName, MeshEntity* owner);
		void ReleaseMesh(Mesh*& mesh, MeshEntity* owner);

	private:
		typedef std::map<WideString, Mesh*> MeshMap;
		MeshMap m_Meshes;
	};
}

#endif // __WmeMeshManager_H__
