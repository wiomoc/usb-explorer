#pragma once

#include <wx/wx.h>
#include <atomic>

extern "C"
{
#include "libusb.h"
}

class USBHotPlugController: public std::enable_shared_from_this<USBHotPlugController>
{
public:
   USBHotPlugController(libusb_context *usb_ctx,
                         wxEvtHandler *event_handler,
                         std::function<void(libusb_device *device, libusb_hotplug_event event)> callback) : usb_ctx(usb_ctx),
                                                                                                       event_handler(event_handler),
                                                                                                       callback(callback){};
    static std::shared_ptr<USBHotPlugController> start(
        libusb_context *usb_ctx,
        wxEvtHandler *event_handler,
        std::function<void(libusb_device *device, libusb_hotplug_event event)> callback);
    void stop();

private:
    static int usb_hotplug_callback_fn(libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data);
    libusb_context *usb_ctx;
    wxEvtHandler *event_handler;
    libusb_hotplug_callback_handle usb_hp_cb_handle;
    std::atomic_bool stopped;
    std::function<void(libusb_device *device, libusb_hotplug_event event)> callback;
};