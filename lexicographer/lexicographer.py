#!/usr/bin/python3

import os
import sys
import argparse
from orchestra import *
from version_fields_generator import *
from version_messages_generator import *

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--namespace', required=True, help='The namespace to generate code in')
    parser.add_argument('--prefix', required=True, help='The prefix for the generated filenames')
    parser.add_argument('--orchestration', required=True, help='The orchestration filename to generate code for')

    args = parser.parse_args()

    orchestration = Orchestration(args.orchestration)

    generate_version_fields(args.namespace, args.prefix, orchestration)
    generate_version_messages(args.namespace, args.prefix, orchestration)

