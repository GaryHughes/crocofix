#!/usr/bin/env python3

import argparse
import os
import sys
sys.path.append("..")
import xml.etree.ElementTree as ET
import fixorchestra.orchestration as orc
import fixrepository.repository as rep

def indent(elem, level=0):
    i = "\n" + level*"  "
    j = "\n" + (level-1)*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for subelem in elem:
            indent(subelem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = j
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = j
    return elem

def build_references(componentID):
    references = []
    for content in repository.msg_contents[source.componentID]:
        if content.reqd == '1':
            presence = 'required'
        else:
            presence = 'optional'
        try:
            field = repository.fields_by_tag[int(content.tagText)]
            references.append(orc.Reference(field.id, None, content.componentID, presence, content.description, content.pedigree))
        except ValueError:
            try:
                component = repository.components[content.tagText]
                references.append(orc.Reference(None, None, component.componentID, presence, content.description, content.pedigree))
            except KeyError:
                # TODO
                print('UNKNOWN REFERENCE ' + str(content))
                pass
    return references


if __name__ == '__main__':
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--repository', required=True, metavar='directory', help='A directory containing a repository to load e.g. fix_repository_2010_edition_20200402/FIX.4.4/Base')

    args = parser.parse_args()

    repository = rep.Repository(args.repository)
    repository.fix_known_errors()

    orchestration = orc.Orchestration()

    # version
    orchestration.version = repository.version

    # data types
    for source in repository.data_types.values():
        target = orc.DataType(source.name, source.base_type, source.description, source.pedigree)
        orchestration.data_types[target.name] = target
    # code sets
    for source in repository.fields_by_tag.values():
        try:
            enum = repository.enums[source.id]
            codes = [orc.Code(value.id * 1000 + index, value.symbolic_name, value.value, value.description, value.pedigree) for index, value in enumerate(enum, start=1)]
            target = orc.CodeSet(source.id, source.name + 'CodeSet', source.type, source.description, source.pedigree, codes)
            orchestration.code_sets[target.name] = target
        except KeyError:
            pass

    # fields
    for source in repository.fields_by_tag.values():
        type = source.type
        if source.id in repository.enums:
            type = source.name + 'CodeSet'
        target = orc.Field(source.id, source.name, type, source.description, source.pedigree)
        orchestration.fields_by_tag[target.id] = target
        orchestration.fields_by_name[target.name] = target

    # groups


    # components
    for source in repository.components.values():
        references = build_references(source.componentID)
        target = orc.Component(source.componentID, source.name, source.categoryId, source.description, source.pedigree, references)
        orchestration.components[target.id] = target

    # messages
    for source in repository.messages_by_msg_type.values():
        references = build_references(source.componentID)
        target = orc.Message(source.componentID, source.name, source.msgType, source.categoryID, source.description, source.pedigree, references)
        orchestration.messages_by_msg_type[target.msg_type] = target
        orchestration.messages_by_name[target.name] = target
   
    xml = orchestration.to_xml()

    print('<?xml version="1.0" encoding="UTF-8"?>')
    ET.dump(indent(xml))
