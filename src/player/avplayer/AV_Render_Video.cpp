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
    YUVItem item;
    yuv_fileQueue->dequeue(item);

    if(texture == nullptr) {
        openVideoDevice(item.width, item.height, item.format);
    }

    try {
        if(texture == nullptr) {
            return false;
        }
        std::cout << "enqueue result:" << std::endl;

        int ret = SDL_UpdateTexture(texture, nullptr, item.data, item.pin);
        if (ret < 0) {
            std::cout << "enqueue result:" << SDL_GetError() << std::endl;;
        }
        SDL_RenderClear(renderer);
        // Do your drawing here
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        SDL_RenderPresent(renderer);
    } catch (std::exception &e) {
        cout << "error:" << e.what() << endl;
    }

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

bool AV_Render_Video::openVideoDevice(int width, int height, uint32_t format) {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
    texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, width, height);
    return true;
}

void AV_Render_Video::setBuffer(ThreadSafeQueue<YUVItem> *queue) {
    yuv_fileQueue = queue;
}
