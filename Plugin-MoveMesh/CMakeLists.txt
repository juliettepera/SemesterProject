include (plugin)
openflipper_plugin (INSTALLDATA Icons)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang") #Tests for Clang and AppleClang which is defined in CMake 3.0 and higher
    message("Clang.")
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wall -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
#    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    add_definitions(-DSHAPEOP_CLANG)
    add_definitions(-DSHAPEOP_EXPORT)
        set(OPENMP OFF CACHE BOOL "OpenMP")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message("GCC.")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
#    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    add_definitions(-DSHAPEOP_GCC)
    add_definitions(-DSHAPEOP_EXPORT)
        set(OPENMP ON CACHE BOOL "OpenMP")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    message("ICC.")
    if(WIN32)
        set(CMAKE_CXX_FLAGS "/Qstd=c++11")
        set(CMAKE_CXX_FLAGS_RELEASE "/O3 /DNDEBUG")
        add_definitions(/DSHAPEOP_ICC)
        add_definitions(/DSHAPEOP_EXPORT)
    else()
        set(CMAKE_CXX_FLAGS "-std=c++11")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
        add_definitions(-DSHAPEOP_ICC)
        add_definitions(-DSHAPEOP_EXPORT)
    endif()
        set(OPENMP OFF CACHE BOOL "OpenMP")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message("MSVC.")
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT /Ox /Ot /fp:fast /GS- /GL /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /INCREMENTAL:NO /OPT:REF")
    add_definitions(/DSHAPEOP_MSVC)
    add_definitions(/DSHAPEOP_EXPORT)
        set(OPENMP ON CACHE BOOL "OpenMP")
else()
    message( FATAL_ERROR "CXX compiler not recognized. CMake will quit." )
endif()


# Find Eigen3

find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})

if(NOT EIGEN3_FOUND)

    message(ERROR " EIGEN not found.")

else()

    message("EIGEN found.")

endif()

