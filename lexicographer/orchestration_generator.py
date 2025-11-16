#!/usr/bin/python3

from sanitise import *

def generate_orchestration(namespace, module, partition, prefix, orchestration):
    sane_prefix = sanitise_for_include_guard(prefix)
    header_filename = '{}orchestration.cppm'.format(prefix)
    with open(header_filename, 'w') as file:
        header = \
'''module;

export module {}:{}_orchestration;

import :orchestration;

export namespace {}
{{
'''.format(module, partition, namespace)
        file.write(header)

        body = \
'''
class orchestration : public crocofix::dictionary::orchestration
{
public:
    
    orchestration()
    : crocofix::dictionary::orchestration(messages(), fields())
    {
    }

};

'''
        file.write(body)

        trailer = \
'''}
'''
        file.write(trailer)
