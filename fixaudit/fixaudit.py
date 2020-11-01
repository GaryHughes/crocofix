#!/usr/bin/env python3

import argparse
import os
import sys
sys.path.append("..")
from fixorchestra.orchestration import *
from fixrepository.repository import *

def compare_repository_with_orchestration(repository, orchestration):

    print("Fields Orchestration = {} Repository = {}".format(len(orchestration.fields_by_tag), len(repository.fields_by_tag)))
    
    data_type_errors = []
    # TODO

    code_set_errors = []
    # TODO

    group_errors = []
    # TODO

    field_errors = []
    for r_id, r_field in repository.fields_by_tag.items():
        try:
            o_field = orchestration.fields_by_tag[r_id]
            if o_field.name != r_field.name:
                field_errors.append("field Id = {} has Name = '{}' in the repository and Name = '{}' in the orchestration".format(r_id, r_field.name, o_field.name))
            if r_field.pedigree != o_field.pedigree:
                field_errors.append('field Id = {} has pedigree {} in the repository and pedigree {} in the orchestration'.format(r_id, str(r_field.pedigree), str(o_field.pedigree)))
            o_values = frozenset(orchestration.field_values(o_field))
            r_values = frozenset(repository.field_values(r_field))
            if len(o_values) != len(r_values):
                field_errors.append("field Name = {} has {} values in the repository and {} values in the orchestration".format(r_field.name, len(r_values), len(o_values)))
            o_extras = o_values - r_values
            r_extras = r_values - o_values
            if len(o_extras) > 0:
                print("field Name = {} has the following values in the orchestration not in the corresponding repostory field {}".format(r_field.name, [value.name for value in o_extras]))
            if len(r_extras) > 0:
                print("field Name = {} has the following values in rhe repository not in the corresponding orchestration field {}".format(r_field.name, [value.symbolic_name for value in r_extras]))
   
        except KeyError:
            print("orchestration does not contain a field with Id = {}".format(r_id))
    
    if len(field_errors) == 0:
        print("All fields have the same name and pedigree in the repository and the orchestration")
    else:
        print("The following {} discrepancies were found".format(len(field_errors)))
        for error in field_errors:
            print(error)

    print("Messages Orchestration = {} Repository = {}".format(len(orchestration.messages), len(repository.messages)))

    message_errors = []
    for msg_type, o_message in orchestration.messages_by_msg_type.items():
        r_message = repository.messages_by_msg_type[msg_type]
        if o_message.name != r_message.name:
            message_errors.append("message MsgType = {} has Name = '{}' in the repository and Name = '{}' in the orchestration".format(msg_type, r_message.name, o_message.name))
        if o_message.pedigree != r_message.pedigree:
            message_errors.append('message MsgType = {} has pedigree {} in the repository and pedigree {} in the orchestration'.format(msg_type, str(r_message.pedigree), str(o_message.pedigree)))
        o_fields = frozenset([field.field for field in orchestration.message_fields(o_message)])
        r_fields = frozenset([field.field for field in repository.message_fields(r_message)])
        if len(o_fields) != len(r_fields):
            message_errors.append("message MsgType = {} has {} fields in the repository and {} fields in the orchestration".format(msg_type, len(r_fields), len(o_fields)))
        o_extras = o_fields - r_fields
        r_extras = r_fields - o_fields
        if len(o_extras) > 0:
            message_errors.append("message MsgType = {} orchestration has the following fields not in the corresponding repository message {}".format(msg_type, [ field.name for field in o_extras]))    
        if len(r_extras) > 0:
            message_errors.append("message MsgType = {} repository has the following fields not in the corresponding orchestration message {}".format(msg_type, [ field.name for field in r_extras]))    
      
    if len(message_errors) == 0:
        print("All messages have the same Name values in the repository and the orchestration")
    else:
        print("The following {} discrepancies were found".format(len(message_errors)))
        for error in message_errors:
            print(error)

    if len(field_errors) > 0 or len(message_errors) > 0:
        sys.exit(-1)


