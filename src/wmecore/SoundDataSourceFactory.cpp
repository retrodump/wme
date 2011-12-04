// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SoundDataSourceFactory.h"
#include "VorbisDataSource.h"
#include "WaveDataSource.h"
#include "StringUtil.h"
#include "PathUtil.h"


namespace Wme
{


//////////////////////////////////////////////////////////////////////////
SoundDataSourceFactory::SoundDataSourceFactory()
{
}

//////////////////////////////////////////////////////////////////////////
SoundDataSourceFactory::~SoundDataSourceFactory()
{
}

//////////////////////////////////////////////////////////////////////////
SoundDataSource* SoundDataSourceFactory::GetInstance(const WideString& fileName)
{
	SoundDataSource* source = NULL;

	WideString ext = PathUtil::GetExtension(fileName);

	// create data source based on file extension
	if (StringUtil::CompareNoCase(ext, L".ogg"))
	{
		source = new VorbisDataSource;
	}
	else if (StringUtil::CompareNoCase(ext, L".wav"))
	{
		source = new WaveDataSource;
	}

	if (source)
	{
		if (!source->InitializeFromFile(fileName))
		{
			SAFE_DELETE(source);
		}
	}
	return source;
}


} // namespace Wme
