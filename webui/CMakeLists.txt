cmake_minimum_required(VERSION 3.20)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "webui must be configured with emcmake")
endif()

add_executable(gear_web
    wasm/wasm_api.cpp
    ../shared/src/point.cpp
    ../shared/src/gear_geometry.cpp
    ../cam/src/nc_converter.cpp
    ../cam/src/toolpath_pass.cpp
    ../cam/src/roughing.cpp
    ../cam/src/finishing.cpp
    ../cam/src/cam_generate.cpp
)

target_include_directories(gear_web PRIVATE
    ../shared/includes
    ../cam/includes
)

target_compile_features(gear_web PRIVATE cxx_std_17)
target_compile_options(gear_web PRIVATE -O3)
target_link_options(gear_web PRIVATE
    -O3
    "SHELL:-s WASM=1"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s MODULARIZE=1"
    "SHELL:-s EXPORT_NAME=createGearModule"
    "SHELL:-s ENVIRONMENT=web,worker"
    "SHELL:-s FILESYSTEM=0"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=['UTF8ToString','HEAPF32','HEAPU32']"
    "SHELL:-s EXPORTED_FUNCTIONS=['_gear_build_mesh','_gear_vertices','_gear_vertex_count','_gear_indices','_gear_index_count','_gear_last_error','_gear_generate_roughing','_gear_generate_finishing','_gear_text','_gear_text_size']"
)

set_target_properties(gear_web PROPERTIES
    OUTPUT_NAME "gear-wasm"
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist"
)
