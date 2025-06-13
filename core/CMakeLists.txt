add_library(core STATIC
    audio_system.cpp
    config.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core PUBLIC
    nlohmann_json::nlohmann_json
) 