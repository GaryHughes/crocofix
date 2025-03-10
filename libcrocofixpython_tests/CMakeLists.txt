find_package(Python COMPONENTS Interpreter Development REQUIRED)

# It is impossible to make target "test" depend on "all":
# https://gitlab.kitware.com/cmake/cmake/-/issues/8774
# Set a magic variable in a magic file that tells ctest
# to invoke the generator once before running the tests:
file(WRITE "${CMAKE_BINARY_DIR}/CTestCustom.cmake"
    "set(CTEST_CUSTOM_PRE_TEST ${CMAKE_MAKE_PROGRAM})\n"
)

add_test(NAME Field
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/field_tests.py
)

add_test(NAME FieldCollection
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/field_collection_tests.py
)

add_test(NAME Message
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/message_tests.py
)

add_test(NAME Types
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/type_tests.py
)

add_test(NAME SocketReader
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/socket_reader_tests.py
)

# add_test(NAME SocketsSession
#     COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/sockets_session_tests.py
# )

add_test(NAME StreamReader
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/stream_reader_tests.py
)

# add_test(NAME StreamsSession
#     COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/streams_session_tests.py
# )

set_tests_properties(
    Field
    FieldCollection
    Message
    Types
    SocketReader
    StreamReader
    # SocketsSession
    # StreamsSession 
    PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/../libcrocofixpython"
)
