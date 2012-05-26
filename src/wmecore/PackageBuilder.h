// This file is part of Wintermute Engine
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef __WmePackageBuilder_H__
#define __WmePackageBuilder_H__

#include "Object.h"
#include <boost/filesystem.hpp>

namespace Wme
{
	class PackageBuilderFilter;
	class PackageEntry;

	using namespace boost::filesystem;

	class WmeDllExport PackageBuilder : public Object
	{
	public:
		PackageBuilder();
		virtual ~PackageBuilder();

		bool ProcessDirectory(const WideString& packageDir, const WideString& outputPath, int priority, const WideString& description);


	private:
		void ProcessFiles(const path& currentPath, const WideString& basePath, std::ofstream& packageStream);
		void ProcessSingleFile(const path& currentPath, const WideString& basePath, std::ofstream& packageStream);

		void DeleteEntries();

		typedef std::list<PackageBuilderFilter*> FilterList;
		FilterList m_Filters;

		typedef std::list<PackageEntry*> PackageEntries;
		PackageEntries m_PackageEntries;
	};
}


#endif // __WmePackageBuilder_H__