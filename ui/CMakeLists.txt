find_package(Qt5 COMPONENTS Widgets REQUIRED)

find_package(VTK REQUIRED COMPONENTS
    GUISupportQt
    RenderingOpenGL2
    InteractionStyle
    RenderingCore
    RenderingFreeType
)

set(CMAKE_AUTOMOC ON)

aux_source_directory(src/ UI_SOURCES)

add_executable(GearUI
    ${UI_SOURCES}
)

target_include_directories(GearUI PRIVATE includes)

target_link_libraries(GearUI PRIVATE
    Qt5::Widgets
    Gear::CAD_Core
    Gear::CAM_Core
    VTK::GUISupportQt
    VTK::RenderingOpenGL2
    VTK::InteractionStyle
    VTK::RenderingCore
    VTK::RenderingFreeType
)

vtk_module_autoinit(
    TARGETS GearUI
    MODULES VTK::RenderingOpenGL2 VTK::GUISupportQt
)
