#pragma once

#include <vector>
#include <wx/wx.h>
#include <wx/treelist.h>
#include "../App.hpp"
#include "DeviceDetailPanel.hpp"

class MainFrame : public wxFrame
{

public:
    MainFrame(App *app);

protected:
    void OnQuit(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnDevicesCountUpdate(wxCommandEvent &event);
    void OnDeviceSelected(wxCommandEvent &event);

private:
    App *app;
    DeviceDetailPanel *detail;
    wxDECLARE_EVENT_TABLE();
};