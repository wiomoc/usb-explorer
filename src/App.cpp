#include "App.hpp"
#include "views/MainFrame.hpp"
#include <stdio.h>

#include "config.h"


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

libusb_context *init_libusb_ctx()
{

    libusb_context *usb_ctx;
    int status = libusb_init(&usb_ctx);
    libusb_set_log_cb(usb_ctx, usb_logger, LIBUSB_LOG_CB_CONTEXT);
    //libusb_set_option(usb_ctx, LIBUSB_OPTION_USE_USBDK, 1);
    //  libusb_set_option(usb_ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);

    if (status)
    {
        printf("libusb init failed: %d\n", status);
        exit(-1);
    }

    return usb_ctx;
}

bool App::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    printf("Version: %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    this->usb_ctx = init_libusb_ctx();
    MainFrame *frame = new MainFrame(this);
    //this->frame = frame;
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    frame->SetMenuBar(menuBar);

    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

int App::OnExit()
{
    printf("exiting\n");

    wxApp::OnExit();
    if (this->usb_ctx)
        libusb_exit(this->usb_ctx);
    return true;
}