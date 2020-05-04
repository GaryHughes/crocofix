set(LEXICOGRAPHER ${CMAKE_SOURCE_DIR}/lexicographer/lexicographer.py)

set(ORCHESTRATION_FIX42 "${CMAKE_SOURCE_DIR}/orchestrations/FIX Standard/FixRepository42.xml")
set(ORCHESTRATION_FIX44 "${CMAKE_SOURCE_DIR}/orchestrations/FIX Standard/FixRepository44.xml")
set(ORCHESTRATION_FIX50SP2 "${CMAKE_SOURCE_DIR}/orchestrations/FIX Standard/FixRepository50SP2.xml")
   
set(ORCHESTRATION_FIX44_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_fields.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_messages.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_messages.cpp 
)

set(ORCHESTRATION_FIX42_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_fields.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_messages.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_messages.cpp 
)

set(ORCHESTRATION_FIX50SP2_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_fields.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_messages.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_messages.cpp 
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX42_SOURCES}
    COMMAND ${LEXICOGRAPHER} --namespace crocofix::FIX_4_2 --prefix fix42_ --orchestration ${ORCHESTRATION_FIX42}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX44_SOURCES}
    COMMAND ${LEXICOGRAPHER} --namespace crocofix::FIX_4_4 --prefix fix44_ --orchestration ${ORCHESTRATION_FIX44}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX50SP2_SOURCES}
    COMMAND ${LEXICOGRAPHER} --namespace crocofix::FIX_5_0SP2 --prefix fix50SP2_ --orchestration ${ORCHESTRATION_FIX50SP2}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(crocofixdictionary
    version_field.hpp
    version_field_collection.hpp
    field_value.hpp
    message_field.hpp
    message_field_collection.hpp
    message.hpp
    message_collection.hpp
    ${ORCHESTRATION_FIX42_SOURCES}
    ${ORCHESTRATION_FIX44_SOURCES}
    ${ORCHESTRATION_FIX50SP2_SOURCES}
)
