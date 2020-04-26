#!/usr/bin/python3

import os
import sys
from orchestra import *
from version_fields_generator import *
from version_messages_generator import *

def usage():
    print('{} NAMESPACE PREFIX PATH'.format(os.path.basename(sys.argv[0])))


if __name__ == '__main__':
    
    if len(sys.argv) != 4:
        usage()
        sys.exit(1)

    # TODO - validate these
    namespace = sys.argv[1]
    prefix = sys.argv[2] 
    filename = sys.argv[3]
    orchestration = Orchestration(filename)

    generate_version_fields(namespace, prefix, orchestration)
    generate_version_messages(namespace, prefix, orchestration)

