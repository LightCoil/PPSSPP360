cmake_minimum_required(VERSION 3.10)
project(PSP360)

# Устанавливаем политику CMake
cmake_policy(SET CMP0077 NEW)

# Включаем конфигурацию Xbox 360
include(cmake/xbox360.cmake)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к локальным библиотекам
set(JSON_DIR "I:/tools/json")
set(SPDLOG_DIR "I:/tools/spdlog")
set(FMT_DIR "I:/tools/fmt")

# Добавляем пути к заголовочным файлам
include_directories(
    ${JSON_DIR}/include
    ${SPDLOG_DIR}/include
    ${FMT_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/xbox360
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

# Добавляем подпроекты
add_subdirectory(xbox360)
add_subdirectory(src)

# Создаем исполняемый файл
add_executable(PSP360 main.cpp core/audio_system.cpp core/video.cpp)

# Линкуем библиотеки
target_link_libraries(PSP360 PRIVATE 
    src 
    xbox360 
    xexwriter
    xboxkrnl
    xapilib
    xgraphics
    xinput
    xaudio2
)

# Установка выходной директории
set_target_properties(PSP360 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out"
    SUFFIX ".xex"
)

# Копирование необходимых файлов в выходную директорию
add_custom_command(TARGET PSP360 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/config"
        "$<TARGET_FILE_DIR:PSP360>/config"
)

# Настройки компиляции
if(MSVC)
    add_compile_options(/W4 /WX- /EHsc)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_XBOX=1)
    add_definitions(-D_XBOX360=1)
endif()
