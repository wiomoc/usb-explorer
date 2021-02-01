#pragma once

#include <vector>
#include <wx/wx.h>
#include <wx/treelist.h>
#include "../App.hpp"
#include "../usb/USBDevice.hpp"
#include "../usb/USBHotPlugController.hpp"

class MainFrame : public wxFrame
{

public:
    MainFrame(App *app);

protected:
    void OnQuit(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);

private:
    void buildTree(wxTreeListCtrl *tree, wxTreeListItem item,  std::vector<USBDevice> &devices);
    void updateDevices(wxTreeListCtrl *tree);

    std::vector<USBDevice> rootDevices;
    int devicesCount;
    App *app;
    std::shared_ptr<USBHotPlugController> hotplugController;
    wxDECLARE_EVENT_TABLE();
};