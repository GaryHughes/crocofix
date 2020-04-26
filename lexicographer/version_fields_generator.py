#!/usr/local/bin/python3

from sanitise import *

def generate_version_fields(namespace, prefix, orchestration):
    header_filename = '{}version_fields.hpp'.format(prefix)
    with open(header_filename, 'w') as file:
        header = \
'''#include <libcrocofixdictionary/version_field.hpp>

namespace {}
{{ 

'''.format(namespace)
        file.write(header)
    
        for field in orchestration.fields.values():
            file.write('''class {} : public crocofix::dictionary::version_field
{{
public:

'''.format(field.name))

            file.write('    {}();'.format(field.name))

            # static constexpr crocofix::dictionary::field_value Buy = crocofix::dictionary::field_value("Buy", "1");
            

            file.write('''
            
};
            
''')

        trailer = \
'''
}
'''
        file.write(trailer)

    source_filename = '{}version_fields.cpp'.format(prefix)
    with open(source_filename, 'w') as file:
        header = \
'''#include "{}version_fields.hpp"

namespace {}
{{

'''.format(prefix, namespace)

        file.write(header)

        for field in orchestration.fields.values():

            file.write('''{}::{}()
:    crocofix::dictionary::version_field({}, "{}", "{}", "{}", "{}")
{{
}}

'''.format(field.name, field.name, field.id, field.name, field.type, field.added, sanitise(field.synopsis)))

        trailer = \
'''
}
'''
        file.write(trailer)
