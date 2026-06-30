cmake_minimum_required(VERSION 3.17.0)

project(crocofix)

# set(CMAKE_CXX_CLANG_TIDY
#     clang-tidy;
#     -checks=-*,clang-analyzer-*,bugprone-*,concurrency-*,cppcoreguidelines-*,misc-*,-misc-include-cleaner,modernize-*,performance-*,portability-*,readability-*,-modernize-use-trailing-return-type,-readability-magic-numbers,-cppcoreguidelines-avoid-magic-numbers,-misc-non-private-member-variables-in-classes,-cppcoreguidelines-non-private-member-variables-in-classes,-bugprone-chained-comparison;
#     -warnings-as-errors=*;
#     )

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Don't embed build tree paths
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "")

set(BOOST_VERSION_REQUIRED 1.89)

add_definitions(-DBOOST_LOG_DYN_LINK)

# Several vcpkg-managed dependencies (Catch2, fmt, ...) have same-named
# Homebrew packages also installed on this machine, sharing the
# /opt/homebrew/include prefix that Boost's own include dir pulls in below.
# Search vcpkg's own include dir first so its headers always win over a
# version-mismatched Homebrew copy of the same library.
if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
    include_directories(${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
endif()

find_package(Boost ${BOOST_VERSION_REQUIRED}
    CONFIG
    COMPONENTS 
    program_options
    iostreams
    thread
    REQUIRED
)

include_directories(${Boost_INCLUDE_DIRS})

set(BOOST_LIBS
    Boost::headers
    Boost::program_options
    Boost::iostreams
    Boost::thread
)

find_package(Catch2 3 REQUIRED)

include_directories(
    "/usr/local/include"
    ${CMAKE_SOURCE_DIR}
)

link_directories(
    "/usr/local/lib"
    ${Boost_LIBRARY_DIRS}
)

macro(CHECK_DTRACE)
    find_program(DTRACE dtrace)
    if(DTRACE)
        set(HAVE_DTRACE true)
    endif()
    execute_process(COMMAND ${DTRACE} -V OUTPUT_VARIABLE out)
endmacro()

CHECK_DTRACE()

enable_testing()
include(CTest)
include(Catch)

add_subdirectory(libcrocofixutility)
add_subdirectory(libcrocofixutility_tests)
add_subdirectory(libcrocofixdictionary)
add_subdirectory(libcrocofixdictionary_tests)
add_subdirectory(libcrocofix)
add_subdirectory(libcrocofix_tests)
add_subdirectory(libcrocofixlua)
add_subdirectory(libcrocofixlua_tests)
add_subdirectory(libcrocofixpython)
add_subdirectory(libcrocofixpython_tests) 
add_subdirectory(fixcat)
add_subdirectory(fixsed)
add_subdirectory(examples)
