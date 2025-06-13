cmake_minimum_required(VERSION 3.10)

# Определяем платформу
set(PLATFORM "Xbox360")

# Устанавливаем компилятор и флаги
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2 /W3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /W3")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")
endif()

# Создаем статическую библиотеку
add_library(xbox360 STATIC
    directx.cpp
    directx.hpp
    directx_defs.hpp
    xinput_defs.hpp
    psp_defs.hpp
)

# Добавляем директории для включения
target_include_directories(xbox360 PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/xbox360
    "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/ucrt"
    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/shared"
)

# Устанавливаем стандарт C++
set_target_properties(xbox360 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Оптимизация линковки для MSVC
if(MSVC)
    set_target_properties(xbox360 PROPERTIES
        STATIC_LIBRARY_FLAGS "/LTCG"
    )
endif()

# Линковка библиотек
target_link_libraries(xbox360
    d3d9
    d3dx9
    xaudio2
    xinput
    dxguid
    dinput8
    dsound
    winmm
)

target_link_directories(xbox360 PRIVATE
    "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64"
    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64"
    "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/ucrt/x64"
)

# Настройки компиляции
if(MSVC)
    target_compile_options(xbox360 PRIVATE
        /W4
        /WX
        /MP
        /O2
        /GL
        /Gy
        /Gw
        /GS-
        /guard:cf-
        /EHsc
    )
    
    target_compile_definitions(xbox360 PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _WIN32_WINNT=0x0601
        UNICODE
        _UNICODE
        DIRECTX_SDK
    )
endif()
