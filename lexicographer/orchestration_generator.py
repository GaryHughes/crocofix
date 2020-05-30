#!/usr/bin/python3

from sanitise import *

def generate_orchestration(namespace, prefix, orchestration):
    sane_prefix = sanitise_for_include_guard(prefix)
    header_filename = '{}orchestration.hpp'.format(prefix)
    with open(header_filename, 'w') as file:
        header = \
'''#ifndef crocofix_libcrocofixdictionary_{}orchestration_hpp
#define crocofix_libcrocofixdictionary_{}orchestration_hpp

namespace {}
{{
'''.format(sane_prefix, sane_prefix, namespace)
        file.write(header)


        trailer = \
'''}

#endif
'''
        file.write(trailer)

    source_filename = '{}orchestration.cpp'.format(prefix)
    with open(source_filename, 'w') as file:
        header = \
'''#include "{}orchestration.hpp"

namespace {}
{{

'''.format(prefix, namespace)
        file.write(header)

        
        trailer = \
'''
}

'''
        file.write(trailer)