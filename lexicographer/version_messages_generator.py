#!/usr/bin/python3

from sanitise import *

def generate_version_messages(namespace, prefix, orchestration):
    header_filename = '{}messages.hpp'.format(prefix)
    with open(header_filename, 'w') as file:
        header = \
'''#include <libcrocofixdictionary/message.hpp>
#include <libcrocofixdictionary/message_collection.hpp>

namespace {}
{{ 

'''.format(namespace)
        file.write(header)
    
        for message in orchestration.messages.values():
            file.write('''class {} : public crocofix::dictionary::message
{{
public:

'''.format(message.name))

            file.write('    {}();\n'.format(message.name))

            file.write('''
};

''')

        trailer = \
'''const crocofix::dictionary::message_collection& messages() noexcept;

}
'''
        file.write(trailer)


    source_filename = '{}messages.cpp'.format(prefix)
    with open(source_filename, 'w') as file:
        header = \
'''#include "{}messages.hpp"

namespace {}
{{

'''.format(prefix, namespace)

        file.write(header)

        for message in orchestration.messages.values():

            file.write('''{}::{}()
:    crocofix::dictionary::message("{}", "{}", "{}", "{}")
{{
}}

'''.format(message.name, message.name, message.name, message.msg_type, message.category, message.added))

        file.write('''const crocofix::dictionary::message_collection& messages() noexcept
{
    static crocofix::dictionary::message_collection messages = {
''')

        for message in orchestration.messages.values():
            file.write('        {}(),\n'.format(message.name))

        file.write('''    };

    return messages; 
}''')

        trailer = \
'''
}
'''
        file.write(trailer)
