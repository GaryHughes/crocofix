#!/usr/bin/python3

import xml.etree.ElementTree as ET

ns = { 
    'xs'     : 'http://www.w3.org/2001/XMLSchema',
    'functx' : 'http://www.functx.com',
    'fixr'   : 'http://fixprotocol.io/2020/orchestra/repository',
    'dc'     : 'http://purl.org/dc/elements/1.1/', 
    'xsi'    : 'http://www.w3.org/2001/XMLSchema-instance' 
}

class DataType:

    def __init__(self, name, baseType, added, synopsis):
        self.name = name
        self.baseType = baseType
        self.added = added
        self.synopsis = synopsis


class Code:

    def __init__(self, id, name, value, added, synopsis):
        self.id = id
        self.name = name
        self.value = value
        self.added = added
        self.synopsis = synopsis


class CodeSet:
 
    def __init__(self, id, name, type, synopsis, codes):
        self.id = id
        self.name = name
        self.tyoe = type
        self.synopsis = synopsis
        self.codes = codes

class Field:

    def __init__(self, id, name, type, added, synopsis):
        self.id = id
        self.name = name
        self.type = type
        self.added = added
        self.synopsis = synopsis

class Reference:

    def __init__(self, field_id, group_id, component_id, presence, added):
        #if field_id and group_id:
        #    raise Exception('A Reference cannot have both a field_id and a group_id')
        self.field_id = field_id
        self.group_id = group_id
        self.component_id = component_id
        self.presence = presence
        self.added = added

class Component:

    def __init__(self, id, name, category, added, references):
        self.id = id
        self.name = name
        self.category = category
        self.added = added
        self.references = references

class Group:

    def __init__(self, id, name, added, category, references):
        self.id = id
        self.name = name
        self.added = added
        self.category = category
        self.references = references

class Message:

    def __init__(self, id, name, msg_type, category, added, synopsis, references):
        self.id = id
        self.name = name
        self.msg_type = msg_type
        self.category = category
        self.added = added
        self.synopsis = synopsis
        self.references = references
 
       
