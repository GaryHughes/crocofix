find_package(Python COMPONENTS Interpreter REQUIRED)

add_test(NAME Message
    COMMAND  ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/message.py
)

set_tests_properties(Message PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/../libcrocofixpython")
