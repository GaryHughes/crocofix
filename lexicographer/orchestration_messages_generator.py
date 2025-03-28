#!/usr/bin/python3

from sanitise import *

def generate_orchestration_messages(namespace, prefix, orchestration):
    sane_prefix = sanitise_for_include_guard(prefix)
    header_filename = '{}messages.hpp'.format(prefix)
    with open(header_filename, 'w') as file:
        header = \
'''#ifndef crocofix_libcrocofixdictionary_{}messages_hpp
#define crocofix_libcrocofixdictionary_{}messages_hpp

#include <libcrocofixdictionary/message.hpp>
#include <libcrocofixdictionary/message_collection.hpp>

namespace {}
{{
namespace message
{{ 

'''.format(sane_prefix, sane_prefix, namespace)
        file.write(header)
    
        for message in orchestration.messages.values():
            file.write('''class {} : public crocofix::dictionary::message
{{
public:

    static constexpr const char* MsgType = "{}";

'''.format(message.name, message.msg_type))

            file.write('    {}();\n'.format(message.name))

            file.write('''
};

''')

        trailer = \
'''}
const crocofix::dictionary::message_collection& messages() noexcept;

}

#endif
'''
        file.write(trailer)


    source_filename = '{}messages.cpp'.format(prefix)
    with open(source_filename, 'w') as file:
        header = \
'''#include "{}messages.hpp"
#include "{}fields.hpp"

namespace {}
{{
namespace message
{{

'''.format(prefix, prefix, namespace)

        file.write(header)

        for message in orchestration.messages.values():

            file.write('''{}::{}()
:    crocofix::dictionary::message(
         "{}", 
         "{}", 
         "{}", 
         "{}",
         crocofix::dictionary::pedigree("{}", "{}", "{}", "{}", "{}", "{}"),
         {{
'''.format(message.name, message.name, message.name, message.msg_type, message.category, 
            sanitise(message.synopsis), sanitise(message.pedigree.added), sanitise(message.pedigree.addedEP), 
            sanitise(message.pedigree.updated), sanitise(message.pedigree.updatedEP), sanitise(message.pedigree.deprecated), sanitise(message.pedigree.deprecatedEP)))

            for field in orchestration.message_fields(message):
                initialiser = '''            crocofix::dictionary::message_field(field::{}(), dictionary::presence::{}, {}), // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)\n'''.format(field.field.name, field.presence, field.depth)
                file.write(initialiser)

            file.write('''
         }
    )
{
}

''')

        file.write('''}

const crocofix::dictionary::message_collection& messages() noexcept
{
    static crocofix::dictionary::message_collection messages = {
''')

        for message in orchestration.messages.values():
            file.write('        message::{}(),\n'.format(message.name))

        file.write('''    };

    return messages; 
}
}''')
