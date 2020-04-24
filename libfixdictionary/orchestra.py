#!/usr/local/bin/python3

import xml.etree.ElementTree as ET

ns = { 
    'xs'     : 'http://www.w3.org/2001/XMLSchema',
    'functx' : 'http://www.functx.com',
    'fixr'   : 'http://fixprotocol.io/2016/fixrepository',
    'dc'     : 'http://purl.org/dc/elements/1.1/', 
    'xsi'    : 'http://www.w3.org/2001/XMLSchema-instance' 
}

class Code:

    def __init__(self, name, id, value, added):
        self.name = name
        self.id = id
        self.value = value
        self.added = added


class CodeSet:
 
    def __init__(self, name, id, type, codes):
        self.name = name
        self.id = id
        self.tyoe = type
        self.codes = codes


class Orchestration:

    code_sets = {}

    def __init__(self, filename):
        self.filename = filename
        tree = ET.parse(filename)
        repository = tree.getroot()
        self.load_code_sets(repository)

    def load_code_sets(self, repository):
        # <fixr:codeSets>
        #   <fixr:codeSet name="AdvSideCodeSet" id="4" type="char">
        #       <fixr:code name="Buy" id="4001" value="B" sort="1" added="FIX.2.7">
        #           <fixr:annotation>
        #               <fixr:documentation purpose="SYNOPSIS">
        #                   Buy
        #               </fixr:documentation>
        #           </fixr:annotation>
        #       </fixr:code>
        #       <fixr:code name="Sell" id="4002" value="S" sort="2" added="FIX.2.7">
        #           <fixr:annotation>
        #               <fixr:documentation purpose="SYNOPSIS">
        #                   Sell
        #               </fixr:documentation>
        #           </fixr:annotation>
        #       </fixr:code>
        codeSets = repository.find('fixr:codeSets', ns)
        for codeSetElement in codeSets.findall('fixr:codeSet', ns):
            codes = []
            for codeElement in codeSetElement.findall('fixr:code', ns):
                code = Code(
                    codeElement.attrib['name'],
                    codeElement.attrib['id'],
                    codeElement.attrib['value'],
                    codeElement.attrib['added']
                )
                codes.append(code)
            code_set = CodeSet(
                codeSetElement.attrib['name'],
                codeSetElement.attrib['id'],
                codeSetElement.attrib['type'],
                codes
            )
            self.code_sets[code_set.name] = code_set

    


class Orchestra:

    orchestrations = []

    def load_orchestration(self, filename):
        orchestration = Orchestration(filename)
        # TODO - check for existence etc
        for code_set in orchestration.code_sets.values():
            print(code_set.name)
            for code in code_set.codes:
                print('  {} {}'.format(code.name, code.value))
        self.orchestrations.append(orchestration)

       

    

