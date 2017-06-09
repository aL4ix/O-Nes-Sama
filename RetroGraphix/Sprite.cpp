#include "Sprite.hpp"

Sprite::Sprite()
{
    texture = NULL;
    MoveTo(0, 0);
    SetWidth(0);
    SetHeight(0);
    UnFlip();
}

Sprite::~Sprite()
{
}
/*
void Sprite::SetTexture(const Texture& Texture)
{
    SetTextureAndDontUpdateSpriteSize(Texture);
    SetWidth(Texture.GetWidth());
    SetHeight(Texture.GetHeight());
}

void Sprite::SetTextureAndDontUpdateSpriteSize(const Texture& Texture)
{
    texture = &Texture;
}

const Texture* Sprite::GetTexture() const
{
    return texture;
}

void Sprite::Draw() const
{
    //Platform-Dependant
    if(texture)
        if(SDL_RenderCopyEx(sdlRenderer, texture->texture, NULL, &sdlRect, 0., NULL, sdlFlip) < 0)
            printf("Failed to render sprite: %s\n", SDL_GetError());
}

void Sprite::MoveX(const int X)
{
    //Platform-Dependant
    sdlRect.x = X;
}

void Sprite::MoveY(const int Y)
{
    //Platform-Dependant
    sdlRect.y = Y;
}

void Sprite::MoveTo(const int X, const int Y)
{
    MoveX(X);
    MoveY(Y);
}

int Sprite::GetX() const
{
    //Platform-Dependant
    return sdlRect.x;
}

int Sprite::GetY() const
{
    //Platform-Dependant
    return sdlRect.y;
}


void Sprite::SetWidth(const int Width)
{
    //Platform-Dependant
    sdlRect.w = Width;
}

void Sprite::SetHeight(const int Height)
{
    //Platform-Dependant
    sdlRect.h = Height;
}

void Sprite::FlipX()
{
    //Platform-Dependant
    sdlFlip = SDL_FLIP_HORIZONTAL;
}

void Sprite::FlipY()
{
    //Platform-Dependant
    sdlFlip = SDL_FLIP_VERTICAL;
}

void Sprite::FlipXY()
{
    //Platform-Dependant
    sdlFlip = SDL_RendererFlip(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
}

void Sprite::UnFlip()
{
    //Platform-Dependant
    sdlFlip = SDL_FLIP_NONE;
}
*/
