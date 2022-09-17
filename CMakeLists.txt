cmake_minimum_required(VERSION 3.10.0)

project(crocofix)

set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -checks=-*,clang-analyzer-*,bugprone-*,concurrency-*,cppcoreguidelines-*,misc-*,modernize-*,performance-*,portability-*,readability-*,-modernize-use-trailing-return-type,-readability-magic-numbers,-cppcoreguidelines-avoid-magic-numbers;
    -warnings-as-errors=*;
    )

# Don't embed build tree paths
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "")

set(BOOST_VERSION_REQUIRED 1.80)

add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(Boost ${BOOST_VERSION_REQUIRED} 
    COMPONENTS 
    program_options
    iostreams
    log
    REQUIRED
)

include_directories(
    "/usr/local/include"
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/thirdparty/catch2/include
    ${CMAKE_SOURCE_DIR}/thirdparty
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
        libboost_program_options-mt.dylib
        libboost_iostreams-mt.dylib
        libboost_log-mt.dylib
        libboost_log_setup-mt.dylib
        libboost_thread-mt.dylib
    )

else()

    set(BOOST_LIBS
        libboost_program_options.so
        libboost_iostreams.so
        libboost_log.so
        libboost_log_setup.so
        libboost_thread.so
        pthread
    )
        
endif()

enable_testing()
include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/thirdparty/catch2/contrib")
include(Catch)

add_subdirectory(libcrocofixutility)
add_subdirectory(libcrocofixutility_tests)
add_subdirectory(libcrocofixdictionary)
add_subdirectory(libcrocofixdictionary_tests)
add_subdirectory(libcrocofix)
add_subdirectory(libcrocofix_tests)
add_subdirectory(libcrocofixlua)
add_subdirectory(libcrocofixlua_tests) 
add_subdirectory(fixcat)
add_subdirectory(fixsed)
add_subdirectory(examples)
