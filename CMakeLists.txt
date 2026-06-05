cmake_minimum_required(VERSION 3.12)
project(HerringboneGear)

set(CMAKE_CXX_STANDARD 17)

find_package(VTK REQUIRED COMPONENTS
    CommonCore
    CommonDataModel
    FiltersSources
    FiltersGeneral
    FiltersModeling
    IOXML
    IOGeometry
)

aux_source_directory(src/ SOURCE_FILE)

add_executable(HerringboneGear
    ${SOURCE_FILE}
)

target_include_directories(HerringboneGear PRIVATE includes)

target_link_libraries(HerringboneGear PRIVATE
    VTK::CommonCore
    VTK::FiltersSources
    VTK::FiltersGeneral
    VTK::FiltersModeling
    VTK::IOGeometry
)

vtk_module_autoinit(
    TARGETS HerringboneGear
    MODULES VTK::IOGeometry
)
