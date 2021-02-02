#include "DeviceListPanel.hpp"
#include <algorithm>

enum
{
    ID_LIST = 102
};

wxBEGIN_EVENT_TABLE(DeviceListPanel, wxWindow)
    EVT_TREELIST_SELECTION_CHANGED(ID_LIST, DeviceListPanel::OnSelectionChanged)
        wxEND_EVENT_TABLE()

            DeviceListPanel::DeviceListPanel(wxWindow *parent, wxWindowID id, App *app) : wxPanel(parent, id)
{
    usbService = app->usbService;
    usbService->enumerateDevices();

    tree = new wxTreeListCtrl(this, ID_LIST, wxDefaultPosition);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(tree, wxEXPAND, wxEXPAND);
    SetSizer(sizer);
    tree->AppendColumn("Hierarchy");
    tree->AppendColumn("VID");
    tree->AppendColumn("PID");
    tree->AppendColumn("Address");
    tree->AppendColumn("Port");

    updateDevices();
    hotplugController = usbService->startHotPlugListener(GetEventHandler(), [=]() {
        tree->DeleteAllItems();
        updateDevices();
    });
}

DeviceListPanel::~DeviceListPanel()
{
    if (hotplugController)
    {
        hotplugController->stop();
        hotplugController = nullptr;
    }
}

wxDEFINE_EVENT(DEVICE_LIST_UPDATE, wxCommandEvent);

void DeviceListPanel::updateDevices()
{
    wxCommandEvent devicesCountUpdate(DEVICE_LIST_UPDATE, GetId());
    devicesCountUpdate.SetInt(usbService->devicesCount);
    devicesCountUpdate.SetEventObject(this);
    ProcessWindowEvent(devicesCountUpdate);

    if (!usbService->rootDevices.empty())
    {
        buildTree(tree->GetRootItem(), usbService->rootDevices);
    }
}

void DeviceListPanel::buildTree(wxTreeListItem item, std::vector<std::shared_ptr<USBDevice>> &devices)
{
    for (auto d = devices.begin(); d != devices.end(); d++)
    {
        std::shared_ptr<USBDevice> device = *d;
        std::string &name = device->name;
        if (name.empty())
        {
            name = "Unknown Device";
        }
        wxTreeListItem newItem = tree->AppendItem(item, name);
        tree->SetItemData(newItem, new DeviceClientData(device));
        tree->SetItemText(newItem, 1, wxString::Format("0x%04hx", device->descriptor.idVendor));
        tree->SetItemText(newItem, 2, wxString::Format("0x%04hx", device->descriptor.idProduct));
        tree->SetItemText(newItem, 3, std::to_string(device->address));
        tree->SetItemText(newItem, 4, std::to_string(device->port));

        tree->Expand(newItem);

        buildTree(newItem, device->childreen);
    }
}

wxDEFINE_EVENT(DEVICE_SELECTED, wxCommandEvent);

void DeviceListPanel::OnSelectionChanged(wxTreeListEvent &event)
{
    wxTreeListItem item = tree->GetSelection();
    if (!item.IsOk())
        return;
    DeviceClientData *clientData = (DeviceClientData *)tree->GetItemData(item);
    wxCommandEvent deviceSelectedEvent(DEVICE_SELECTED, GetId());
    deviceSelectedEvent.SetClientObject(clientData);
    deviceSelectedEvent.SetEventObject(this);
    ProcessWindowEvent(deviceSelectedEvent);
}