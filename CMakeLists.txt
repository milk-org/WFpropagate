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
            ${MILK_INCLUDE_DIR}/libmilkdata
            ${MILK_INCLUDE_DIR}/ImageStreamIO
        )
    endif()

    target_include_directories(${LIBNAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/${SRCNAME}>
        ${MILK_STANDALONE_INCLUDES}
    )

    set(MILK_LIB_NAMES
        milkOpticsMaterials
        CLIcore
        ImageStreamIO
        milkCOREMODmemory
        milkCOREMODarith
        milkCOREMODiofits
        milkCOREMODtools
        milkfft
    )

    set(MILK_SEARCH_HINTS
        ${MILK_ROOT}/lib
        $ENV{MILK_ROOT}/lib
        /usr/local/milk/lib
        /usr/local/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/../OpticsMaterials/_build
    )
    if(MILK_SOURCE_FOUND)
        list(APPEND MILK_SEARCH_HINTS
            ${MILK_SOURCE_FOUND}/_build/lib
            ${MILK_SOURCE_FOUND}/_build/src/cli/CLIcore
            ${MILK_SOURCE_FOUND}/_build/src/engine/ImageStreamIO
            ${MILK_SOURCE_FOUND}/_build/src/coremods/COREMOD_memory
            ${MILK_SOURCE_FOUND}/_build/src/coremods/COREMOD_arith
            ${MILK_SOURCE_FOUND}/_build/src/coremods/COREMOD_iofits
            ${MILK_SOURCE_FOUND}/_build/src/coremods/COREMOD_tools
            ${MILK_SOURCE_FOUND}/_build/plugins/milk-extra-src/fft
            ${MILK_SOURCE_FOUND}/_build/plugins/milk-extra-src/OpticsMaterials
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

# ── Standalone executable (FPS V2 pattern) ───────────────────────
if(NOT COMMAND add_milk_standalone AND MILK_SOURCE_FOUND AND EXISTS "${MILK_SOURCE_FOUND}/cmake/milk_standalone.cmake")
    if(EXISTS "${MILK_SOURCE_FOUND}/cmake/milk_extensions.cmake")
        include("${MILK_SOURCE_FOUND}/cmake/milk_extensions.cmake")
    endif()
    include("${MILK_SOURCE_FOUND}/cmake/milk_standalone.cmake")
endif()

if(COMMAND add_milk_standalone)
    link_directories(/usr/local/milk/lib ${MILK_SEARCH_HINTS})
    add_milk_standalone(wfprop-fresnel wfprop_fresnel_FPS.c)
    target_include_directories(milk-fpsexec-wfprop-fresnel PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(milk-fpsexec-wfprop-fresnel PRIVATE ${LIBNAME})
endif()
