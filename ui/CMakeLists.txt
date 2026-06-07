if(WIN32)
    find_package(Qt6 COMPONENTS Widgets REQUIRED)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

find_package(VTK REQUIRED COMPONENTS
    GUISupportQt
    RenderingOpenGL2
    InteractionStyle
    RenderingCore
    RenderingFreeType
)

set(CMAKE_AUTOMOC ON)

aux_source_directory(src/ UI_SOURCES)

if(WIN32)
    add_executable(GearUI WIN32
        ${UI_SOURCES}
        includes/mainwindow.h
    )
else()
    add_executable(GearUI
        ${UI_SOURCES}
        includes/mainwindow.h
    )
endif()

target_include_directories(GearUI PRIVATE includes)

target_link_libraries(GearUI PRIVATE
    Qt6::Widgets
    Gear::CAD_Core
    Gear::CAM_Core
    VTK::GUISupportQt
    VTK::RenderingOpenGL2
    VTK::InteractionStyle
    VTK::RenderingCore
    VTK::RenderingFreeType
)

if(WIN32)
    target_link_libraries(GearUI PRIVATE
        Qt6::Widgets
    )
else()
    target_link_libraries(GearUI PRIVATE
        Qt5::Widgets
    )
endif()

vtk_module_autoinit(
    TARGETS GearUI
    MODULES VTK::RenderingOpenGL2 VTK::GUISupportQt
)

if(WIN32)
    add_custom_command(TARGET GearUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:GearUI>/plugins/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "$ENV{USERPROFILE}/vcpkg/installed/x64-windows/Qt6/plugins/platforms"
            "$<TARGET_FILE_DIR:GearUI>/platforms"
    )
endif()
