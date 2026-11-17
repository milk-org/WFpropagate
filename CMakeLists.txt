# =====================================================================
# milkWFpropagate - Optical Wavefront Fresnel Propagation
# =====================================================================

cmake_minimum_required(VERSION 3.16)

set(LIBNAME "milkWFpropagate")
set(SRCNAME "WFpropagate")

project(lib_${LIBNAME}_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED fftw3)
pkg_check_modules(FFTWF REQUIRED fftw3f)

set(SOURCEFILES
    WFpropagate.c
    wfprop_fresnel.c
    wfprop_fresnel_engine.c
    wfprop_fresnel_FPS.c
    wfprop_cube.c
    wfprop_lyot.c
)

set(INCLUDEFILES
    WFpropagate.h
    wfprop_fresnel_engine.h
    wfprop_fresnel_FPS.h
)

# CUDA GPU Acceleration
if(HAVE_CUDA OR USE_CUDA)
    include(CheckLanguage)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        enable_language(CUDA OPTIONAL)
        find_package(CUDAToolkit QUIET)
        if(CUDAToolkit_FOUND)
            set(USE_CUDA_ACCEL TRUE)
            add_compile_definitions(HAVE_CUDA)
            list(APPEND SOURCEFILES wfprop_fresnel_cuda.cu)
            list(APPEND INCLUDEFILES wfprop_fresnel_cuda.h)
            message(STATUS "milkWFpropagate: CUDA GPU acceleration ENABLED")
        endif()
    endif()
endif()

add_library(${LIBNAME} SHARED ${SOURCEFILES})

set_target_properties(${LIBNAME} PROPERTIES
    LINKER_LANGUAGE C
    INTERPROCEDURAL_OPTIMIZATION OFF
)

