-- Base solution
solution("ucoap")
   configurations({ "Debug", "Release" })
   includedirs({ ".." })

-- General debug and release configurations
configuration("Debug")
    defines({ "DEBUG" })
    flags({ "Symbols", "ExtraWarnings", "StaticRuntime" })
    targetdir("../debug")

configuration("Release")
    defines({ "NDEBUG" })
    flags({ "Optimize", "ExtraWarnings", "StaticRuntime" })
    targetdir("../release")

-- Visual studio general options for all projects.
configuration("vs*")
    buildoptions({ "/wd4127", "/wd4206", "/wd4996" })
    defines({ "WIN32", "_CRT_SECURE_NO_WARNINGS" })
   
-- Library project files

project("cutest")
    kind("StaticLib")
    language("C")
    files({ "../cutest/**.h", "../cutest/**.c" })

project("msys")
    kind("StaticLib")
    language("C")
    files({ "../msys/**.h", "../msys/**.c" })
	
project("mnet")
    kind("StaticLib")
    language("C")
    files({ "../mnet/**.h", "../mnet/**.c" })
	
project("mcoap")
    kind("StaticLib")
    language("C")
    files({ "../mcoap/**.h", "../mcoap/**.c" })
	
    
-- Application project files.  Includes application and platform specific options.

project("testus")
    kind("ConsoleApp")
    language("C")
    files({ "../testms/**.h", "../testms/**.c" })
    links({ "msys", "cutest"})
    configuration("windows")
        links("ws2_32")
    configuration("gmake")
        links("m")
        
project("testuc")
    kind("ConsoleApp")
    language("C")
    files({ "../testmc/**.h", "../testmc/**.c" })
    links({ "mcoap", "msys", "cutest"})
    configuration("windows")
        links("ws2_32")
    configuration("gmake")
        links("m")
        

        
