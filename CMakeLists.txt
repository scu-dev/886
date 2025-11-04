cmake_minimum_required(VERSION "3.20")

project("886")

#------------Basic-----------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "" FORCE)
#----------------------------------

#----------Before Tweaks-----------
if(MSVC)
    add_link_options(/INCREMENTAL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor /utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()
#----------------------------------

#------------Clangd----------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
#----------------------------------

#-------------gperf----------------
find_program(GPERF_EXECUTABLE NAMES gperf)
if(NOT GPERF_EXECUTABLE)
    message(FATAL_ERROR "gperf not found! Please install gperf and ensure it is in your PATH.")
endif()

set(GPERF_INTRINSIC_IN ${CMAKE_CURRENT_SOURCE_DIR}/src/perfs/intrinsic.gperf)
set(GPERF_INTRINSIC_OUT ${CMAKE_CURRENT_BINARY_DIR}/intrinsic_hash.h)
add_custom_command(
    OUTPUT ${GPERF_INTRINSIC_OUT}
    COMMAND ${GPERF_EXECUTABLE} --language=C++ --readonly-tables --output-file=${GPERF_INTRINSIC_OUT} ${GPERF_INTRINSIC_IN}
    DEPENDS ${GPERF_INTRINSIC_IN}
    COMMENT "Generating ${GPERF_INTRINSIC_OUT} from ${GPERF_INTRINSIC_IN} with gperf"
    VERBATIM
)
set_source_files_properties(${GPERF_INTRINSIC_OUT} PROPERTIES GENERATED TRUE)
add_custom_target(gperf_intrinsic DEPENDS ${GPERF_INTRINSIC_OUT})

set(GPERF_CUSTOM_IN ${CMAKE_CURRENT_SOURCE_DIR}/src/perfs/custom.gperf)
set(GPERF_CUSTOM_OUT ${CMAKE_CURRENT_BINARY_DIR}/custom_hash.h)
add_custom_command(
    OUTPUT ${GPERF_CUSTOM_OUT}
    COMMAND ${GPERF_EXECUTABLE} --language=C++ --readonly-tables --output-file=${GPERF_CUSTOM_OUT} ${GPERF_CUSTOM_IN}
    DEPENDS ${GPERF_CUSTOM_IN}
    COMMENT "Generating ${GPERF_CUSTOM_OUT} from ${GPERF_CUSTOM_IN} with gperf"
    VERBATIM
)
set_source_files_properties(${GPERF_CUSTOM_OUT} PROPERTIES GENERATED TRUE)
add_custom_target(gperf_custom DEPENDS ${GPERF_CUSTOM_OUT})
#----------------------------------

#------------Files-----------------
file(GLOB_RECURSE 886_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
#----------------------------------

#---------------886----------------
add_executable(${PROJECT_NAME} ${886_SOURCES})
add_dependencies(${PROJECT_NAME} gperf_intrinsic)
add_dependencies(${PROJECT_NAME} gperf_custom)
#----------------------------------

#------------Inclusions------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    "libs/boost/assert/include"
    "libs/boost/config/include"
    "libs/boost/container_hash/include"
    "libs/boost/core/include"
    "libs/boost/describe/include"
    "libs/boost/mp11/include"
    "libs/boost/predef/include"
    "libs/boost/static_assert/include"
    "libs/boost/throw_exception/include"
    "libs/boost/unordered/include"
    "libs/cli11/include"
)
#----------------------------------

#------------Link------------------
if(WIN32)
    #Packing
    target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows")
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows/886.rc")
endif()
#----------------------------------