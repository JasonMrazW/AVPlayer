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

bool AV_Render_Video::onUpdate(double *remaining_time) {
    YUVItem *item = getNextPic(remaining_time);

    //    cout << "item is null? " << item << endl;
    if (item != nullptr) {
        current_yuv_data = item;
    }


    //    std::cout << getSystemClock->getMainClock() << endl;
    //    std::cout << "yuv buffer size:" << yuv_fileQueue->current_size << std::endl;

    if(texture == nullptr) {
        openVideoDevice(current_yuv_data->width, current_yuv_data->height, current_yuv_data->format);
    }
//    cout << "data address:" << &(current_yuv_data->data) << endl;

    return true;
}

bool AV_Render_Video::onRender() {
    try {
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

YUVItem *AV_Render_Video::getNextPic(double *remaining_time) {
    YUVItem *item = new YUVItem();
    if (current_yuv_data == nullptr) { //第一帧
        yuv_fileQueue->dequeue(*item);
        current_yuv_data = item;
    } else {
        //只取当前队列里即将显示的一帧，但是不出队
        yuv_fileQueue->getFront(*item);
    }

    if (frame_timer == 0) {
        frame_timer = SyncClock::getCurrentRelativeTime();
    }

    //计算当前帧于上一帧的差值
    double ideal_duration = item->pts - current_yuv_data->pts;
    //计算矫正后的差值
    double delay = SyncClock::computeVideoFrameDelay(video_clock, getSystemClock->getMainClock(), ideal_duration);

    double current_time = SyncClock::getCurrentRelativeTime();
    //还没到要显示的时间，继续显示上一帧
    if (current_time < frame_timer + delay) {
        *remaining_time = FFMIN(frame_timer + delay - current_time, *remaining_time);
        return nullptr;
    }

    frame_timer += delay;
    //矫正frame_timer，如果落后太多，就直接更新成当前时间
    if (delay > 0 && current_time - frame_timer > AV_SYNC_THRESHOLD_MAX) {
        frame_timer = current_time;
    }

    //更新video的时钟，基于当前帧的pts
    SyncClock::setClockAtTime(video_clock, item->pts, SyncClock::getCurrentRelativeTime());

    //当前帧出队
    yuv_fileQueue->dequeue(*item);
    //再拿当前帧的下一帧，判断是否需要丢帧
    YUVItem next_item;
    yuv_fileQueue->getFront(next_item);
    double next_duration = next_item.pts - item->pts;
    //当前时间比下一帧的末尾还要晚，来不及显示了，那么丢弃当前帧
    if (current_time > next_duration + frame_timer) {
        dropped_frame++;
        return getNextPic(remaining_time);
    }

    return item;
}

