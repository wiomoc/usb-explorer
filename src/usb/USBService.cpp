#include "USBService.hpp"
#include "usb.ids.hpp"

#include <algorithm>

void usb_logger(libusb_context *ctx, enum libusb_log_level level, const char *str)
{
    const char *level_str;
    switch (level)
    {

    case LIBUSB_LOG_LEVEL_DEBUG:
        level_str = "DEBUG";
        break;
    case LIBUSB_LOG_LEVEL_INFO:
        level_str = "INFO";
        break;
    case LIBUSB_LOG_LEVEL_WARNING:
        level_str = "WARNING";
        break;
    case LIBUSB_LOG_LEVEL_ERROR:
        level_str = "ERROR";
        break;
    case LIBUSB_LOG_LEVEL_NONE:
    default:
        level_str = "NONE";
        break;
    }

    printf("libusb log %s: %s", level_str, str);
}

USBService::USBService()
{
    int status = libusb_init(&usb_ctx);
    libusb_set_log_cb(usb_ctx, usb_logger, LIBUSB_LOG_CB_CONTEXT);
    //libusb_set_option(usb_ctx, LIBUSB_OPTION_USE_USBDK, 1);
    //  libusb_set_option(usb_ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);

    if (status)
    {
        printf("libusb init failed: %d\n", status);
        exit(-1);
    }
}

USBService::~USBService()
{
    if (usb_ctx)
        libusb_exit(usb_ctx);
}

std::vector<std::shared_ptr<USBDevice>> *USBService::getOrCreateDevice(libusb_device *ref)
{
    if (!ref)
    {
        return &rootDevices;
    }
    else
    {
        libusb_device *parent = libusb_get_parent(ref);
        std::vector<std::shared_ptr<USBDevice>> *parent_list = getOrCreateDevice(parent);
        auto child = std::find_if(parent_list->begin(), parent_list->end(), [=](std::shared_ptr<USBDevice> &device) { return device->ref == ref; });
        if (child == parent_list->end())
        {
            uint8_t port = libusb_get_port_number(ref);
            std::shared_ptr<USBDevice> dev = std::make_shared<USBDevice>(ref, libusb_get_bus_number(ref), port, libusb_get_device_address(ref));
            libusb_get_device_descriptor(ref, &dev->descriptor);
            libusb_device_handle *device_handle;
            if (!libusb_open(ref, &device_handle))
            {
                uint8_t product_string_index = dev->descriptor.iProduct;
                if (product_string_index)
                {
                    char str[20];
                    libusb_get_string_descriptor_ascii(device_handle, product_string_index, (uint8_t *)str, sizeof(str));

                    dev->name += str;
                }
            }

            try
            {
                const Vendor &vendor = vendors.at(dev->descriptor.idVendor);
                dev->vendorName = vendor.name;
            }
            catch (std::out_of_range &e)
            {
            }

            // Sort by port number
            auto insertAt = find_if(parent_list->begin(), parent_list->end(), [=](std::shared_ptr<USBDevice> &device) { return device->port > port; });
            insertAt = parent_list->insert(insertAt, std::move(dev));
            return &((*insertAt)->childreen);
        }
        else
        {
            return &(*child)->childreen;
        }
    }
}

void USBService::deleteDevice(std::vector<std::shared_ptr<USBDevice>> *root, libusb_device *ref)
{
    root->erase(std::remove_if(root->begin(), root->end(), [=](std::shared_ptr<USBDevice> &device) {
                    deleteDevice(&device->childreen, ref);
                    return device->ref == ref;
                }),
                root->end());
}

void USBService::enumerateDevices()
{
    libusb_device **device_list;
    int device_list_count = libusb_get_device_list(usb_ctx, &device_list);

    if (device_list_count < 0)
    {
        printf("libusb get_device_list failed: %d\n", device_list_count);
        libusb_exit(usb_ctx);
        exit(-1);
    }

    for (int device_list_pos = 0; device_list_pos < device_list_count; device_list_pos++)
    {
        libusb_device *device = device_list[device_list_pos];
        getOrCreateDevice(device);
    }
    libusb_free_device_list(device_list, false);

    devicesCount = device_list_count;
}

std::shared_ptr<USBHotPlugController> USBService::startHotPlugListener(wxEvtHandler *eventHandler, std::function<void()> callback)
{

    if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
    {
        return USBHotPlugController::start(usb_ctx, eventHandler, [=](libusb_device *device, libusb_hotplug_event event) {
            if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
            {
                printf("new device arrived; address: %d\n", libusb_get_device_address(device));
                devicesCount++;
                getOrCreateDevice(device);
            }
            else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
            {
                printf("device left; address: %d\n", libusb_get_device_address(device));
                devicesCount--;
                deleteDevice(&rootDevices, device);
            }
            callback();
        });
    }
    else
    {
        return nullptr;
    }
}