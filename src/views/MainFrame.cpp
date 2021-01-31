#include "MainFrame.hpp"
#include <algorithm>

extern "C"
{
#include "libusb.h"
}


using namespace std;

static vector<USBDevice> *get_or_create_device(vector<USBDevice> *root, libusb_device *ref)
{

    if (!ref)
    {
        return root;
    }
    else
    {
        libusb_device *parent = libusb_get_parent(ref);
        vector<USBDevice> *parent_list = get_or_create_device(root, parent);
        auto child = find_if(parent_list->begin(), parent_list->end(), [=](USBDevice &device) { return device.ref == ref; });
        if (child == parent_list->end())
        {
            uint8_t port = libusb_get_port_number(ref);
            USBDevice dev = USBDevice(ref, libusb_get_bus_number(ref), port, libusb_get_device_address(ref));
            libusb_get_device_descriptor(ref, &dev.descriptor);
            libusb_device_handle *device_handle;
            if (!libusb_open(ref, &device_handle))
            {
                uint8_t product_string_index = dev.descriptor.iProduct;
                if (product_string_index)
                {
                    char str[20];
                    libusb_get_string_descriptor_ascii(device_handle, product_string_index, (uint8_t *)str, sizeof(str));

                    dev.name += str;
                }
            }
            // Sort by port number
            auto insertAt = find_if(parent_list->begin(), parent_list->end(), [=](USBDevice &device) { return device.port > port; });
            insertAt = parent_list->insert(insertAt, move(dev));
            return &(insertAt->childreen);
        }
        else
        {
            return &child->childreen;
        }
    }
}

static void delete_device(vector<USBDevice> *root, libusb_device *ref)
{
    root->erase(std::remove_if(root->begin(), root->end(), [=](USBDevice &device) {
                    delete_device(&device.childreen, ref);
                    return device.ref == ref;
                }),
                root->end());
}

vector<USBDevice> enumerate_devices(libusb_context *usb_ctx)
{
    libusb_device **device_list;
    int device_list_count = libusb_get_device_list(usb_ctx, &device_list);

    if (device_list_count < 0)
    {
        printf("libusb get_device_list failed: %d\n", device_list_count);
        libusb_exit(usb_ctx);
        exit(-1);
    }

    vector<USBDevice> root_devices;

    for (int device_list_pos = 0; device_list_pos < device_list_count; device_list_pos++)
    {
        libusb_device *device = device_list[device_list_pos];

        get_or_create_device(&root_devices, device);
    }

    return root_devices;
}



wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
        EVT_CLOSE(MainFrame::OnClose)
            wxEND_EVENT_TABLE()

                void MainFrame::OnQuit(wxCommandEvent &event)
{

    Close(true);
}
void MainFrame::OnClose(wxCloseEvent &event)
{
    if (this->hotplugController)
    {
        this->hotplugController->stop();
        this->hotplugController = nullptr;
    }
    Destroy();
    //this->app->OnExit();
    //exit(0);
}

void MainFrame::buildTree(wxTreeListCtrl *tree, wxTreeListItem item, vector<USBDevice> &devices)
{
    for (auto device = devices.begin(); device != devices.end(); device++)
    {
        string &name = device->name;
        if (name.empty())
        {
            name = "Unknown Device";
        }
        wxTreeListItem newItem = tree->AppendItem(item, name);
        tree->SetItemText(newItem, 1, wxString::Format("0x%04hx", device->descriptor.idVendor));
        tree->SetItemText(newItem, 2, wxString::Format("0x%04hx", device->descriptor.idProduct));
        tree->SetItemText(newItem, 3, to_string(device->address));
        tree->SetItemText(newItem, 4, to_string(device->port));

        tree->Expand(newItem);

        buildTree(tree, newItem, device->childreen);
    }
}

MainFrame::MainFrame(App *app) : wxFrame(NULL, wxID_ANY, "USB", wxDefaultPosition, wxSize(650, 400)), app(app)
{

    libusb_context *usb_ctx = this->app->usb_ctx;
    this->rootDevices = enumerate_devices(usb_ctx);
    wxTreeListCtrl *tree = new wxTreeListCtrl(this, wxID_ANY);
    tree->AppendColumn("Hierarchy");
    tree->AppendColumn("VID");
    tree->AppendColumn("PID");
    tree->AppendColumn("Address");
    tree->AppendColumn("Port");
    buildTree(tree, tree->GetRootItem(), rootDevices);
    if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
    {
        this->hotplugController = USBHotPlugController::start(usb_ctx, this->GetEventHandler(), [=](libusb_device *device, libusb_hotplug_event event) {
            printf("Event: %p \n", device);
            if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
            {
                get_or_create_device(&this->rootDevices, device);
            }
            else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
            {
                delete_device(&this->rootDevices, device);
            }
            // XXX
            tree->DeleteAllItems();
            this->buildTree(tree, tree->GetRootItem(), rootDevices);
        });
    }

    CreateStatusBar();
    SetStatusText("Welcome to USB Explorer!");
}