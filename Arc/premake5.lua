project "Arc"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "arcpch.h"
	pchsource "src/arcpch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",

		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",

		"%{IncludeDir.EASTL}/EASTL.natvis",
		"%{IncludeDir.glm}/util/glm.natvis",
		"%{IncludeDir.yaml_cpp}/../src/contrib/yaml-cpp.natvis",
		"%{IncludeDir.JoltPhysics}/Jolt/Jolt.natvis"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.assimp_config}",
		"%{IncludeDir.assimp_config_assimp}",
		"%{IncludeDir.optick}",
		"%{IncludeDir.box2d}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.EABase}",
		"%{IncludeDir.EASTL}",
		"%{IncludeDir.miniaudio}",
		"%{IncludeDir.icons}",
		"%{IncludeDir.JoltPhysics}",
		"%{IncludeDir.tinyobj}",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"assimp",
		"optick",
		"box2d",
		"EASTL",
		"JoltPhysics",
		"Arc-ScriptCore",

		"%{Lib.mono}"
	}

	filter "files:vendor/ImGuizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ARC_DEBUG"
		runtime "Debug"
		symbols "on"
		postbuildcommands
		{
			'{COPY} "../Arc/vendor/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		defines "ARC_RELEASE"
		runtime "Release"
		optimize "speed"
		postbuildcommands
		{
			'{COPY} "../Arc/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "ARC_DIST"
		runtime "Release"
		optimize "speed"
		postbuildcommands
		{
			'{COPY} "../Arc/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}
