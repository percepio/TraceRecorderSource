#!/usr/bin/env python3

"""
    Trace Recorder for Tracealyzer v4.8.0.hotfix1
    Copyright 2023 Percepio AB
    www.percepio.com

    SPDX-License-Identifier: Apache-2.0

    A script used during the post-build to generate an extension file to be used by Tracealyzer for syscall events.
"""

import argparse
import re
import xml.etree.ElementTree as ET
import os

parser = argparse.ArgumentParser(
    prog='tz_parse_syscalls',
    description='Parse the generated syscalls_list.h and generate the appropriate xml file for Tracealyzer'
)

parser.add_argument('build_dir', help='Path to the build directory')
parser.add_argument('zephyr_base', help='Path to the zephyr repository')

args = parser.parse_args()

if not os.path.isdir(args.build_dir):
    print("Build directory: {} doesn't exist, exiting...".format(args.build_dir))
    exit(1)

if not os.path.isdir(args.zephyr_base):
    print("Zephyr base directory: {} doesn't exist, exiting...".format(args.zephyr_base))
    exit(1)

syscall_list_h = "{}/zephyr/include/generated/syscall_list.h".format(args.build_dir)

if not os.path.isfile(syscall_list_h):
    print("Syscall list file: {} does not exist".format(syscall_list_h))

if not os.path.isfile("{}/VERSION".format(args.zephyr_base)):
    print("VERSION file: {}/VERSION does not exist, exiting...".format(args.zephyr_base))

# Parse the version number, needed for the output file name of the extension xml file
out_file: str
with open("{}/VERSION".format(args.zephyr_base), "r") as fh:
    major = None
    minor = None
    patch_level = None
    for line in fh:
        if matches := re.match('^VERSION_MAJOR\s*=\s*(\d+)$', line):
            major = int(matches.group(1))
        elif matches := re.match('^VERSION_MINOR\s*=\s*(\d+)$', line):
            minor = int(matches.group(1))
        elif matches := re.match('^PATCHLEVEL\s*=\s*(\d+)$', line):
            patch_level = int(matches.group(1))

    if not major or not minor or not patch_level:
        print("Invalid version specified: major: {}, minor: {}, patchlevel: {}, exiting...".format(major, minor, patch_level))
        exit(1)

    out_file = "syscalls-v{}.{}.{}.xml".format(major, minor, patch_level)

syscalls = []
syscall_exit_start_id = None
with open(syscall_list_h, 'r') as fh:
    for line in fh:
        matches = re.match('^#define\sK_SYSCALL_((\w|_)+)\s(\d+)$', line)
        if matches:
            syscall_name = matches.group(1)
            syscall_id = matches.group(3)
            syscalls.append({
                'id': int(syscall_id),
                'name': syscall_name.lower()
            })

            # Check if syscall K_SYSCALL_LIMIT
            if syscall_name == "LIMIT":
                syscall_exit_start_id = int(syscall_id) + 1

if not syscall_exit_start_id:
    print("No K_SYSCALL_LIMIT definition could be found, exiting...")
    exit(1)

root = ET.Element('PlatformExtension')

target_platform = ET.SubElement(root, 'TargetPlatform')
kernel_service_groups = ET.SubElement(target_platform, 'KernelServiceGroups')
kernel_service_group = ET.SubElement(kernel_service_groups, 'KernelServiceGroup', {'name': 'syscall'})

event_codes = ET.SubElement(root, 'EventCodes')
event_group = ET.SubElement(event_codes, 'EventGroup', {'name': 'syscalls'})

for syscall in syscalls:
    event_enter = ET.SubElement(
        event_group,
        'Event',
        {
            'code': '0x{:02X}'.format(syscall['id']),
            'service': 'syscall/{}'.format(syscall['name']),
            'type': 'KernelServiceEntry',
            'status': 'StatusOK'
        }
    )

    kernel_service_enter = ET.SubElement(
        kernel_service_group,
        'KernelService',
        {
            'name': "{}".format(syscall['name']),
            'parameters': 'None'
        }
    )

    event_exit = ET.SubElement(
        event_group,
        'Event',
        {
            'code': '0x{:02X}'.format(syscall['id'] + syscall_exit_start_id),
            'service': 'syscall/{}'.format(syscall['name']),
            'type': 'KernelServiceReturn',
            'status': 'StatusOK'
        }
    )

tree = ET.ElementTree(root)
ET.indent(tree)
tree.write("{}/{}".format(args.build_dir, out_file), xml_declaration=True, encoding='utf-8', method='xml')
