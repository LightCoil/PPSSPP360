# Xbox 360 configuration
set(XDK_ROOT "C:/Program Files (x86)/Microsoft Xbox 360 SDK" CACHE PATH "Xbox 360 SDK root directory")

# Set Xbox 360 specific compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /X360 /D_XBOX=1 /D_XBOX360=1 /D_WIN32_WINNT=0x0600")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /X360 /D_XBOX=1 /D_XBOX360=1 /D_WIN32_WINNT=0x0600")

# Set Xbox 360 specific linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MACHINE:X360 /SUBSYSTEM:XBOX360")

# Add XDK include directories
include_directories(
    "${XDK_ROOT}/include"
    "${XDK_ROOT}/include/xbox"
)

# Add XDK library directories
link_directories(
    "${XDK_ROOT}/lib/xbox"
)

# Set output directory for XEX files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Add XEX writer
set(XEXWRITER_LIB "${XDK_ROOT}/lib/xbox/xexwriter.lib")

# Set compiler to use Xbox 360 toolchain
set(CMAKE_C_COMPILER "cl.exe")
set(CMAKE_CXX_COMPILER "cl.exe")

# Set platform to Xbox 360
set(CMAKE_SYSTEM_NAME "Xbox360")
set(CMAKE_SYSTEM_PROCESSOR "xbox360")

# Set Windows SDK version
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.19041.0") 