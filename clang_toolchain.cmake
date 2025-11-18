set(CLANG_ROOT "/usr")

set(CMAKE_C_COMPILER             "${CLANG_ROOT}/bin/clang")
set(CMAKE_C_FLAGS                "-Wall -fPIC")
set(CMAKE_C_FLAGS_DEBUG          "-g")
set(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE        "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_CXX_COMPILER             "${CLANG_ROOT}/bin/clang++")
set(CMAKE_CXX_FLAGS                "-Wall -ftemplate-backtrace-limit=0 -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG          "-g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

add_definitions(-Wno-mismatched-tags)
add_definitions(-Wno-delete-non-abstract-non-virtual-dtor)
add_definitions(-D _WCHAR_T=int)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)