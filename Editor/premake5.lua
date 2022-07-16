project "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir(Dir)
    objdir(Obj)

    defines
    {
        
    }

    files
    {
        "Source/**.h",
        "Source/**.cpp",
        "Shader/**.frag",
        "Shader/**.vert"
    }

    includedirs
    {
        "Source/",
        "%{IncludeList.Engine}",
        "%{IncludeList.Vulkan}",
        "%{IncludeList.Volk}",
        "%{IncludeList.GLFW}",
        "%{IncludeList.GLM}",
        "%{LibraryList.Shaderc}"
    }

    links
    {
        "Engine"
    }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        runtime "Release"
        optimize "On"