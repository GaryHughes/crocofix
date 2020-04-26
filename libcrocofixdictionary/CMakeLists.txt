set(LEXICOGRAPHER ${CMAKE_SOURCE_DIR}/lexicographer/lexicographer.py)

set(ORCHESTRATION_FIX42 "${CMAKE_SOURCE_DIR}/orchestrations/FIX Standard/FixRepository42.xml")
set(ORCHESTRATION_FIX44 "${CMAKE_SOURCE_DIR}/orchestrations/FIX Standard/FixRepository44.xml")
set(ORCHESTRATION_FIX50SP2 "${CMAKE_SOURCE_DIR}/orchestrations/FIX Standard/FixRepository50SP2.xml")
   
set(ORCHESTRATION_FIX44_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX44_version_fields.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX44_version_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX44_version_messages.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX44_version_messages.cpp 
)

set(ORCHESTRATION_FIX42_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX42_version_fields.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX42_version_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX42_version_messages.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX42_version_messages.cpp 
)

set(ORCHESTRATION_FIX50SP2_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX50SP2_version_fields.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX50SP2_version_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX50SP2_version_messages.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX50SP2_version_messages.cpp 
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX42_SOURCES}
    COMMAND ${LEXICOGRAPHER} crocofix::FIX_4_2 fix42_ ${ORCHESTRATION_FIX42}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX44_SOURCES}
    COMMAND ${LEXICOGRAPHER} crocofix::FIX_4_4 fix44_ ${ORCHESTRATION_FIX44}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX50SP2_SOURCES}
    COMMAND ${LEXICOGRAPHER} crocofix::FIX_5_0SP2 fix50SP2_ ${ORCHESTRATION_FIX50SP2}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(crocofixdictionary
    version_field.hpp
    version_field_collection.hpp
    field_value.hpp
    ${ORCHESTRATION_FIX42_SOURCES}
    ${ORCHESTRATION_FIX44_SOURCES}
    ${ORCHESTRATION_FIX50SP2_SOURCES}
)
