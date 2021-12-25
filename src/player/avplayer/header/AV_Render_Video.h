//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_VIDEO_H
#define AVPLAYER_AV_RENDER_VIDEO_H
#include "AV_Render_Interface.h"
#include "../../../thread/ThreadSafeQueue.h"
#include "AV_Render_RawItem.h"
#include "AV_Render_IClockCallback.h"

typedef struct Video_Render_Parameters {
    uint8_t *data; // 文件二进制内容
    int width; //分辨率宽
    int height; //分辨率高
    Uint32 format; //文件格式
    int pin;
}VideoRenderParameters;

class AV_Render_Video : public IAVRender{
public:
    AV_Render_Video(IClockCallBack *callback): IAVRender() {
        getSystemClock = callback;
    }
    ~AV_Render_Video() {
        onDestroy();
    }

    bool init() override;
    bool onUpdate() override;
    bool onRender() override;
    bool onStop() override;
    bool onDestroy() override;
    void setBuffer(ThreadSafeQueue<YUVItem> *queue);
private:
    SDL_Window*        window;
    SDL_Renderer*    renderer;
    SDL_Texture*     texture = nullptr;
    #define WINDOW_TITLE         "AV Player by bo"

    const int SCREEN_WIDTH     = 900;
    const int SCREEN_HEIGHT    = 600;

    YUVItem *current_yuv_data = nullptr;
    int current_pin = 0;
    IClockCallBack *getSystemClock;
    ThreadSafeQueue<YUVItem> *yuv_fileQueue = nullptr;
    bool openVideoDevice(int width, int height, Uint32 format);

    YUVItem *getNextPic();
};


#endif //AVPLAYER_AV_RENDER_VIDEO_H
