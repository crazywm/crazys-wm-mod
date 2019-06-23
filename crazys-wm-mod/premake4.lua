function main()
	local _solution_name = 'WhoreMaster'
	solution(_solution_name)

	location(".")

	base_src_dir = "."
	base_deps_dir = "_lib"
	base_bin_dir = "_bin"
	base_prj_dir = ".";

	project("WM")

	configurations({"Debug", "Release"})

	language("c++")

	files({
		"*.cpp",
		"*.h",
		"SDL_anigif.c"
	})
	
	excludes({"cMovieScreen.*"})
	
	includedirs({
		"/usr/include/SDL",
		"/usr/include/lua5.1"
	})

	libdirs({})

	links({
		"lua5.1",
		"SDL",
		"SDL_ttf",
		"SDL_gfx",
		"SDL_image",
	})

	buildoptions({
		"-std=c++11",
		"-fpermissive",
		"-Wno-enum-compare",
		"-Wno-unused-result",
		"-x c++"
	})

	linkoptions({})

	configuration({"Debug"})
	defines({"DEBUG"})
	flags({"Symbols"})

	configuration({"Release"})
  flags({"Optimize"})

	configuration({})

	kind("ConsoleApp")

	targetname("WhoreMaster")
	targetdir(".")
	objdir(base_bin_dir)

	if os.is("linux") then
		defines({"LINUX"})
	else
		files("SDL_anigif.c")
	end
end

main()
