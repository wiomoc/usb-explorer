#include "App.hpp"
#include "views/MainFrame.hpp"
#include <stdio.h>

#include "config.h"

bool App::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    printf("Version: %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    usbService = new USBService();
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
    delete usbService;

    return true;
}