#ifndef WX_SDL_PANEL
#define WX_SDL_PANEL

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <SDL2/SDL.h>
#include <thread>

/********************************************************************
    SDL PANEL CLASS
*******************************************************************/

class ParamsForSDL {
public:
    ParamsForSDL(const void* window);
    bool IsRunning() const;
    void QuitSDL();
    const void* GetWindow() const;

private:
    bool isRunning;
    const void* window;
};

void sdlCore(ParamsForSDL const&);

class SDLPanel : public wxWindow
{
public:
    std::thread *renderThread;
    SDLPanel(wxWindow *parent);
    ~SDLPanel();

private:
    ParamsForSDL paramsForSDL;
};

#endif
