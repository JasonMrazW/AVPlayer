//
//  SDLActivity.cpp
//  AVProject
//
//  Created by bo on 2021/11/26.
//

#include "SDLImagePlayer.hpp"


// ============================================================================
// [Defines & Constants]
// ============================================================================
#define APPTITLE         "SDL Image Player by bo"

const int SCREEN_WIDTH     = 900;
const int SCREEN_HEIGHT    = 600;

SDLImagePlayer::SDLImagePlayer() :
    running(false)
{
}

SDLImagePlayer::~SDLImagePlayer()
{
    OnCleanup();
}

int SDLImagePlayer::OnInit()
{
    // Initialize the SDL library.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_FAILED;
    }
    
    window = SDL_CreateWindow(APPTITLE,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (window != NULL) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        texture = SDL_CreateTexture(renderer, imageParser->yuvFileData->format, SDL_TEXTUREACCESS_STREAMING, imageParser->yuvFileData->width, imageParser->yuvFileData->height);
    }
    
    // Success
    return APP_OK;
}

void SDLImagePlayer::OnCleanup()
{
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != NULL)    {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

int SDLImagePlayer::OnExecute(IImageParser*    parser)
{
    // Initialize application.
    imageParser = parser;
    int state = OnInit();
    if (state != APP_OK) {
        return state;
    }
    
    // Enter the SDL event loop.
    SDL_Event event;

    running = true;
    
    while (running)
    {
        while (SDL_PollEvent(&event)) {
            OnEvent(&event);
        }
        
        OnUpdate();
        OnRender();
    }
    
    return state;
}

void SDLImagePlayer::OnEvent(SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_QUIT:
            running = false;
            break;
            
        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        default:
            break;
    }
}

void SDLImagePlayer::OnUpdate()
{
    // Update your game logic here
}

void SDLImagePlayer::OnRender()
{
    SDL_UpdateTexture(texture, nullptr, imageParser->yuvFileData->data, imageParser->yuvFileData->pin);

    SDL_RenderClear(renderer);
    // Do your drawing here
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    
    SDL_RenderPresent(renderer);
}

void SDLImagePlayer::updateYUVFileData(YUVFileData &data) {
    imageParser->yuvFileData = &data;
    notifyGetVideoFrame();
}

void SDLImagePlayer::notifyGetVideoFrame() {
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
}

