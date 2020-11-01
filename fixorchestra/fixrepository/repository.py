#!/usr/bin/env python3

import argparse
import xml.etree.ElementTree as ET
import os
import sys


class Pedigree:

    def __init__(self, added, addedEP, updated, updatedEP, deprecated, deprecatedEP):
        self.added = added
        self.addedEP = addedEP
        self.updated = updated
        self.updatedEP = updatedEP
        self.deprecated = deprecated
        self.deprecatedEP = deprecatedEP

    def __str__(self):
        buffer = ''
        if self.added:
            buffer += 'added=' + self.added
        if self.addedEP:
            if len(buffer) > 0:
                buffer += ', '
            buffer += 'addedEP=' + self.addedEP
        if self.updated:
            if len(buffer) > 0:
                buffer += ', '
            buffer += 'updated=' + self.updated
        if self.updatedEP:
            if len(buffer) > 0:
                buffer += ', '
            buffer += 'updatedEP=' + self.updatedEP
        if self.deprecated:
            if len(buffer) > 0:
                buffer += ', '
            buffer += 'deprecated=' + self.deprecated
        if self.deprecatedEP:
            if len(buffer) > 0:
                buffer += ', '
            buffer += 'deprecatedEP=' + self.deprecatedEP
        return '(' + buffer + ')'


class DataType:

    def __init__(self, name, base_type, description, pedigree):
        self.name = name
        self.base_type = base_type
        self.description = description
        self.synopsis = description
        self.pedigree = pedigree

class Enum:
    # This class needs to be kept in sync with orchestra.Code because fixaudit.py stores 
    # instances of these classes in Sets. Specifically both implementations have to be hashable 
    # and they have to be hashing the same thing.
    def __init__(self, id, value, symbolic_name, description, pedigree):
        self.id = id
        self.value = value
        self.symbolic_name = symbolic_name
        self.description = description
        self.pedigree = pedigree

    def __hash__(self):
        return hash(self.value)

    def __eq__(self, rhs):
        return self.value == rhs.value


class Field:
    # This class needs to be kept in sync with orchestra.Field because fixaudit.py stores 
    # nstances of these classes in Sets. Specifically both implementations have to be hashable 
    # and they have to be hashing the same thing.
    def __init__(self, id, name, type, description, pedigree):
        self.id = id
        self.name = name
        self.type = type
        self.description = description
        self.pedigree = pedigree
    
    def __hash__(self):
        return hash(self.id)

    def __eq__(self, rhs):
        return self.id == rhs.id

class Component:

    def __init__(self, componentID, componentType, categoryID, name, description, pedigree):
        self.componentID = componentID
        self.componentType = componentType
        self.categoryId = categoryID
        self.name = name
        self.description = description
        self.pedigree = pedigree


class MsgContent:

    def __init__(self, componentID, tagText, indent, position, reqd, description, pedigree):
        self.componentID = componentID
        self.tagText = tagText
        self.indent = indent
        self.position = position
        self.reqd = reqd
        self.description = description
        self.pedigree = pedigree


class Message:

    def __init__(self, componentID, msgType, name, categoryID, sectionID, description, pedigree):
        self.componentID = componentID
        self.msgType = msgType
        self.name = name
        self.categoryID = categoryID
        self.sectionID = sectionID
        self.description = description
        self.pedigree = pedigree


class MessageField:

    def __init__(self, field, required, depth):
        self.field = field
        self.required = required
        self.depth = depth


