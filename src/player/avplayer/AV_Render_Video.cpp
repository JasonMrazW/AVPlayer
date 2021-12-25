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
    YUVItem *item = getNextPic();
//    cout << "item is null? " << item << endl;
    if (item != nullptr) {
        current_yuv_data = item;
    } else {
//        cout << "empty" <<endl;
    }
//    std::cout << getSystemClock->getMainClock() << endl;
//    std::cout << "yuv buffer size:" << yuv_fileQueue->current_size << std::endl;

    if(texture == nullptr) {
        openVideoDevice(current_yuv_data->width, current_yuv_data->height, current_yuv_data->format);
    }

    try {
        cout << "data address:" << &(current_yuv_data->data) << endl;
        SDL_UpdateTexture(texture, nullptr, current_yuv_data->data, current_yuv_data->width);

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

YUVItem *AV_Render_Video::getNextPic() {
    double delay = 0;
    YUVItem *item = nullptr;
    if (current_pts == 0) {
        item = new YUVItem();
        yuv_fileQueue->dequeue(*item);
        current_pts.store(item->pts);
        time_base = item->time_base;
//        cout << "item start:" << item << endl;
        return item;
    }
    delay = (current_pts - getSystemClock->getMainClock()) * time_base * 1000;
    cout << "delay1:" << delay << endl;
    if (delay > 48) {
        return nullptr;
    }
    do {
        item = new YUVItem();
        yuv_fileQueue->dequeue(*item);
//        cout << "item next:" << item << endl;
//        cout << "main clock" << getSystemClock->getMainClock() << endl;

        //两者间隔小于24ms，换算出来就是delay * packet_time_base * 1000 < 24
        delay = (item->pts - getSystemClock->getMainClock()) * item->time_base * 1000 ;
        cout << "delay2:" << delay << endl;
        if (delay > 100) {
            return nullptr;
        }
        int times = delay/24;
        while (times > 1) {
            item = new YUVItem();
            yuv_fileQueue->dequeue(*item);
            times--;
        }
        current_pts.store(item->pts);
    }while(false);

    return item;
}
