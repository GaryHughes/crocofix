#!/usr/bin/python3

from sanitise import *

def generate_orchestration(namespace, module, partition):
    header_filename = '{}_orchestration.cppm'.format(partition)
    with open(header_filename, 'w') as file:
        header = \
'''module;

export module {}:{}_orchestration;

import :orchestration;
import :{}_messages;
import :{}_fields;

export namespace {}
{{
'''.format(module, partition, partition, partition, namespace)
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
'''
        file.write(trailer)