class Repository:

    enums = {}                  # Enum.id -> [Enum]
    fields_by_tag = {}          # Field.id -> Field
    fields_by_name = {}         # Field.name.lower() -> Field
    data_types = {}             # DataType.name -> DataType
    components = {}             # Component.Name -> Component
    msg_contents = {}           # MsgContent.componentID -> [MsgContent]
    messages = []               # [Message]
    messages_by_msg_type = {}   # Message.msg_type -> Message
    messages_by_name = {}       # Message.name.lower() -> Message
    version = ''

    def __init__(self, directory):
        if not os.path.exists(directory):
            raise Exception("directory '{}' does not exist".format(directory))
        self.load_abbreviations(directory)
        self.load_categories(directory)
        self.load_components(directory)
        self.load_data_types(directory)
        self.load_enums(directory)
        self.load_fields(directory)
        self.load_messages(directory)
        self.load_msg_contents(directory)
        self.load_sections(directory)

    def extract_pedigree(self, element):
        return Pedigree(
            element.get('added'),
            element.get('addedEP'),
            element.get('updated'),
            element.get('updatedEP'),
            element.get('deprecated'),
            element.get('deprecatedEP')
        )

    def load_components(self, directory):
        # <Component added="FIX.4.0">
        #     <ComponentID>1002</ComponentID>
        #     <ComponentType>Block</ComponentType>
        #     <CategoryID>Session</CategoryID>
        #     <Name>StandardTrailer</Name>
        #     <NotReqXML>1</NotReqXML>		
        #     <Description>The standard FIX message trailer</Description>
	    # </Component>
        filename = os.path.join(directory, 'Components.xml')
        if not os.path.exists(filename):
            raise Exception("directory '{}' does not contain a Components.xml".format(directory))
        componentsElement = ET.parse(filename).getroot()
        for componentElement in componentsElement.findall('Component'):
            description = componentElement.find('Description')
            component = Component(
                componentElement.find('ComponentID').text,
                componentElement.find('ComponentType').text,
                componentElement.find('CategoryID').text,
                componentElement.find('Name').text,
                description.text if description else '',
                self.extract_pedigree(componentElement)
            )
            self.components[component.name] = component

  
    def load_data_types(self, directory):
        # <Datatype added="FIX.4.2">
        #     <Name>Qty</Name>
        #     <BaseType>float</BaseType>
        #     <Description>float field (see definition of "float" above) capable of storing either a whole number (no decimal places) of "shares" or a decimal value containing decimal places for non-share quantity asset classes.</Description>
        # </Datatype>
        filename = os.path.join(directory, 'Datatypes.xml')
        if not os.path.exists(filename):
            raise Exception("directory '{}' does not contain a Datatypes.xml".format(directory))
        dataTypesElement = ET.parse(filename).getroot()
        for dataTypeElement in dataTypesElement.findall('Datatype'):
            baseType = dataTypeElement.find('BaseType')
            dataType = DataType(
                dataTypeElement.find('Name').text,
                baseType.text if baseType is not None else None,
                dataTypeElement.find('Description').text,
                self.extract_pedigree(dataTypeElement)
            )
            self.data_types[dataType.name] = dataType
        # This is present in all the files we parse so this is as good a place as any to get it.
        self.version = dataTypesElement.attrib['version']


    def load_enums(self, directory):
        # <Enum added="FIX.2.7">
        #     <Tag>4</Tag>
        #     <Value>S</Value>
        #     <SymbolicName>Sell</SymbolicName>
        #     <Description>Sell</Description>
    	# </Enum>
        filename = os.path.join(directory, 'Enums.xml')
        if not os.path.exists(filename):
            raise Exception("directory '{}' does not contain an Enums.xml".format(directory))
        enumsElement = ET.parse(filename).getroot()
        for enumElement in enumsElement.findall('Enum'):
            elaboration = enumElement.find('Elaboration')
            enum = Enum(
                int(enumElement.find('Tag').text),
                enumElement.find('Value').text,
                enumElement.find('SymbolicName').text,
                elaboration.text if elaboration is not None else enumElement.find('Description').text,
                self.extract_pedigree(enumElement)
            )
            try:
                self.enums[enum.id].append(enum)
            except KeyError:
                self.enums[enum.id] = [enum]
        

    def load_fields(self, directory):
        # <Field added="FIX.2.7">
        #     <Tag>4</Tag>
        #     <Name>AdvSide</Name>
        #     <Type>char</Type>
        #     <NotReqXML>1</NotReqXML>
        #     <Description>Broker's side of advertised trade</Description>
        # </Field>
        filename = os.path.join(directory, 'Fields.xml')
        if not os.path.exists(filename):
            raise Exception("directory '{}' does not contain a Fields.xml".format(directory))
        fieldsElement = ET.parse(filename).getroot()
        for fieldElement in fieldsElement.findall('Field'):
            field = Field(
                int(fieldElement.find('Tag').text),
                fieldElement.find('Name').text,
                fieldElement.find('Type').text,
                fieldElement.find('Description').text,
                self.extract_pedigree(fieldElement)
            )
            self.fields_by_tag[field.id] = field
            self.fields_by_name[field.name.lower()] = field

    def load_messages(self, directory):
        # <Message added="FIX.2.7">
        #     <ComponentID>2</ComponentID>
        #     <MsgType>1</MsgType>
        #     <Name>TestRequest</Name>
        #     <CategoryID>Session</CategoryID>
        #     <SectionID>Session</SectionID>
        #     <NotReqXML>1</NotReqXML>
        #     <Description>The test request message forces a heartbeat from the opposing application.</Description>
        # </Message>
        filename = "Messages.xml"
        path = os.path.join(directory, filename)
        if not os.path.exists(path):
            raise Exception("directory '{}' does not contain a {}".format(directory, filename))
        for messageElement in ET.parse(path).getroot().findall('Message'):
            message = Message(
                messageElement.find('ComponentID').text,
                messageElement.find('MsgType').text,
                messageElement.find('Name').text,
                messageElement.find('CategoryID').text,
                messageElement.find('SectionID').text,
                messageElement.find('Description').text,
                self.extract_pedigree(messageElement)
            )
            self.messages.append(message)
            self.messages_by_msg_type[message.msgType] = message
            self.messages_by_name[message.name.lower()] = message
      

    def load_msg_contents(self, directory):
        # <MsgContent added="FIX.2.7">
        #     <ComponentID>1</ComponentID>
        #     <TagText>StandardHeader</TagText>
        #     <Indent>0</Indent>
        #     <Position>1</Position>
        #     <Reqd>1</Reqd>
        #     <Description>MsgType = 0</Description>
        # </MsgContent>
        filename = os.path.join(directory, 'MsgContents.xml')
        if not os.path.exists(filename):
            raise Exception("directory '{}' does not contain a MsgContents.xml".format(directory))
        msgContentsElement = ET.parse(filename).getroot()
        for msgContentElement in msgContentsElement.findall('MsgContent'):
            description = msgContentElement.find('Description')
            msgContent = MsgContent(
                msgContentElement.find('ComponentID').text,
                msgContentElement.find('TagText').text,
                msgContentElement.find('Indent').text,
                msgContentElement.find('Position').text,
                msgContentElement.find('Reqd').text,
                description.text if description is not None else None,
                self.extract_pedigree(msgContentElement)
            )
            try:
                self.msg_contents[msgContent.componentID].append(msgContent)
            except KeyError:
                self.msg_contents[msgContent.componentID] = [msgContent]


    def load_sections(self, directory):
        pass

    def load_abbreviations(self, directory):
        pass

    def load_categories(self, directory):
        pass

    def extract_fields(self, componentID, depth):
        fields = []
        try:
            contents = self.msg_contents[componentID]
            for content in contents:
                try:
                    id = int(content.tagText)
                    field = self.fields_by_tag[id]
                    fields.append(MessageField(field, content.reqd, depth)) 
                except ValueError:
                    component = self.components[content.tagText]
                    fields += self.extract_fields(component.componentID, depth + 1)
        except KeyError:
            print("Can't find MsgContent with ComponentID = {}".format(componentID))
        return fields
    
    
    def message_fields(self, message):
        return self.extract_fields(message.componentID, 0)


    def field_values(self, field):
        try:
            return self.enums[field.id]
        except KeyError:
            return []   


    def fix_known_errors(self):
        #
        # This method will attempt to fix errors known to exist in the repositories published by fixprotocol.org. 
        # This is useful when generating code etc where the repository needs to be internally consistent.
        #
        # The publiched 4.0 specification contains fields with undefined data type Length
        # The publiched 4.1 specification contains fields with undefined data type Length
        # The published 4.2 specification contains fields with undefined data types Length and MultipleValueString
        # The published 4.3 specification contains fields with undefined data types MultipleValueString, Boolean, and DayOfMonth
        # The published 4.4 specification contains fields with undefined data type MultipleValueString
        #
        for field in self.fields_by_tag.values():
            try:
                data_type = self.data_types[field.type]
            except KeyError:
                if field.type == 'Length':
                    sys.stderr.writelines('Defining missing data type Length\n')
                    data_type = DataType('Length', 'int', 'int field representing the length in bytes. Value must be positive.', Pedigree('FIX.4.3', '', '', '', '', ''))
                elif field.type == 'MultipleValueString':
                    sys.stderr.writelines('Renaming data type MultipleValueString -> MultipleStringValue\n')
                    field.type = 'MultipleStringValue'
                    continue
                elif field.type == 'Boolean':
                    sys.stderr.writelines('Definiing missing data type Boolean\n')
                    data_type = DataType('Boolean', 'char', "char field containing one of two values: 'Y' = True/Yes 'N' = False/No", Pedigree('FIX.4.2', '', '', '', '', ''))
                elif field.type == 'DayOfMonth':
                    sys.stderr.writelines('Defining missing data type DayOfMonth\n')
                    data_type = DataType('DayOfMonth', 'int', 'int field representing a day during a particular monthy (values 1 to 31).', Pedigree('FIX.4.1', '', '', '', '', ''))
                else:
                    raise Exception("Found a undefined data type '{}' that I dont know how to fix".format(field.type))
                self.data_types[data_type.name] = data_type



