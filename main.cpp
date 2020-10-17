

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxSDLPanel.hpp"

/*******************************************************************
    MAIN FRAME CLASS
*******************************************************************/

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    void OnQuit(wxCommandEvent& event);
    void OnClose (wxCloseEvent &event);

private:
		wxStaticText *m_st;
		SDLPanel *m_pan;

    DECLARE_EVENT_TABLE()
};

enum
{
    // menu items
    Minimal_Quit = wxID_EXIT
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{

    // set the frame icon
    //SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
    SetStatusText("You see the GUI thread not blocking the rendering routine. It fills you with determination!");
#endif // wxUSE_STATUSBAR

    m_pan = new SDLPanel(this);
}

void MyFrame::OnQuit(wxCommandEvent &event)
{
    // true is to force the frame to close
    Close(true);
}

/*******************************************************************
    APPLICATION CLASS
*******************************************************************/

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{

    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("Minimal wxWidgets App");
    frame->SetSize(wxSize(640,480));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

IMPLEMENT_APP(MyApp);