class Orchestration:

    data_types = {}
    code_sets = {}
    fields = {}
    components = {}
    groups = {}
    messages = {}

    def __init__(self, filename):
        self.filename = filename
        tree = ET.parse(filename)
        repository = tree.getroot()
        self.load_data_types(repository)
        self.load_code_sets(repository)
        self.load_fields(repository)
        self.load_components(repository)
        self.load_groups(repository)
        self.load_messages(repository)


    def references_to_fields(self, references, depth):
        result = []
        for reference in references:
            if reference.field_id:
                result.append((self.fields[reference.field_id], depth))
            elif reference.group_id:
                try:
                    group = self.groups[reference.group_id]
                    result = result + self.references_to_fields(group.references, depth + 1)
                except KeyError:
                    # Broken groupRef in FIX44
                    # https://github.com/FIXTradingCommunity/orchestrations/issues/11
                    pass
            elif reference.component_id:
                component = self.components[reference.component_id]
                result = result + self.references_to_fields(component.references, depth)
        return result


    def message_fields(self, message):
        return self.references_to_fields(message.references, 0)


    def extract_synopsis(self, element):
        # <element>
        #   <fixr:annotation>
        #       <fixr:documentation purpose="SYNOPSIS">
        #           int field representing the number of entries in a repeating group. Value must be positive.
        #       </fixr:documentation>
        #   </fixr:annotation>
        documentation = element.findall("./fixr:annotation/fixr:documentation/[@purpose='SYNOPSIS']", ns)
        if not documentation or len(documentation) == 0 or documentation[0].text is None:
            return ''
        return documentation[0].text.strip()
  

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
            dataType = DataType(
                dataTypeElement.get('name'),
                dataTypeElement.get('baseType'),
                dataTypeElement.get('added'),
                self.extract_synopsis(dataTypeElement)
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
                    codeElement.get('id'),
                    codeElement.get('name'),
                    codeElement.get('value'),
                    codeElement.get('added'),
                    self.extract_synopsis(codeElement)
                )
                codes.append(code)
            code_set = CodeSet(
                codeSetElement.get('id'),
                codeSetElement.get('name'),
                codeSetElement.get('type'),
                self.extract_synopsis(codeSetElement),
                codes
            )
            self.code_sets[code_set.name] = code_set

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
                fieldElement.get('id'),
                fieldElement.get('name'),
                fieldElement.get('type'),
                fieldElement.get('added'),
                self.extract_synopsis(fieldElement)
            )
            self.fields[field.id] = field

    def extract_references(self, element):
        references = []
        for refElement in list(element):
            if refElement.tag == '{{{}}}fieldRef'.format(ns['fixr']) or refElement.tag == '{{{}}}numInGroup'.format(ns['fixr']):
                reference = Reference(
                    refElement.get('id'),
                    None,
                    None,
                    refElement.get("presence"),
                    refElement.get('added')
                )
                references.append(reference)
            elif refElement.tag == '{{{}}}groupRef'.format(ns['fixr']):
                reference = Reference(
                    None,
                    refElement.get('id'),
                    None,
                    refElement.get("presence"),
                    refElement.get('added')
                )
                references.append(reference)
            elif refElement.tag == '{{{}}}componentRef'.format(ns['fixr']):
                reference = Reference(
                    None,
                    None,
                    refElement.get('id'),
                    refElement.get("presence"),
                    refElement.get('added')
                )
                references.append(reference)
            elif refElement.tag == '{{{}}}annotation'.format(ns['fixr']):
                # Don't care about these atleast for now
                pass
            else:
                raise Exception('Unexpected component element type {}'.format(refElement.tag))
        return references


    def load_components(self, repository):
        # <fixr:component name="DiscretionInstructions" id="1001" category="Common" added="FIX.4.4" abbrName="DiscInstr">
        #   <fixr:fieldRef id="388" added="FIX.4.4">
        #       <fixr:annotation>
        #           <fixr:documentation>
        #               What the discretionary price is related to (e.g. primary price, display price etc)
        #           </fixr:documentation>
        #       </fixr:annotation>
        #   </fixr:fieldRef>
        componentsElement = repository.find('fixr:components', ns)
        for componentElement in componentsElement.findall('fixr:component', ns):
            component = Component(
                componentElement.get('id'), 
                componentElement.get('name'), 
                componentElement.get('category'), 
                componentElement.get('added'), 
                self.extract_references(componentElement)
            )
            self.components[component.id] = component

    def load_groups(self, repository):
        # <fixr:groups>
        #   <fixr:group id="1007" added="FIX.4.4" name="LegStipulations" category="Common" abbrName="Stip">
        #       <fixr:numInGroup id="683"/>
        #       <fixr:fieldRef id="688" added="FIX.4.4">
        #           <fixr:annotation>
        #               <fixr:documentation>
        #                   Required if NoLegStipulations &gt;0
        #               </fixr:documentation>
        #           </fixr:annotation>
        #       </fixr:fieldRef>
        #       <fixr:fieldRef id="689" added="FIX.4.4">
        #           <fixr:annotation>
        #               <fixr:documentation/>
        #           </fixr:annotation>
        #       </fixr:fieldRef>
        #       <fixr:annotation>
        #           <fixr:documentation/>
        #       </fixr:annotation>
        #    </fixr:group>
        groupsElement = repository.find('fixr:groups', ns)
        for groupElement in groupsElement.findall('fixr:group', ns):
            group = Group(
                groupElement.get('id'),
                groupElement.get('name'),
                groupElement.get('added'),
                groupElement.get('category'),
                self.extract_references(groupElement)
            )
            self.groups[group.id] = group    


    def load_messages(self, repository):
        # <fixr:messages>
        #   <fixr:message name="Heartbeat" id="1" msgType="0" category="Session" added="FIX.2.7" abbrName="Heartbeat">
        #       <fixr:structure>
        #           <fixr:componentRef id="1024" presence="required" added="FIX.2.7">
        #               <fixr:annotation>
        #                   <fixr:documentation>
        #                       MsgType = 0
        #                   </fixr:documentation>
        #               </fixr:annotation>
        #           </fixr:componentRef>
        messagesElement = repository.find('fixr:messages', ns)
        for messageElement in messagesElement.findall('fixr:message', ns):
            structureElement = messageElement.find('fixr:structure', ns)
            message = Message(
                messageElement.get('id'),
                messageElement.get('name'),
                messageElement.get('msgType'),
                messageElement.get('category'),
                messageElement.get('added'),
                self.extract_synopsis(messageElement),
                self.extract_references(structureElement)
            )
            self.messages[message.id] = message


       

    

