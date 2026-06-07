find_package(VTK REQUIRED COMPONENTS
    CommonCore
    CommonDataModel
    FiltersSources
    FiltersGeneral
    FiltersModeling
    IOXML
    IOGeometry
)

aux_source_directory(src/ CAD_SOURCES)

# Static library for reuse by UI and other targets
add_library(cad_core STATIC ${CAD_SOURCES})
add_library(Gear::CAD_Core ALIAS cad_core)

target_include_directories(cad_core PUBLIC includes)
target_link_libraries(cad_core PUBLIC gear_params
    VTK::CommonCore
    VTK::FiltersSources
    VTK::FiltersGeneral
    VTK::FiltersModeling
    VTK::IOGeometry
)
vtk_module_autoinit(
    TARGETS cad_core
    MODULES VTK::IOGeometry
)

# Standalone CAD executable
add_executable(HerringboneGearCAD main.cpp)
target_link_libraries(HerringboneGearCAD PRIVATE Gear::CAD_Core)
