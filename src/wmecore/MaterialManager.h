// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeMaterialManager_H__
#define __WmeMaterialManager_H__


#include "Object.h"


namespace Wme
{
	class Material;
	class MaterialInstance;

	class WmeDllExport MaterialManager : public Object
	{
	public:
		MaterialManager();
		~MaterialManager();

		bool Initialize();

		MaterialInstance* GetMaterial(const WideString& fileName);
		void ReleaseMaterial(MaterialInstance*& material);

		Material* GetMaterialDef(const WideString& fileName);
		void ReleaseMaterialDef(Material*& material);


	private:
		typedef std::map<WideString, MaterialInstance*> MaterialMap;
		MaterialMap m_Materials;

		typedef std::map<WideString, Material*> MaterialDefMap;
		MaterialDefMap m_MaterialDefs;

	};
}

#endif // __WmeMaterialManager_H__