cmake_minimum_required(VERSION 3.10.0)

project(crocofix)

# Don't embed build tree paths
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "")


set(BOOST_VERSION_REQUIRED 1.72)

find_package(Boost ${BOOST_VERSION_REQUIRED} EXACT
    COMPONENTS 
    program_options
    iostreams
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

if(APPLE)

    set(BOOST_LIBS
        libboost_program_options-mt.dylib
        libboost_iostreams-mt.dylib)

else()

    set(BOOST_LIBS
        libboost_program_options.so
        libboost_iostreams.so
        pthread)
        
endif()

enable_testing()

add_subdirectory(libcrocofixutility)
add_subdirectory(libcrocofixutility_tests)
add_subdirectory(libcrocofixdictionary)
add_subdirectory(libcrocofixdictionary_tests)
add_subdirectory(libcrocofix)
add_subdirectory(libcrocofix_tests)
add_subdirectory(fixcat)
add_subdirectory(fixsed)
