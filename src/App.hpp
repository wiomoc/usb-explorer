#pragma once

#include <wx/wx.h>
#include "usb/USBService.hpp"

class App : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
    int OnExit() wxOVERRIDE;
    USBService *usbService;

private:
    wxFrame *frame;
};