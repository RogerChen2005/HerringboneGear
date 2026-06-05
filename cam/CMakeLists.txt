add_executable(HerringboneGearCAM
    src/main.cpp
    src/nc_converter.cpp
    src/roughing.cpp
    src/finishing.cpp
)

target_include_directories(HerringboneGearCAM PRIVATE includes)
target_link_libraries(HerringboneGearCAM PRIVATE gear_params)
