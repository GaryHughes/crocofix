cmake_minimum_required(VERSION 3.10.0)

project(fixtools)

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

add_subdirectory(libfixdictionary)
add_subdirectory(libfixdictionary_tests)
add_subdirectory(libfix)
add_subdirectory(libfix_tests)
