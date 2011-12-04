// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#include "Wme.h"
#include "SystemInfo.h"
#include "StringUtil.h"


namespace Wme
{


WideStringSet SystemInfo::s_SupportedImageFormats;


//////////////////////////////////////////////////////////////////////////
const WideStringSet& SystemInfo::GetSupportedImageFormats()
{
	if (s_SupportedImageFormats.empty())
	{
		WideString extensions = L"bmp,ico,jpg,jif,jpeg,jpe,jng,koa,iff,lbm,mng,pbm,pbm,pcd,pcx,pgm,pgm,png,ppm,ppm,ras,tga,targa,tif,tiff,wap,wbmp,wbm,psd,cut,xbm,xpm,gif,hdr,g3,sgi,exr,j2k,j2c,jp2";
		WideStringListPtr extList = StringUtil::Split(extensions, L",");

		WideStringSet extSet;

		for (WideStringList::iterator it = extList->begin(); it != extList->end(); it++)
		{
			WideString lcExt = L"." + (*it);
			StringUtil::ToLowerCase(lcExt);

			s_SupportedImageFormats.insert(lcExt);
		}
	}

	return s_SupportedImageFormats;
}


} // namespace Wme
