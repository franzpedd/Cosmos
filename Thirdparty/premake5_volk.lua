project "VOLK"
    location "volk"
    kind "None"
    language "C"
    cdialect "C89"
    
    targetdir(dirbin)
    objdir(dirobj)

    files
    {
        "volk/volk.h"
    }

    includedirs
    {
        "volk"
    }

    filter "system:windows"
        disablewarnings
        {

        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"