def validate_repository(repository):
    print('Validating repository')
    data_type_errors = []
    for field in repository.fields_by_tag.values():
        try:
            data_type = repository.data_types[field.type]
            if data_type.base_type != None:
                try:
                    base_type = repository.data_types[data_type.base_type]
                except KeyError:
                    data_type_errors.append('data type {} has base type {} but there is no such data type defined'.format(data_type.name, data_type.base_type))
        except KeyError:
            data_type_errors.append('field tag={} has type={} but there is no such data type defined'.format(field.id, field.type))
    if len(data_type_errors) == 0:
        print('All data types referenced by fields are defined')
    else:
        for error in data_type_errors:
            print(error)


def visit_orchestration_references(orchestration, references, context, field_errors, group_errors, component_errors):
    for reference in references:
        if reference.field_id:
            try:
                field = orchestration.fields_by_tag[reference.field_id]
            except:
                field_errors.append('{} references field id={} that is not defined'.format(context, reference.field_id))
        if reference.group_id:
            try:
                group = orchestration.groups[reference.group_id]
                visit_orchestration_references(orchestration, group.references, context + ' references group id={}'.format(group.id), field_errors, group_errors, component_errors)
            except KeyError:
                group_errors.append('{} that references group id={} that is not defined'.format(context, reference.group_id))
        if reference.component_id:
            try:
                component = orchestration.components[reference.component_id]
                visit_orchestration_references(orchestration, component.references, context + 'references component id={}'.format(component.id), field_errors, group_errors, component_errors)
            except KeyError:
                component_errors.append('{} that references component id={} that is not defined'.format(context, reference.component_id))
       


def validate_orchestration(orchestration):
    print('Validating orchestration')
    data_type_errors = []
    field_errors = []
    group_errors = []
    component_errors = []
    for field in orchestration.fields_by_tag.values():
        try:
            data_type = orchestration.data_types[field.type]
            if data_type.base_type != None:
                try:
                    base_type = orchestration.data_types[data_type.base_type]
                except KeyError:
                    data_type_errors.append('data type {} has base type {} but there is no such data type defined'.format(data_type.name, data_type.base_type))
        except KeyError:
            try:
                code_set = orchestration.code_sets[field.type]
            except KeyError:
                data_type_errors.append('field tag={} has type={} but there is no such data type or code set defined'.format(field.id, field.type))
    for message in orchestration.messages.values():
        visit_orchestration_references(orchestration, message.references, 'message MsgType={}'.format(message.msg_type), field_errors, group_errors, component_errors)
    
    if len(data_type_errors) == 0:
        print('All data types referenced by fields are defined')
    else:
        for error in data_type_errors:
            print(error)

    if len(field_errors) == 0:
        print('All referenced fields are defined')
    else:
        for error in field_errors:
            print(error)

    if len(group_errors) == 0:
        print('All referenced groups are defined')
    else:
        for error in group_errors:
            print(error)

    if len(component_errors) == 0:
        print('All referenced components are defined')
    else:
        for error in component_errors:
            print(error)


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    
    parser.add_argument('--orchestration', metavar='file', help='The orchestration to load')
    parser.add_argument('--repository', metavar='directory', help='A directory containing a repository to load e.g. fix_repository_2010_edition_20200402/FIX.4.4/Base')

    args = parser.parse_args()

    if args.orchestration and args.repository:
        orchestration = Orchestration(args.orchestration)
        validate_orchestration(orchestration)
        repository = Repository(args.repository)
        validate_repository(repository)
        compare_repository_with_orchestration(repository, orchestration)
    elif args.repository:
        repository = Repository(args.repository)
        validate_repository(repository)
    elif args.orchestration:
        orchestration = Orchestration(args.orchestration)
        validate_orchestration(orchestration)


    