def dump_field(repository, tag_or_name):
    try:
        field = repository.fields_by_tag[int(tag_or_name)]
    except (KeyError, ValueError):
        try:
            field = repository.fields_by_name[tag_or_name.lower()]
        except KeyError:
            print("Could not find a field with Tag or Name = '{}'".format(tag_or_name))
            return
    print(field.name + " {")
    print("    Id   = " + str(field.id))
    print("    Type  = " + field.type)
    print("    Pedigree = " + str(field.pedigree))
    print("    (" + field.description + ")")
    try:
        enums = repository.enums[field.id]
        print("    Values {")
        for enum in enums:
            print("        {} ({}, Pedigree = {}, {})".format(enum.value, enum.symbolic_name, str(enum.pedigree), enum.description))
        print("    }")
    except KeyError:
        pass
    print("}")


def dump_message_contents(repository, componentID, depth):
    padding = '    ' * depth
    try:
        contents = repository.msg_contents[componentID]
        for content in contents:
            try:
                id = int(content.tagText)
                field = repository.fields_by_tag[id]
                print(padding + '{} (Id = {}, Type = {}, Pedigree = {}, Required = {})'.format(field.name, field.id, field.type, str(field.pedigree), content.reqd))
            except ValueError:
                component = repository.components[content.tagText]
                print(padding + '{} {{'.format(component.name))
                dump_message_contents(repository, component.componentID, depth + 1)
                print(padding + '}')
    except KeyError:
        print("Can't find MsgContent with ComponentID = {}".format(componentID))
        return
    

