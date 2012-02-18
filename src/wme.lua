project "wme"
	kind "ConsoleApp"
	language "C++"
	flags { "optimize" }
	objdir "../obj/wme"

	files {
		"wme/main.cpp",
		"wme/pch.cpp",
		"wme/resource.h"	
	}
