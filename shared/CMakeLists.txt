find_package(nlohmann_json REQUIRED)

add_library(gear_params
    gear_params.h
    gear_params.cpp
    gear_geometry.h
    gear_geometry.cpp
)

target_link_libraries(gear_params PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(gear_params PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
