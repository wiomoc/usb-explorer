#!/usr/bin/env python3
import sys
from collections import namedtuple
import re

in_filename = sys.argv[1]
out_filename = sys.argv[2]

VendorEntry = namedtuple('VendorEntry', 'name devices')

vendors = dict()

vendor_re = re.compile("^([0-9a-f]{4})\s\s(.+)$")
device_re = re.compile("^\s([0-9a-f]{4})\s\s(.+)$")

with open(in_filename) as f:
    current_vendor = None
    for line in f:
        if line.startswith("#"):
            continue
        vendor_match = vendor_re.match(line)
        if vendor_match:
            current_vendor = VendorEntry(
                name=vendor_match.group(2), devices=dict())
            vendors[vendor_match.group(1)] = current_vendor
            continue
        device_match = device_re.match(line)
        if device_match:
            current_vendor.devices[device_match.group(
                1)] = device_match.group(2)


output_header_1 = """
# include <map>
# include <string>
# include <stdint.h>

struct Vendor {
    std::string name;
    std::map<uint16_t, std::string> devices;
};
static std::map<uint16_t, Vendor> create_vendor_map()
    {
          std::map<uint16_t, Vendor> m;
"""
output_header_2 = """
          return m;
    }
const std::map<uint16_t, Vendor> vendors =  create_vendor_map();
"""


def escape(string):
    return string.replace('\\', '\\\\').replace('"', '\\"').replace('?', '\\?')


with open(out_filename, "w") as f:
    f.write(output_header_1)
    for i, (vid, vendor) in enumerate(vendors.items()):
        f.write(f'std::map<uint16_t, std::string> devices{i};\n')
        for pid, device_name in vendor.devices.items():
            f.write(f'devices{i}[0x{pid}] = "{escape(device_name)}";\n')
        f.write(
            f'm[0x{vid}] = {{.name = "{escape(vendor.name)}", .devices = devices{i}}};\n')
    f.write(output_header_2)
