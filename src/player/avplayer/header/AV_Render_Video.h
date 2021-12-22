//
// Created by bo on 2021/12/20.
//

#ifndef AVPLAYER_AV_RENDER_VIDEO_H
#define AVPLAYER_AV_RENDER_VIDEO_H
#include "AV_Render_Interface.h"
#include "../../../thread/ThreadSafeQueue.h"
#include "../../../parser/header/YUVFileData.h"
typedef struct Video_Render_Parameters {
    uint8_t *data; // 文件二进制内容
    int width; //分辨率宽
    int height; //分辨率高
    Uint32 format; //文件格式
    int pin;
}VideoRenderParameters;

class AV_Render_Video : public IAVRender{
public:
    AV_Render_Video() {

    }
    ~AV_Render_Video() {
        onDestroy();
    }

    bool init() override;
    bool onUpdate() override;
    bool onRender() override;
    bool onStop() override;
    bool onDestroy() override;

    bool openDevice(void * params) override{
        Video_Render_Parameters *video_params = static_cast<Video_Render_Parameters *>(params);
        bool ret = openVideoDevice(video_params->data, video_params->width, video_params->height, video_params->format,
                                   video_params->pin);
        return ret;
    }

    void setBuffer(ThreadSafeQueue<YUVFileData> *queue);
private:
    SDL_Window*        window;
    SDL_Renderer*    renderer;
    SDL_Texture*     texture;
    #define WINDOW_TITLE         "AV Player by bo"

    const int SCREEN_WIDTH     = 900;
    const int SCREEN_HEIGHT    = 600;

    uint8_t *current_yuv_data = nullptr;
    int current_pin = 0;

    ThreadSafeQueue<YUVFileData> *yuv_fileQueue = nullptr;

    bool openVideoDevice(uint8_t *data, int width, int height, Uint32 format, int pin);
};


#endif //AVPLAYER_AV_RENDER_VIDEO_H