if(TARGET CLIcore)
    # In-tree build under milk framework
    target_include_directories(${LIBNAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/${SRCNAME}>
    )
    target_include_directories(${LIBNAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/coremods
        ${CMAKE_SOURCE_DIR}/plugins
        ${CMAKE_SOURCE_DIR}/plugins/milk-extra-src
        $<TARGET_PROPERTY:CLIcore,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(${LIBNAME} PUBLIC
        milkOpticsMaterials
        CLIcore
        ImageStreamIO
        milkCOREMODmemory
        milkCOREMODarith
        milkCOREMODiofits
        milkCOREMODtools
        milkfft
        OpenMP::OpenMP_C
        ${FFTW_LIBRARIES}
        ${FFTWF_LIBRARIES}
        m
    )
else()
    # Out-of-tree / standalone build
    set(MILK_SOURCE_CANDIDATES
        ${MILK_SOURCE_DIR}
        $ENV{MILK_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../milk-framework-dev
        $ENV{MILK_ROOT}
        ${CMAKE_CURRENT_SOURCE_DIR}/../milk
        /usr/local/milk
    )
    set(MILK_SOURCE_FOUND "")
    foreach(cand ${MILK_SOURCE_CANDIDATES})
        if(cand AND EXISTS "${cand}/src/coremods")
            get_filename_component(MILK_SOURCE_FOUND "${cand}" ABSOLUTE)
            break()
        endif()
    endforeach()

    find_path(MILK_INCLUDE_DIR
        NAMES CommandLineInterface/CLIcore.h CLIcore.h
        HINTS
            ${MILK_ROOT}/include
            $ENV{MILK_ROOT}/include
            /usr/local/milk/include
            /usr/local/include
    )

    set(MILK_STANDALONE_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../OpticsMaterials")
        list(APPEND MILK_STANDALONE_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/../OpticsMaterials")
    endif()
    if(MILK_SOURCE_FOUND)
        list(APPEND MILK_STANDALONE_INCLUDES
            ${MILK_SOURCE_FOUND}/src
            ${MILK_SOURCE_FOUND}/src/coremods
            ${MILK_SOURCE_FOUND}/plugins/milk-extra-src
            ${MILK_SOURCE_FOUND}/plugins/milk-extra-src/OpticsMaterials
            ${MILK_SOURCE_FOUND}/src/cli
            ${MILK_SOURCE_FOUND}/src/cli/CLIcore
            ${MILK_SOURCE_FOUND}/src/engine
            ${MILK_SOURCE_FOUND}/src/engine/libfps
            ${MILK_SOURCE_FOUND}/src/engine/libmilkcommon
            ${MILK_SOURCE_FOUND}/src/engine/libprocessinfo
            ${MILK_SOURCE_FOUND}/src/engine/libmilkdata
            ${MILK_SOURCE_FOUND}/src/engine/libcomputeunit
            ${MILK_SOURCE_FOUND}/src/engine/ImageStreamIO
        )
        if(EXISTS "${MILK_SOURCE_FOUND}/_build/config")
            list(APPEND MILK_STANDALONE_INCLUDES "${MILK_SOURCE_FOUND}/_build/config")
        endif()
    endif()

    if(MILK_INCLUDE_DIR)
        list(APPEND MILK_STANDALONE_INCLUDES
            ${MILK_INCLUDE_DIR}
            ${MILK_INCLUDE_DIR}/CommandLineInterface
            ${MILK_INCLUDE_DIR}/CommandLineInterface/CLIcore
            ${MILK_INCLUDE_DIR}/libmilkcommon
            ${MILK_INCLUDE_DIR}/libfps
            ${MILK_INCLUDE_DIR}/libprocessinfo
            ${MILK_INCLUDE_DIR}/libmilkdata
            ${MILK_INCLUDE_DIR}/libcomputeunit
            ${MILK_INCLUDE_DIR}/ImageStreamIO
        )
    endif()

    target_compile_definitions(${LIBNAME} PRIVATE MILK_NO_CLI)

    target_include_directories(${LIBNAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/${SRCNAME}>
        ${MILK_STANDALONE_INCLUDES}
        ${FFTW_INCLUDE_DIRS}
        ${FFTWF_INCLUDE_DIRS}
    )

    set(MILK_LIB_NAMES
        milkcomputeunit
        milkfps
        milkprocessinfo
        milkdata
        milkcommon
        milkOpticsMaterials
        ImageStreamIO
        milkCOREMODmemory
        milkCOREMODarith
        milkCOREMODiofits
        milkCOREMODtools
        milkfft
        milkimagegen
    )

    set(MILK_SEARCH_HINTS
        ${MILK_ROOT}/lib
        $ENV{MILK_ROOT}/lib
        ${CMAKE_INSTALL_PREFIX}/lib
        /usr/local/milk/lib
        /usr/local/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/../OpticsMaterials/_build
    )
    if(MILK_SOURCE_FOUND)
        list(APPEND MILK_SEARCH_HINTS
            ${MILK_SOURCE_FOUND}/_build/lib
        )
    endif()

    set(MILK_LIBRARIES "")
    foreach(lib ${MILK_LIB_NAMES})
        find_library(MILK_${lib}_LIB NAMES ${lib} HINTS ${MILK_SEARCH_HINTS})
        if(MILK_${lib}_LIB)
            list(APPEND MILK_LIBRARIES ${MILK_${lib}_LIB})
        else()
            list(APPEND MILK_LIBRARIES ${lib})
        endif()
    endforeach()

    target_link_libraries(${LIBNAME} PUBLIC
        ${MILK_LIBRARIES}
        OpenMP::OpenMP_C
        ${FFTW_LIBRARIES}
        ${FFTWF_LIBRARIES}
        m
    )
endif()

if(USE_CUDA_ACCEL)
    target_link_libraries(${LIBNAME} PRIVATE CUDA::cudart CUDA::cufft)
endif()

install(TARGETS ${LIBNAME} EXPORT milkTargets DESTINATION lib)
install(FILES ${INCLUDEFILES} DESTINATION include/${SRCNAME})
install(FILES ${INCLUDEFILES} DESTINATION include)

# ── Standalone fpsexec stubs parsed from EXPORT_FPSEXEC(...) ───────
set(_FPSEXEC_STUB_TPL [=[
#include "milk_skinny_runnable.h"

extern const MILK_RUNNABLE @UNIT@;

int main(int argc, char *argv[])
{
    MILK_EMBED_BUILD_TAG();
    return MILK_FPSEXEC_MAIN(argc, argv, &@UNIT@);
}
]=])

string(CONCAT _FPSEXEC_RE
    "EXPORT_FPSEXEC[ \t]*\\([ \t]*([A-Za-z0-9_]+)[ \t]*,[ \t]*"
    "\"([^\"]+)\"[ \t]*,[ \t]*([A-Za-z_][A-Za-z0-9_]*)[ \t]*\\)"
)

foreach(_rel_src IN LISTS SOURCEFILES)
    set(_abs_src "${CMAKE_CURRENT_SOURCE_DIR}/${_rel_src}")
    if(NOT EXISTS "${_abs_src}")
        continue()
    endif()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_abs_src}")
    file(READ "${_abs_src}" _src_text)
    string(REGEX MATCHALL "(^|\n)[ \t]*EXPORT_FPSEXEC[ \t]*\\([^\n]*\\)"
           _fpsexec_hits "${_src_text}")
    foreach(_hit IN LISTS _fpsexec_hits)
        if(_hit MATCHES "${_FPSEXEC_RE}")
            set(_exe "${CMAKE_MATCH_1}-fpsexec-${CMAKE_MATCH_2}")
            set(_unit "${CMAKE_MATCH_3}")
            string(REPLACE "@UNIT@" "${_unit}" _stub_src "${_FPSEXEC_STUB_TPL}")
            set(_stub_file "${CMAKE_CURRENT_BINARY_DIR}/fpsexec_stubs/${_exe}.c")
            file(GENERATE OUTPUT "${_stub_file}" CONTENT "${_stub_src}")
            add_executable(${_exe} "${_stub_file}")
            target_compile_definitions(${_exe} PRIVATE MILK_NO_CLI MILK_BUILD_BINNAME="${_exe}")
            target_include_directories(${_exe} SYSTEM PRIVATE ${MILK_STANDALONE_INCLUDES})
            target_link_libraries(${_exe} PRIVATE
                ${LIBNAME}
                ${MILK_LIBRARIES}
                m
                rt
                pthread
                -Wl,--allow-shlib-undefined
            )
            set_target_properties(${_exe} PROPERTIES
                BUILD_RPATH "${CMAKE_CURRENT_BINARY_DIR};${CMAKE_INSTALL_RPATH}"
                INSTALL_RPATH "$ORIGIN/../lib;${CMAKE_INSTALL_RPATH}"
            )
            install(TARGETS ${_exe} DESTINATION bin)
        endif()
    endforeach()
endforeach()

install(CODE "
    set(_milk_cli \"\${CMAKE_INSTALL_PREFIX}/bin/milk-cli\")
    if(EXISTS \"\${_milk_cli}\")
        execute_process(COMMAND \"\${_milk_cli}\" --catalog RESULT_VARIABLE _cat_rc)
    endif()
")
