#!/usr/bin/env python3

# This script uses edtlib to generate IRQ properties for Tracealyzer

import argparse
from collections import defaultdict
import os
import pathlib
import re
import sys
from typing import Iterable, NoReturn, Optional
import pickle

dts_path = os.path.join(os.path.dirname(__file__), 
    '..', '..', '..', '..', '..', '..', '..', 
    'zephyr', 'scripts', 'dts')
sys.path.insert(0, dts_path)
sys.path.insert(0, os.path.join(dts_path, 'python-devicetree', 'src'))

import edtlib_logger
from devicetree import edtlib
from pprint import pprint

class IrqInfo:
    names: list[str]
    num: int
    prio: int
    disabled: bool
    
    def __init__(self, names: list[str], num: int, prio: int):
        self.names = names
        self.num   = num
        self.prio  = prio
        self.disabled = False
    
    @property
    def name(self):
        return self.names[0]

    @property
    def aliases(self):
        return self.names[1:]

def main():
    global xml_file

    args = parse_args()

    edtlib_logger.setup_edtlib_logging()

    with open(args.edt_pickle, 'rb') as f:
        edt = pickle.load(f)

    # Create the generated XML.
    with open(args.xml_out, "w", encoding="utf-8") as xml_file:
        write_top(edt)

        # read nodes, collect IRQ info
        irq_infos = list()
        for node in edt.nodes:
            irq_infos += read_node(node)
        
        # check for conflicts
        irq_info_dict = dict()  # int -> list(IrqInfo)
        for e in irq_infos:
            k = e.num
            if k in irq_info_dict:
                irq_info_dict[k].append(e)
            else:
                irq_info_dict[k] = [e]
        for k, v in irq_info_dict.items():
            if len(v) > 1:
                warn(f"IRQ numbering conflict: {k} (0x{k:x}) {list(map(lambda e: e.name, v))}")
            for e in v[1:]:
                e.disabled = True

        # sort by IRQ number
        irq_infos_sorted = sorted(irq_infos, key=lambda e: e.num)

        # write KnownObject elements
        for e in irq_infos_sorted:
            write_known_object(e)
            
        write_middle()
        
        # write Event elements
        for e in irq_infos_sorted:
            write_events(e)
        
        write_bottom()



def parse_args() -> argparse.Namespace:
    # Returns parsed command-line arguments

    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument("--xml-out", required=True,
                        help="path to write XML to")
    parser.add_argument("--edt-pickle",
                        help="path to read pickled edtlib.EDT object from")

    return parser.parse_args()


# a node can have 0 or more interrupt definitions
def read_node(node) -> list[IrqInfo]:
    r = list()
    if node.status == "disabled":
        return r
    labels = node._node.labels
    
    # hard-coded logic for interrupt mapping, copied from gen_defines.py
    def map_arm_gic_irq_type(irq, irq_num):
        # Maps ARM GIC IRQ (type)+(index) combo to linear IRQ number
        if "type" not in irq.data:
            err(f"Expected binding for {irq.controller!r} to have 'type' in "
                "interrupt-cells")
        irq_type = irq.data["type"]

        if irq_type == 0:  # GIC_SPI
            return irq_num + 32
        if irq_type == 1:  # GIC_PPI
            return irq_num + 16
        err(f"Invalid interrupt type specified for {irq!r}")

    for i, irq in enumerate(node.interrupts):        
        irq_names = list()
        for label in labels:
            name = label
            if irq.name:
                if irq.name.startswith(name):
                    name = irq.name
                else:
                    name += f"-{irq.name}"
            elif len(node.interrupts) > 1:
                name += f"#{i}"
            irq_names.append(name)
        irq_num = None
        irq_prio = None
        for cell_name, cell_value in irq.data.items():
            if cell_name == "irq":
                if "arm,gic" in irq.controller.compats:
                    cell_value = map_arm_gic_irq_type(irq, cell_value)
                irq_num = cell_value
            elif cell_name == "priority":
                irq_prio = cell_value
        
        e = IrqInfo(irq_names, irq_num, irq_prio)
        r.append(e)

    return r

def write_top(edt: edtlib.EDT) -> None:
    s = f"""<?xml version="1.0" encoding="utf-8"?>
<!-- generated from {edt.dts_path} -->
<PlatformExtension>
  <KnownObjects>
"""

    for scc in edt.scc_order:
        if len(scc) > 1:
            err("cycle in devicetree involving "
                + ", ".join(node.path for node in scc))

    print(s, file=xml_file)

def write_known_object(irq: IrqInfo) -> None:
    name_elem = f'<Name>{irq.name} ISR</Name>'
    id_elem   = f'<Id>ISR{irq.num}</Id>'
    prio_elem = f'<State0>{irq.prio}</State0>'

    if irq.disabled:
        s  = '<!--'
    else:
        s  = '    '
    s += '<KnownObject>'
    s += f'{name_elem:33}'
    s += f'{id_elem:15}'
    s += '<Type>Task</Type>'
    s += '<ObjectClass>ISR</ObjectClass>'
    s += prio_elem
    s += '</KnownObject>'
    if irq.disabled:
        s += '-->'
    aliases = irq.aliases
    if len(aliases) == 1:
        s += f'<!-- alternative name: {aliases[0]} -->'
    elif len(aliases) > 1:
        s += f'<!-- alternative names: {aliases} -->'
    print(s, file=xml_file)

def write_middle() -> None:
    s = """
 </KnownObjects>
  <EventCodes>
    <EventGroup name="ISR">
"""
    print(s, file=xml_file)

def write_events(irq: IrqInfo) -> None:
    num = irq.num
    knownObjectId = f"ISR{num}"
    write_event(2 * num,     "false", knownObjectId, irq.disabled)
    write_event(2 * num + 1, "true",  knownObjectId, irq.disabled)
    
def write_event(code: int, resumeInstance: str, knownObjectId: str, disabled: bool) -> None:
    code_attr = f'code="{code}"'
    ri_attr   = f'resumeInstance="{resumeInstance}"'
    koi_attr  = f'knownObjectId="{knownObjectId}"'

    if disabled:
        s = '  <!--'
    else:
        s = '      '
    s += f'<Event {code_attr:10} type="SwitchToInterrupt" {ri_attr:22}>'
    s += f'<Param index="-1" type="KnownObjectId" {koi_attr:22} />'
    s += '</Event>'
    if disabled:
        s += '-->'
    print(s, file=xml_file)
    
def write_bottom() -> None:
    s = """
    </EventGroup>
  </EventCodes>
</PlatformExtension>
"""
    print(s, file=xml_file)


def warn(s: str) -> None:
    print(s, file=sys.stderr)

def err(s: str) -> NoReturn:
    raise Exception(s)


if __name__ == "__main__":
    main()
