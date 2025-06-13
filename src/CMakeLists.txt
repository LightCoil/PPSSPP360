add_library(src STATIC
    memory.cpp
    xex.cpp
    audio.cpp
    video.cpp
)

target_include_directories(src PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/xbox360
) 