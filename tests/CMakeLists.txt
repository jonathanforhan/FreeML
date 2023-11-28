cmake_minimum_required(VERSION 3.20)
project(FreeML_TESTS VERSION 1.0.0 LANGUAGES CXX C)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/spirv"
)

file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")
foreach (SHADER ${SHADERS})
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(
            COMMAND ${glslc_executable} ${SHADER} -o "spirv/${FILENAME}.spv"
            OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/spirv/${FILENAME}.spv"
            DEPENDS ${SHADER} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/spirv"
            COMMENT "Compiling ${FILENAME}"
    )
    list(APPEND SPV_SHADERS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/spirv/${FILENAME}.spv")
endforeach ()
add_custom_target(COMPILE_SHADERS ALL DEPENDS ${SPV_SHADERS})

function(TEST_PROJECT)
    file(GLOB_RECURSE C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    get_filename_component(DIRNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    add_executable(${DIRNAME})

    if (MSVC)
        set_target_properties(${DIRNAME} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} /W4")
    else ()
        set_target_properties(${DIRNAME} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
    endif ()

    target_sources(${DIRNAME} PUBLIC ${C_SOURCES})
    target_include_directories(${DIRNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${DIRNAME} PUBLIC FreeML)
    add_dependencies(${DIRNAME} COMPILE_SHADERS)
endfunction()

file(GLOB SUBDIRS LIST_DIRECTORIES true "*")
foreach (DIR ${SUBDIRS})
    get_filename_component(FILENAME ${DIR} NAME)
    if (IS_DIRECTORY ${DIR} AND NOT ${FILENAME} STREQUAL "shaders")
        add_subdirectory(${DIR})
    endif ()
endforeach ()