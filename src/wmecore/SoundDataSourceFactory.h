// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmeSoundDataSourceFactory_H__
#define __WmeSoundDataSourceFactory_H__


#include "Object.h"


namespace Wme
{
	class SoundDataSource;

	class WmeDllExport SoundDataSourceFactory : public Object
	{
	public:
		SoundDataSourceFactory();
		~SoundDataSourceFactory();

		static SoundDataSource* GetInstance(const WideString& fileName);
	};
}

#endif // __WmeSoundDataSourceFactory_H__