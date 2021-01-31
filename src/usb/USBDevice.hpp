#pragma once
#include <stdint.h>
#include <vector>
#include <string>

extern "C"
{
#include "libusb.h"
}

class USBDevice
{
public:
    USBDevice(libusb_device *ref,
              uint8_t bus,
              uint8_t port,
              uint8_t address) : ref(ref), bus(bus), port(port), address(address) {}

    libusb_device *ref;
    uint8_t bus;
    uint8_t port;
    uint8_t address;
    struct libusb_device_descriptor descriptor;
    std::string name;

    std::vector<USBDevice> childreen;
};