
cmake_minimum_required(VERSION 3.22.1)

include_directories(D:/Dev/Include/) 

project("zemu")

add_library(zemu SHARED
		android_native.cpp
		sshApp.cpp
		zSpeccyLayout.cpp
        zFormDebugger.cpp
		zSpeccyKeyboard.cpp
		zFormBrowser.cpp
		zFormSettings.cpp
		emu/zDevs.cpp
		emu/zSpeccy.cpp
		emu/zCpus.cpp
		emu/zCpuGS.cpp
		emu/zVG93.cpp
		emu/zTape.cpp
		emu/zDisAsm.cpp
		zRibbonDebugger.cpp
        zFormAssembler.cpp
		zFormBpList.cpp
		zFormTZX.cpp
		zFormPokes.cpp
		zFormSelFile.cpp
		sshCommon.cpp
)

find_package(zstandard REQUIRED CONFIG)
target_include_directories(zemu PUBLIC include)
find_package(zostrov REQUIRED CONFIG)
add_library(curl STATIC IMPORTED)
set_target_properties(curl PROPERTIES IMPORTED_LOCATION d:/Dev/Lib/${ANDROID_ABI}/libcurl.a)
target_link_libraries(zemu android EGL GLESv2 OpenSLES z log curl zstandard::zstandard zostrov::zostrov)
