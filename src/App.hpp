#pragma once

#include <wx/wx.h>
extern "C"
{
#include "libusb.h"
}

class App : public wxApp
{
public:
    bool OnInit() wxOVERRIDE;
    int OnExit() wxOVERRIDE;
    libusb_context *usb_ctx;

private:
    wxFrame *frame;
};