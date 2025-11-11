cmake_minimum_required(VERSION 3.17.0)

project(crocofix)

# Badly broken in Homebrew LLVM version 20.1.2
# Emitting errors saying methods that clearly can't be static should be, that mutated variables should be const, and that member variables that are initialised in constructors are not.
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -checks=-*,clang-analyzer-*,bugprone-*,concurrency-*,cppcoreguidelines-*,misc-*,-misc-include-cleaner,modernize-*,performance-*,portability-*,readability-*,-modernize-use-trailing-return-type,-readability-magic-numbers,-cppcoreguidelines-avoid-magic-numbers,-misc-non-private-member-variables-in-classes,-cppcoreguidelines-non-private-member-variables-in-classes,-bugprone-chained-comparison;
    -warnings-as-errors=*;
    )

# Don't embed build tree paths
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "")

set(BOOST_VERSION_REQUIRED 1.89)

add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(Boost ${BOOST_VERSION_REQUIRED}
    CONFIG
    COMPONENTS 
    program_options
    iostreams
    REQUIRED
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

if(APPLE)

    set(BOOST_LIBS
        libboost_program_options.dylib
        libboost_iostreams.dylib
        libboost_thread.dylib
    )

else()

    set(BOOST_LIBS
        libboost_program_options.so
        libboost_iostreams.so
        libboost_thread.so
        pthread
    )
        
endif()

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
