find_package(Python COMPONENTS Interpreter REQUIRED)

# It is impossible to make target "test" depend on "all":
# https://gitlab.kitware.com/cmake/cmake/-/issues/8774
# Set a magic variable in a magic file that tells ctest
# to invoke the generator once before running the tests:
file(WRITE "${CMAKE_BINARY_DIR}/CTestCustom.cmake"
    "set(CTEST_CUSTOM_PRE_TEST ${CMAKE_MAKE_PROGRAM})\n"
)

add_test(NAME Field
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/field.py
)

add_test(NAME FieldCollection
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/field_collection.py
)

add_test(NAME Message
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/message.py
)

add_test(NAME Session
    COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/session.py
)

set_tests_properties(
    Field
    FieldCollection
    Message
    Session 
    PROPERTIES 
    ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/../libcrocofixpython")
