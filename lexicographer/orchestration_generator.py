#!/usr/bin/python3

from sanitise import *

def generate_orchestration(namespace, prefix, orchestration):
    sane_prefix = sanitise_for_include_guard(prefix)
    header_filename = '{}orchestration.hpp'.format(prefix)
    with open(header_filename, 'w') as file:
        header = \
'''#ifndef crocofix_libcrocofixdictionary_{}orchestration_hpp
#define crocofix_libcrocofixdictionary_{}orchestration_hpp

#include <libcrocofixdictionary/orchestration.hpp>
#include "{}messages.hpp"
#include "{}fields.hpp"

namespace {}
{{
'''.format(sane_prefix, sane_prefix, sane_prefix, sane_prefix, namespace)
        file.write(header)

        body = \
'''
class orchestration : public crocofix::dictionary::orchestration
{{
public:
    
    orchestration()
    : crocofix::dictionary::orchestration({}::messages(), {}::fields())
    {{
    }}

}};

'''.format(namespace, namespace)

        file.write(body)

        trailer = \
'''}

#endif
'''
        file.write(trailer)
