project "armed"
	kind "ConsoleApp"
	language "C++"
	flags { "optimize" }
	objdir "../obj/armed"

	files {
		"armed/*.h",
		"armed/*.cpp"
	}
