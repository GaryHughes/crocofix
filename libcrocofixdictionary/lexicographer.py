#!/usr/local/bin/python3

import os
import sys
from orchestra import *

def usage():
    print('{} PATH'.format(os.path.basename(sys.argv[0])))

if __name__ == '__main__':
    
    if len(sys.argv) != 2:
        usage()
        sys.exit(1)

    path = sys.argv[1]
    orchestra = Orchestra()
    orchestra.load_orchestration(path)

