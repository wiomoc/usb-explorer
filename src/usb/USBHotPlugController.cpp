#include "USBHotPlugController.hpp"
#include <thread>

int USBHotPlugController::usb_hotplug_callback_fn(libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data)
{
    USBHotPlugController *controller = (USBHotPlugController *)user_data;
    std::weak_ptr<USBHotPlugController> weak_controller = controller->weak_from_this();
    controller->event_handler->CallAfter([=]() {
        std::shared_ptr<USBHotPlugController> controller_shared = weak_controller.lock();
        if (controller_shared != nullptr && !controller_shared->stopped)
        {
            controller_shared->callback(device, event);
        }
    });
    return 0;
}

std::shared_ptr<USBHotPlugController>
USBHotPlugController::start(libusb_context *usb_ctx,
                            wxEvtHandler *event_handler,
                            std::function<void(libusb_device *device, libusb_hotplug_event event)> callback)
{
    std::shared_ptr<USBHotPlugController> controller = std::make_shared<USBHotPlugController>(usb_ctx, event_handler, callback);

    int status = libusb_hotplug_register_callback(usb_ctx,
                                                  LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                                  LIBUSB_HOTPLUG_NO_FLAGS,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  LIBUSB_HOTPLUG_MATCH_ANY,
                                                  USBHotPlugController::usb_hotplug_callback_fn,
                                                  // controller lives as long as the thread calling the cb
                                                  (void *)controller.get(),
                                                  &controller->usb_hp_cb_handle);
    if (status)
    {
        printf("Error during registration for hot plug events\n");
    }
    controller->stopped = false;
    new std::thread([=] {
        while (!controller->stopped)
            libusb_handle_events(usb_ctx);
    });

    return controller;
}

void USBHotPlugController::stop()
{
    this->stopped = true;
    libusb_hotplug_deregister_callback(this->usb_ctx, this->usb_hp_cb_handle);
    libusb_interrupt_event_handler(this->usb_ctx);
}