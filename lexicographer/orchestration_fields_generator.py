#!/usr/bin/python3

from sanitise import *

def generate_orchestration_fields(namespace, prefix, orchestration):
    sorted_fields = sorted(orchestration.fields_by_tag.values(), key=lambda x: int(x.id))
    sane_prefix = sanitise_for_include_guard(prefix)
    header_filename = '{}fields.hpp'.format(prefix)
    with open(header_filename, 'w') as file:
        header = \
'''#ifndef crocofix_libcrocofixdictionary_{}fields_hpp
#define crocofix_libcrocofixdictionary_{}fields_hpp

#include <libcrocofixdictionary/orchestration_field_collection.hpp>
#include <libcrocofixdictionary/orchestration_field.hpp>
#include <libcrocofixdictionary/field_value.hpp>

namespace {}
{{
namespace field
{{ 

'''.format(sane_prefix, sane_prefix, namespace)
        file.write(header)
        for field in sorted_fields:
            file.write('''class {} : public crocofix::dictionary::orchestration_field
{{
public:

    constexpr static const int Tag = {};

'''.format(field.name, field.id))

            file.write('    {}();\n'.format(field.name))

            # static constexpr crocofix::dictionary::field_value Buy = crocofix::dictionary::field_value("Buy", "1");
            try:
                code_set = orchestration.code_sets[field.type]
                file.write('\n')
                for code in code_set.codes:
                    name = code.name
                    if name == field.name:
                        print('{}.{} would result in a class member having the same name as the class which is invalid C++, renaming to {}.{}_'.format(field.name, code.name, field.name, code.name))
                        name = name + '_'
                    if name in ('char', 'int', 'float', 'double'):
                        before = name
                        name = name[0].upper() + name[1:]
                        print('renaming field value from {} to {} which is a builtin C++ typename'.format(before, name))             
                    file.write('    static constexpr crocofix::dictionary::field_value {} = crocofix::dictionary::field_value("{}", "{}");\n'.format(name, name, code.value)) 
            except KeyError:
                # TODO - maybe check that its an expected built in type
                pass

            file.write('''
};

''')

        trailer = \
'''}

const crocofix::dictionary::orchestration_field_collection& fields() noexcept;

}

#endif
'''
        file.write(trailer)

    source_filename = '{}fields.cpp'.format(prefix)
    with open(source_filename, 'w') as file:
        header = \
'''#include "{}fields.hpp"

namespace {}
{{
namespace field
{{

'''.format(prefix, namespace)

        file.write(header)

        for field in sorted_fields:
            try:
                data_type = orchestration.data_types[field.type]
            except:
                data_type = orchestration.code_sets[field.type]
            if data_type.name == 'data':
                is_data = 'true'
            else:
                is_data = 'false'
            file.write('''{}::{}()
: crocofix::dictionary::orchestration_field(
    {},
    {},
    "{}", 
    "{}", 
    "{}", 
    crocofix::dictionary::pedigree("{}", "{}", "{}", "{}", "{}", "{}")'''.format(field.name, field.name, field.id, is_data, field.name, field.type, sanitise(field.synopsis), 
                sanitise(field.pedigree.added), sanitise(field.pedigree.addedEP), sanitise(field.pedigree.updated), 
                sanitise(field.pedigree.updatedEP), sanitise(field.pedigree.deprecated), sanitise(field.pedigree.deprecatedEP)))

            try:
                code_set = orchestration.code_sets[field.type]
                if len(code_set.codes) > 0:
                    file.write(",\n    {\n")
                    for code in code_set.codes:
                        name = code.name
                        if name == field.name:
                            print('{}.{} would result in a class member having the same name as the class which is invalid C++, renaming to {}.{}_'.format(field.name, code.name, field.name, code.name))
                            name = name + '_'  
                        if name in ('char', 'int', 'float', 'double'):
                            before = name
                            name = name[0].upper() + name[1:]
                            print('renaming field value from {} to {} which is a builtin C++ typename'.format(before, name))     
                        file.write("        {},\n".format(name))
                    file.write("    }") 
            except KeyError:
                pass  

            file.write(''')
{
}

''')

        file.write('''}

const crocofix::dictionary::orchestration_field_collection& fields() noexcept
{
''')
        file.write('''
    static crocofix::dictionary::orchestration_field_collection fields({\n        ''')
   
        index = 0
        offset = -1
        for field in sorted_fields:
            while index < int(field.id):
                file.write("0,")
                index += 1
                offset += 1
                if index % 20 == 0:
                    file.write('\n         ')
            file.write("{},".format(index - offset))
            index += 1
            if index % 20 == 0:
                file.write('\n        ')
    
        file.write('''
        }, {
        dictionary::orchestration_field(0, false, "", "", "", crocofix::dictionary::pedigree("", "", "", "", "", "")),
''')

        index = 1
        for field in sorted_fields:
            file.write('        field::{}(),\n'.format(field.name))
            if index > 3000:
                break
            index += 1

        file.write('''    });

    return fields; 
}''')

        trailer = \
'''
}

'''
        file.write(trailer)
