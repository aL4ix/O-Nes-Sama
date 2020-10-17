#include "wxSDLPanel.hpp"

#ifdef __WXGTK__
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#endif // __WXGTK__

/********************************************************************
    SDL PANEL CLASS
*******************************************************************/

ParamsForSDL::ParamsForSDL(const void* window) : isRunning(true)
{
#ifdef __WXGTK__

    GtkWidget* widget = (GtkWidget*)window;
    gtk_widget_realize(widget);
    GdkWindow* draw_window = gtk_widget_get_window(widget);
    Window Xwindow = GDK_WINDOW_XID(draw_window);
    this->window = (const void*)Xwindow;
#else
    this->window = window;
#endif // __WXGTK__
}

bool ParamsForSDL::IsRunning() const
{
    return isRunning;
}

void ParamsForSDL::QuitSDL()
{
    isRunning = false;
}

const void* ParamsForSDL::GetWindow() const
{
    return window;
}


SDLPanel::SDLPanel(wxWindow *parent) : wxWindow(parent, wxID_ANY), paramsForSDL(this->GetHandle())
{
    renderThread = new std::thread(sdlCore, std::ref(paramsForSDL));
}


SDLPanel::~SDLPanel()
{
    paramsForSDL.QuitSDL();
    renderThread->join();
    delete renderThread;
    SDL_Quit();
}

