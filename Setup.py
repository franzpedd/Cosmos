import os;
import subprocess;
import platform;
import shutil;
import requests;
from tqdm import tqdm;

class Util :

    @classmethod
    def Download(cls, url, path) :

        if(os.path.isfile(path) is True) : return;

        downloadrequest = requests.get(url, stream = True);
        downloadpercentage = int(downloadrequest.headers.get("content-lenght", 0));

        with open(path, "wb") as file, tqdm(
                desc = path, total = downloadpercentage, unit = "iB", unit_scale = True, unit_divisor = 1024,
            ) as bar :
                for data in downloadrequest.iter_content(chunk_size = 1024) :
                    size = file.write(data);
                    bar.update(size);
    
    @classmethod
    def Unzip(cls, file, path) :

        if(os.path.isfile(file) is None) : return;

        shutil.unpack_archive(file, path);

class Vulkan :

    version = "1.3.216.0";
    path = "Thirdparty/vulkan";

    @classmethod
    def Download(cls) :

        if(os.environ.get("VULKAN_SDK") is None) :

            if(os.path.isdir(f"{cls.path}") is False) : os.makedirs(f"{cls.path}");

            url = "";
            file = "";

            if(platform.system() == "Windows") :
                url = f"http://sdk.lunarg.com/sdk/download/{cls.version}/windows/VulkanSDK-{cls.version}-Installer.exe";
                file = f"{cls.path}/VulkanSDK-{cls.version}-Installer.exe";

            if(platform.system() == "Linux") :
                url = f"http://sdk.lunarg.com/sdk/download/{cls.version}/linux/vulkansdk-linux-x86_64-{cls.version}.tar.gz";
                file = f"{cls.path}/vulkansdk-linux-x86_64-{cls.version}.tar.gz";

            if(platform.system() == "Darwin") :
                url = f"http://sdk.lunarg.com/sdk/download/{cls.version}/mac/vulkansdk-macos-{cls.version}.dmg";
                file = f"{cls.path}/vulkansdk-macos-{cls.version}.dmg";

            Util.Download(url, file);
            os.startfile(os.path.abspath(file));

class Premake5 :

    version = "5.0.0-beta1";
    path = "Thirdparty/premake5";

    @classmethod
    def Download(cls) :

        if(os.path.isdir(f"{cls.path}") is False) : os.makedirs(f"{cls.path}");

        url = "";
        file = "";

        if(platform.system() == "Windows") :
            url = f"https://github.com/premake/premake-core/releases/download/v{cls.version}/premake-{cls.version}-windows.zip";
            file = f"{cls.path}/premake-{cls.version}-windows.zip";

        if(platform.system() == "Linux") :
            url = f"https://github.com/premake/premake-core/releases/download/v{cls.version}/premake-{cls.version}-linux.tar.gz";
            file = f"{cls.path}/premake-{cls.version}-linux.tar.gz";

        if(platform.system() == "Darwin") :
            url = f"https://github.com/premake/premake-core/releases/download/v{cls.version}/premake-{cls.version}-macosx.tar.gz";
            file = f"{cls.path}/premake-{cls.version}-macosx.tar.gz";

        Util.Download(url, file);
        Util.Unzip(file, cls.path);

    @classmethod
    def Generate(cls) :

        if(platform.system() == "Windows") : subprocess.call(f"{cls.path}/premake5 vs2022");
        if(platform.system() == "Linux") : subprocess.call(f"{cls.path}/premake5 gmake2");
        if(platform.system() == "Darwin") : subprocess.call(f"{cls.path}/premake5 xcode4");

class GLFW :

    @classmethod
    def Clone(cls) :

        if(os.path.isdir("Thirdparty/GLFW") is True) : return;

        dir = os.getcwd();
        os.chdir("Thirdparty");
        subprocess.call("git clone https://github.com/glfw/glfw glfw");
        os.chdir("glfw");
        subprocess.call("git checkout 3.3-stable");
        os.chdir(dir);

class Volk :

    @classmethod
    def Clone(cls) :

        if(os.path.isdir("Thirdparty/volk") is True) : return;

        dir = os.getcwd();
        os.chdir("Thirdparty");
        subprocess.call("git clone https://github.com/zeux/volk volk");
        os.chdir(dir);

# global logic
Vulkan.Download();
GLFW.Clone();
Volk.Clone();
Premake5.Download();
Premake5.Generate();