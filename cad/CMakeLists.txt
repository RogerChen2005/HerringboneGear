find_package(VTK REQUIRED COMPONENTS
    CommonCore
    CommonDataModel
    FiltersSources
    FiltersGeneral
    FiltersModeling
    IOXML
    IOGeometry
)

add_executable(HerringboneGearCAD
    src/main.cpp
    src/herringBoneGear.cpp
    src/profile.cpp
    src/stock.cpp
    src/sweep.cpp
)

target_include_directories(HerringboneGearCAD PRIVATE includes)
target_link_libraries(HerringboneGearCAD PRIVATE gear_params
    VTK::CommonCore
    VTK::FiltersSources
    VTK::FiltersGeneral
    VTK::FiltersModeling
    VTK::IOGeometry
)

vtk_module_autoinit(
    TARGETS HerringboneGearCAD
    MODULES VTK::IOGeometry
)
