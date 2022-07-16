project "Engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir(Dir)
    objdir(Obj)

    pchheader "enpch.h"
    pchsource "Source/enpch.cpp"

    defines
    {
        
    }

    files
    {
        "Source/**.h",
        "Source/**.cpp"
    }

    includedirs
    {
        "Source/",
        "%{IncludeList.Vulkan}",
        "%{IncludeList.Volk}",
        "%{IncludeList.GLFW}",
        "%{IncludeList.GLM}"
    }

    links
    {
        "GLFW",
        "%{LibraryList.Shaderc}"
    }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        runtime "Release"
        optimize "On"

    filter "system:windows"
        disablewarnings
        {
            "26812"
        }