def dump_message(repository, msg_type_or_name):
    try:
        message = repository.messages_by_msg_type[msg_type_or_name]
    except KeyError:
        try:
            message = repository.messages_by_name[msg_type_or_name.lower()]
        except KeyError:
            print("Could not find a message with MsgType or Name = '{}'".format(msg_type_or_name))
            return
    print(message.name + " {")
    print("    ComponentId = " + message.componentID)
    print("    MsgType = " + message.msgType)
    print("    CategoryID = " + message.categoryID)
    print("    SectionID = " + message.sectionID)
    print("    Pedigree = " + str(message.pedigree))
    print("    (" + message.description + ")")
    print("    MsgContents {")
    dump_message_contents(repository, message.componentID, 2)
    print("    }")
    print("}")


def list_messages(repository):
    for message in repository.messages_by_msg_type.values():
        print('{}\t{}'.format(message.msgType, message.name))


def list_fields(repository):
    for field in repository.fields_by_tag.values():
        print('{}\t{} ({})'.format(field.id, field.name, field.type))


def list_enumerated_fields(repository):
    for field in repository.fields_by_tag.values():
        try:
            enum = repository.enums[field.id]
            print('{}\t{} ({})'.format(field.id, field.name, field.type))
        except KeyError:
            pass


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--repository', required=True, metavar='directory', help='A directory containing a repository to load e.g. fix_repository_2010_edition_20200402/FIX.4.4/Base')
    parser.add_argument('--dump-field', required=False, metavar='(tag|name)', type=str, help='Display the definition of a field (name is not case sensitive)')
    parser.add_argument('--dump-message', required=False, metavar='(msgtype|name)', help='Display the definition of a message (name is not case sensitive')
    parser.add_argument('--list-messages', default=False, action='store_true', help='List all the messages in this repository')
    parser.add_argument('--list-fields', default=False, action='store_true', help='List all the fields in this repository')
    parser.add_argument('--list-enumerated-fields', default=False, action='store_true', help='List all fields with an enumerated value')

    args = parser.parse_args()

    repository = Repository(args.repository)

    if args.dump_field:
        dump_field(repository, args.dump_field)

    if args.dump_message:
        dump_message(repository, args.dump_message)

    if args.list_messages:
        list_messages(repository)

    if args.list_fields:
        list_fields(repository)

    if args.list_enumerated_fields:
        list_enumerated_fields(repository)