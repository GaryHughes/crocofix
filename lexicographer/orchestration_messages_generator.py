#!/usr/bin/python3

from sanitise import *

def generate_orchestration_messages(namespace, module, partition, orchestration):
    header_filename = '{}_messages.cppm'.format(partition)
    with open(header_filename, 'w') as file:
        header = \
'''module;

export module {}:{}_messages;

import :message;
import :message_collection;

export namespace {}
{{
namespace message
{{ 

'''.format(module, partition, namespace)
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
'''
        file.write(trailer)


    source_filename = '{}_messages.cpp'.format(partition)
    with open(source_filename, 'w') as file:
        header = \
'''module;

module {};

namespace {}
{{
namespace message
{{

'''.format(module, namespace)

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
