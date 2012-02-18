project "wmecore"
	location "../build/"
	kind "SharedLib"
	language "C++"
	flags { "optimize" }
	objdir "../obj/wmecore"

	includedirs {
		"/usr/include/",
		"/usr/include/boost",
		"/usr/include/freetype2",
		"/usr/include/ogg",
		"/usr/include/theora",
		"/usr/include/vorbis",
		"/usr/include/OIS",
		"/usr/include/AL",
		"/usr/include/",
		"/usr/include/",
		"/usr/include/",
		"/usr/include/",
		"/usr/local/include/OGRE"
	}
	
	libdirs {
		"/usr/lib",
		"/usr/local/lib"
	}

	links {
		"boost_filesystem",
		"boost_system",
		"boost_thread",
		"freetype",
		"ogg",
		"theora",
		"vorbis",
		"OgreMain",
		"OIS",
		"openal",
		"tinyxml",
		"z",
	}

	files {
		"wmecore/*.h",
		"wmecore/*.cpp"
	}
	
	excludes {
		"wmecore/FileManager.h",
		"wmecore/FileManager.cpp",
		"wmecore/Game.h",
		"wmecore/Game.cpp",
		"wmecore/PackageBuilder.h",
		"wmecore/PackageBuilder.cpp",
		"wmecore/TheoraAudioBridge.h",
		"wmecore/TheoraAudioBridge.cpp",
		"wmecore/TheoraFileSystemBridge.h",
		"wmecore/TheoraFileSystemBridge.cpp",
		"wmecore/VideoClip.h",
		"wmecore/VideoClip.cpp",
		"wmecore/VideoManager.h",
		"wmecore/VideoManager.cpp"
	}
