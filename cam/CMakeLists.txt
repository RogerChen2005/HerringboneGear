aux_source_directory(src/ CAM_SOURCES)

# Static library for reuse by UI and other targets
add_library(cam_core STATIC ${CAM_SOURCES})
add_library(Gear::CAM_Core ALIAS cam_core)

target_include_directories(cam_core PUBLIC includes)
target_link_libraries(cam_core PUBLIC gear_params)

# Standalone CAM executable
add_executable(HerringboneGearCAM main.cpp)
target_link_libraries(HerringboneGearCAM PRIVATE Gear::CAM_Core)
