//
// Created by bo on 2021/12/20.
//

#include "header/AV_Render_Video.h"

bool AV_Render_Video::init() {
    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window != nullptr) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
    return true;
}

bool AV_Render_Video::onUpdate() {
    return false;
}

bool AV_Render_Video::onRender() {
    if (current_yuv_data == nullptr) return false;

    SDL_UpdateTexture(texture, nullptr, current_yuv_data, current_pin);

    SDL_RenderClear(renderer);
    // Do your drawing here
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);
    return true;
}

bool AV_Render_Video::onStop() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
    return true;
}

bool AV_Render_Video::onDestroy() {
    if (current_yuv_data != nullptr) {
        delete current_yuv_data;
    }

    return true;
}

bool AV_Render_Video::openVideoDevice(uint8_t *data, int width, int height, Uint32 format, int pin) {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
    current_yuv_data = data;
    current_pin = pin;
    texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, width, height);
    return true;
}

void AV_Render_Video::setBuffer(ThreadSafeQueue<YUVFileData> *queue) {
    yuv_fileQueue = queue;
}
