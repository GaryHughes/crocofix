set(LEXICOGRAPHER ${CMAKE_SOURCE_DIR}/lexicographer/lexicographer.py)

set(ORCHESTRATION_FIX44 "${CMAKE_SOURCE_DIR}/orchestrations/FIX Standard/FixRepository44.xml")
   
set(ORCHESTRATION_FIX44_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX44_version_fields.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/FIX44_version_fields.cpp 
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX44_SOURCES}
    COMMAND ${LEXICOGRAPHER} crocofix::FIX_4_4 fix44_ ${ORCHESTRATION_FIX44}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(crocofixdictionary
    version_field.hpp
    version_field_collection.hpp
    field_value.hpp
    ${ORCHESTRATION_FIX44_SOURCES}
)
