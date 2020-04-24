#!/usr/local/bin/python3

import xml.etree.ElementTree as ET

ns = { 
    'xs'     : 'http://www.w3.org/2001/XMLSchema',
    'functx' : 'http://www.functx.com',
    'fixr'   : 'http://fixprotocol.io/2016/fixrepository',
    'dc'     : 'http://purl.org/dc/elements/1.1/', 
    'xsi'    : 'http://www.w3.org/2001/XMLSchema-instance' 
}

class DataType:

    def __init__(self, name, baseType, added):
        self.name = name
        self.baseType = baseType
        self.added = added


class Code:

    def __init__(self, id, name, value, added):
        self.id = id
        self.name = name
        self.value = value
        self.added = added


class CodeSet:
 
    def __init__(self, id, name, type, codes):
        self.id = id
        self.name = name
        self.tyoe = type
        self.codes = codes

class Field:

    def __init__(self, id, name, type, added):
        self.id = id
        self.name = name
        self.type = type
        self.added = added


class Orchestration:

    data_types = {}
    code_sets = {}
    fields = {}
    components = {}
    messages = {}

    def __init__(self, filename):
        self.filename = filename
        tree = ET.parse(filename)
        repository = tree.getroot()
        self.load_data_types(repository)
        self.load_code_sets(repository)
        self.load_fields(repository)
        self.load_components(repository)
        self.load_messages(repository)


    def load_data_types(self, repository):
        # <fixr:datatypes>
        #   <fixr:datatype name="NumInGroup" baseType="int" added="FIX.4.3">
        #       <fixr:mappedDatatype standard="XML" base="xs:positiveInteger" builtin="0">
        #           <fixr:annotation>
        #               <fixr:documentation purpose="SYNOPSIS">
        #                   int field representing the number of entries in a repeating group. Value must be positive.
        #               </fixr:documentation>
        #           </fixr:annotation>
        #       </fixr:mappedDatatype>
        #       <fixr:annotation>
        #           <fixr:documentation purpose="SYNOPSIS">
        #               int field representing the number of entries in a repeating group. Value must be positive.
        #           </fixr:documentation>
        #       </fixr:annotation>
        #   </fixr:datatype>
        dataTypesElement = repository.find('fixr:datatypes', ns)
        for dataTypeElement in dataTypesElement.findall('fixr:datatype', ns):
            baseType = None
            try:
                baseType = dataTypeElement.attrib['baseType']
            except KeyError:
                pass
            dataType = DataType(
                dataTypeElement.attrib['name'],
                baseType,
                dataTypeElement.attrib['added']
            )
            self.data_types[dataType.name] = dataType
        

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
        codeSetsElement = repository.find('fixr:codeSets', ns)
        for codeSetElement in codeSetsElement.findall('fixr:codeSet', ns):
            codes = []
            for codeElement in codeSetElement.findall('fixr:code', ns):
                code = Code(
                    codeElement.attrib['id'],
                    codeElement.attrib['name'],
                    codeElement.attrib['value'],
                    codeElement.attrib['added']
                )
                codes.append(code)
            code_set = CodeSet(
                codeSetElement.attrib['id'],
                codeSetElement.attrib['name'],
                codeSetElement.attrib['type'],
                codes
            )
            self.code_sets[code_set.id] = code_set

    def load_fields(self, repository):
        # <fixr:fields>
		#   <fixr:field id="1" name="Account" type="String" added="FIX.2.7" abbrName="Acct">
		# 	    <fixr:annotation>
		# 		    <fixr:documentation purpose="SYNOPSIS">
        #               Account mnemonic as agreed between buy and sell sides, e.g. broker and institution or investor/intermediary and fund manager.
        #           </fixr:documentation>
		# 	    </fixr:annotation>
		#   </fixr:field>
        fieldsElement = repository.find('fixr:fields', ns)
        for fieldElement in fieldsElement.findall('fixr:field', ns):
            field = Field(
                fieldElement.attrib['id'],
                fieldElement.attrib['name'],
                fieldElement.attrib['type'],
                fieldElement.attrib['added']
            )
            self.fields[field.id] = field


    def load_components(self, repository):
        pass


    def load_messages(self, repository):
        pass


class Orchestra:

    orchestrations = []

    def load_orchestration(self, filename):
        orchestration = Orchestration(filename)
        # TODO - check for existence etc
        for code_set in orchestration.code_sets.values():
            print(code_set.name)
            for code in code_set.codes:
                print('  {} {}'.format(code.name, code.value))
        for field in orchestration.fields.values():
            print('{} {}'.format(field.name, field.type))
        for data_type in orchestration.data_types.values():
            print('{}'.format(data_type.name))
        self.orchestrations.append(orchestration)

       

    

