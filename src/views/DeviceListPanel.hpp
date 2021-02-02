#pragma once

#include <wx/wx.h>
#include <wx/treelist.h>
#include "../App.hpp"
#include "../usb/USBDevice.hpp"
#include "../usb/USBService.hpp"
#include "../usb/USBHotPlugController.hpp"

class DeviceListPanel : public wxPanel
{

public:
    DeviceListPanel(wxWindow *parent, wxWindowID id, App *app);
    ~DeviceListPanel();

private:
    void updateDevices();
    void buildTree(wxTreeListItem item, std::vector<std::shared_ptr<USBDevice>> &devices);
    void OnSelectionChanged(wxTreeListEvent &event);

    wxTreeListCtrl *tree;
    USBService *usbService;
    std::shared_ptr<USBHotPlugController> hotplugController;
    wxDECLARE_EVENT_TABLE();
};

class DeviceClientData : public wxClientData
{
public:
    DeviceClientData(std::shared_ptr<USBDevice> device) : wxClientData(), device(device){};
    std::shared_ptr<USBDevice> device;
};

wxDECLARE_EVENT(DEVICE_LIST_UPDATE, wxCommandEvent);
wxDECLARE_EVENT(DEVICE_SELECTED, wxCommandEvent);