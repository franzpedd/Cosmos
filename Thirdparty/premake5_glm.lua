project "GLM"
    location "glm"
    kind "None"
    language "C++"
    cppdialect "C++11"
    warnings "Off"
    
    targetdir(dirbin)
    objdir(dirobj)

    files
    {
        "glm/**.hpp",
        "glm/**.h"
    }

    includedirs
    {
        "glm"
    }

    filter "system:windows"
        disablewarnings
        {
            "26495",
            "26812",
            "4305"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    