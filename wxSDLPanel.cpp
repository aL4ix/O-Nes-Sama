#include "wxSDLPanel.hpp"

/********************************************************************
    SDL PANEL CLASS
*******************************************************************/

ParamsForSDL::ParamsForSDL(const void* window) : isRunning(true)
{
    this->window = window;
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

