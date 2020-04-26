#!/usr/bin/python3

from sanitise import *

def generate_version_messages(namespace, prefix, orchestration):
    header_filename = '{}version_messages.hpp'.format(prefix)
    with open(header_filename, 'w') as file:
        pass


    source_filename = '{}version_messages.cpp'.format(prefix)
    with open(source_filename, 'w') as file:
        pass
