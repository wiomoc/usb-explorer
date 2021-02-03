#include "MainFrame.hpp"
#include "DeviceListPanel.hpp"

#include <wx/splitter.h>

enum
{
    ID_DEVICE_LIST = 102
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
        EVT_CLOSE(MainFrame::OnClose)
            EVT_COMMAND(ID_DEVICE_LIST, DEVICE_LIST_UPDATE, MainFrame::OnDevicesCountUpdate)
                EVT_COMMAND(ID_DEVICE_LIST, DEVICE_SELECTED, MainFrame::OnDeviceSelected)
                    wxEND_EVENT_TABLE()

                        MainFrame::MainFrame(App *app) : wxFrame(NULL, wxID_ANY, "USB Explorer", wxDefaultPosition, wxSize(700, 400)), app(app)
{
    CreateStatusBar();
    wxSplitterWindow *splitter = new wxSplitterWindow(this);

    DeviceListPanel *list = new DeviceListPanel(splitter, ID_DEVICE_LIST, app);
    detail = new DeviceDetailPanel(splitter, wxID_ANY, app);
    splitter->SplitHorizontally(list, detail);
}

void MainFrame::OnQuit(wxCommandEvent &event)
{
    Close(true);
}
void MainFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
    // TODO
    this->app->OnExit();
    //exit(0);
}

void MainFrame::OnDevicesCountUpdate(wxCommandEvent &event)
{
    static wxString noDevicesFoundStatusBarText = "No devices found!";
    static wxString oneDeviceFoundStatusBarText = "Found one device";
    static wxString defaultStatusBarText = "Found %d devices";

    int devicesCount = event.GetInt();
    if (devicesCount == 0)
    {
        SetStatusText(noDevicesFoundStatusBarText);
    }
    else if (devicesCount == 1)
    {
        SetStatusText(oneDeviceFoundStatusBarText);
    }
    else
    {
        SetStatusText(wxString::Format(defaultStatusBarText, devicesCount));
    }
}

void MainFrame::OnDeviceSelected(wxCommandEvent &event)
{
    DeviceClientData *clientData = (DeviceClientData *)event.GetClientObject();
    detail->setDevice(clientData->device);
}