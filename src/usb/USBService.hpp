#pragma once

#include "USBDevice.hpp"
#include "USBHotPlugController.hpp"
#include <vector>

extern "C"
{
#include "libusb.h"
}

class USBService
{
public:
    USBService();
    ~USBService();
    void enumerateDevices();
    std::shared_ptr<USBHotPlugController> startHotPlugListener(wxEvtHandler *eventHandler, std::function<void()> callback);

    std::vector<std::shared_ptr<USBDevice>> rootDevices;
    int devicesCount;

private:
    struct libusb_context *usb_ctx;
    std::vector<std::shared_ptr<USBDevice>> *getOrCreateDevice(libusb_device *ref);
    static void deleteDevice(std::vector<std::shared_ptr<USBDevice>> *root, libusb_device *ref);
};