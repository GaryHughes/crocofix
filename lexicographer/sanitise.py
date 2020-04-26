#!/usr/local/bin/python3

def sanitise(string):
    return string.replace('\n', '').replace('"', '\'')