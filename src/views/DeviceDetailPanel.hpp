#pragma once

#include <wx/wx.h>
#include "../App.hpp"
#include "../usb/USBDevice.hpp"

class DeviceDetailPanel : public wxPanel
{

public:
    DeviceDetailPanel(wxWindow *parent, wxWindowID id, App *app);
    ~DeviceDetailPanel();
    void setDevice(USBDevice *device);

private:
    App *app;
    wxStaticText *titleText;
    wxDECLARE_EVENT_TABLE();
};
