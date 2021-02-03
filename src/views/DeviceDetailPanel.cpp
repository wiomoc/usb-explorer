#include "DeviceDetailPanel.hpp"

extern "C"
{
#include "libusb.h"
}

wxBEGIN_EVENT_TABLE(DeviceDetailPanel, wxWindow)
    wxEND_EVENT_TABLE()

        DeviceDetailPanel::DeviceDetailPanel(wxWindow *parent, wxWindowID id, App *app) : wxPanel(parent, id), app(app)
{
    titleText = new wxStaticText(this, wxID_ANY, "");
}

DeviceDetailPanel::~DeviceDetailPanel()
{
}

void DeviceDetailPanel::setDevice(std::shared_ptr<USBDevice> device)
{
    titleText->SetLabel(device->name);
}