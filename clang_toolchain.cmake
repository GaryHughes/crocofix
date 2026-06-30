if(APPLE)
    # Apple's Xcode Clang does not ship clang-scan-deps, which CMake needs to
    # discover C++20 module dependencies. Use Homebrew LLVM instead.
    set(CLANG_ROOT "/opt/homebrew/opt/llvm")

    # This machine has separate physical copies of the macOS SDK under
    # CommandLineTools and under Xcode.app. Homebrew Clang defaults to the
    # CommandLineTools copy, but CMake's Apple-platform find_package() calls
    # (e.g. Iconv, pulled in by Python's Development component) can resolve
    # against the Xcode.app copy instead. Mixing headers from both copies in
    # one translation unit fools their include guards (identical guard macro
    # names, different files) into skipping real typedefs. Pin a single SDK
    # so every lookup and the compiler's own -isysroot agree.
    execute_process(
        COMMAND xcrun --sdk macosx --show-sdk-path
        OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(CLANG_ROOT "/usr")
endif()

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

if(NOT APPLE)
    # Pre-defining _WCHAR_T avoids a wchar_t redefinition clash on Linux. On
    # macOS it instead defeats the Apple SDK headers' own "#ifndef _WCHAR_T"
    # include guards, leaving __darwin_wint_t/__darwin_wchar_t undeclared.
    add_definitions(-D _WCHAR_T=int)
else()
    # Homebrew LLVM's libc++ tags _LIBCPP_HIDE_FROM_ABI symbols with an
    # __abi_tag__ attribute. When a TU both imports a C++20 named module and
    # textually #includes a libc++ header that the module's BMI already
    # declared internally (e.g. <array> pulling in __algorithm/equal.h,
    # which redeclares __bit_reference's __equal_unaligned), Clang sees the
    # attribute applied twice via two different declaration paths and
    # rejects it as a redeclaration. _LIBCPP_NO_ABI_TAG is libc++'s own
    # documented escape hatch (__config) that drops the attribute entirely,
    # removing the only difference between the two declarations.
    add_definitions(-D _LIBCPP_NO_ABI_TAG)
endif()

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)