#!/usr/bin/python3

def sanitise(string):
    return string.replace('\n', '').replace('"', '\'').encode("ascii", errors="ignore").decode()