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
    IOGeometry
)

set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

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
    Gear::CAD_Core
    Gear::CAM_Core
    VTK::GUISupportQt
    VTK::RenderingOpenGL2
    VTK::InteractionStyle
    VTK::RenderingCore
    VTK::RenderingFreeType
    VTK::IOGeometry
)

if(MSVC)
    target_compile_options(GearUI PRIVATE /O2 /GL)
    target_link_options(GearUI PRIVATE /LTCG /OPT:REF /OPT:ICF)
endif()

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
    qt_generate_deploy_app_script(
        TARGET GearUI
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )

    install(SCRIPT ${deploy_script})
    install(TARGETS GearUI DESTINATION .)
endif()
