find_package(nlohmann_json REQUIRED)

aux_source_directory(src SHARED_SOURCES)

add_library(gear_params
    ${SHARED_SOURCES}
)
target_include_directories(gear_params PUBLIC includes)
add_library(Gear::Params ALIAS gear_params)

target_link_libraries(gear_params PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(gear_params PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
