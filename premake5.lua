workspace "Cosmos"
    architecture "x86_64"
    configurations { "Debug", "Release" }
    startproject "Game"

    Dir = "%{wks.location}/Build/Bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    Obj = "%{wks.location}/Build/Obj/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    VulkanSDK = os.getenv("VULKAN_SDK")

    IncludeList = {}

    IncludeList["Editor"] = "%{wks.location}/Editor/Source"
    IncludeList["Engine"] = "%{wks.location}/Engine/Source"
    IncludeList["Vulkan"] = "%{VulkanSDK}/Include"
    IncludeList["Volk"] = "%{wks.location}/Thirdparty/Volk"
    IncludeList["GLFW"] = "%{wks.location}/Thirdparty/glfw/include"
    IncludeList["GLM"] = "%{wks.location}/Thirdparty/glm"
    
group "Projects"
    include "Editor"
    include "Engine"
group ""

group "Dependencies"
    include "Thirdparty/premake5_glfw.lua"
    include "Thirdparty/premake5_glm.lua"
    include "Thirdparty/premake5_volk.lua"
group ""