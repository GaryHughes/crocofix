#!/usr/local/bin/python3

def generate_version_fields(namespace, prefix, orchestration):
    header_filename = '{}fields.hpp'.format(prefix)
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

            file.write('''
            
};
            
''')

        trailer = \
'''
}
'''
        file.write(trailer)

    source_filename = '{}fields.cpp'.format(prefix)
    with open(source_filename, 'w') as file:
        header = \
'''#include "{}fields.hpp"

namespace
{{

'''.format(prefix)

        file.write(header)

        for field in orchestration.fields.values():

            file.write('''{}::{}()
:    crocofix::dictionary::version_field({}, "{}", "{}", "{}", "{}")
{{
}}

'''.format(field.name, field.name, field.id, field.name, field.type, field.added, field.synopsis.replace('\n', '')))

        trailer = \
'''
}
'''
        file.write(trailer)
