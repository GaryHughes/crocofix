set(LEXICOGRAPHER ${CMAKE_SOURCE_DIR}/lexicographer/lexicographer.py)

set(ORCHESTRATION_FIX42 "${CMAKE_SOURCE_DIR}/orchestrations/fix_repository_4_2.xml")
set(ORCHESTRATION_FIX44 "${CMAKE_SOURCE_DIR}/orchestrations/fix_repository_4_4.xml")
set(ORCHESTRATION_FIX50SP2 "${CMAKE_SOURCE_DIR}/orchestrations/fix_repository_5_0SP2_EP258.xml")

set(ORCHESTRATION_FIX44_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_fields.cppm 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_messages.cppm 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_orchestration.cppm
)

set(ORCHESTRATION_FIX44_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix44_messages.cpp
)

set(ORCHESTRATION_FIX42_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_fields.cppm 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_messages.cppm 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_orchestration.cppm
)

set(ORCHESTRATION_FIX42_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix42_messages.cpp 
)

set(ORCHESTRATION_FIX50SP2_INTERFACES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_fields.cppm 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_messages.cppm 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_orchestration.cppm
)

set(ORCHESTRATION_FIX50SP2_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_fields.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/fix50SP2_messages.cpp 
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX42_INTERFACES} ${ORCHESTRATION_FIX42_SOURCES}
    COMMAND ${LEXICOGRAPHER} --namespace crocofix::FIX_4_2 --module crocofix.dictionary --partition FIX_4_2 --prefix fix42_ --orchestration ${ORCHESTRATION_FIX42}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX44_INTERFACES} ${ORCHESTRATION_FIX44_SOURCES}
    COMMAND ${LEXICOGRAPHER} --namespace crocofix::FIX_4_4 --module crocofix.dictionary --partition FIX_4_4 --prefix fix44_ --orchestration ${ORCHESTRATION_FIX44}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${ORCHESTRATION_FIX50SP2_INTERFACES} ${ORCHESTRATION_FIX50SP2_SOURCES}
    COMMAND ${LEXICOGRAPHER} --namespace crocofix::FIX_5_0SP2 --module crocofix.dictionary --partition FIX_5_0SP2 --prefix fix50SP2_ --orchestration ${ORCHESTRATION_FIX50SP2}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(crocofixdictionary)

target_sources(crocofixdictionary
    PUBLIC
        FILE_SET CXX_MODULES FILES
            orchestration_field.cppm
            orchestration_field_collection.cppm
            field_value.cppm
            message_field.cppm
            message_field_collection.cppm
            message.cppm
            message_collection.cppm
            orchestration.cppm
            orchestra.cppm
            pedigree.cppm
            presence.cppm
            crocofixdictionary.cppm
            ${ORCHESTRATION_FIX42_INTERFACES}
            ${ORCHESTRATION_FIX44_INTERFACES}
            ${ORCHESTRATION_FIX50SP2_INTERFACES}
    PRIVATE
        ${ORCHESTRATION_FIX42_SOURCES}
        ${ORCHESTRATION_FIX44_SOURCES}
        ${ORCHESTRATION_FIX50SP2_SOURCES}
)

# clang-tidy takes too long with the generated files. There doesn't seem to be a way to disable this
# for individual source files so just turn off for the whole target. We could move the files into
# another library but thats not great.
set_target_properties(
    crocofixdictionary
    PROPERTIES
         CXX_CLANG_TIDY ""
)