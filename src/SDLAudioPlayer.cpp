//
// Created by bo on 2021/11/29.
//

#include "SDLAudioPlayer.h"

Uint8 * SDLAudioPlayer::audio_chunk;
Uint32 SDLAudioPlayer::audio_length;
Uint8 * SDLAudioPlayer::audio_pos;
char * SDLAudioPlayer::pcm_buffer;
unsigned short SDLAudioPlayer::audioFormat;

const Uint32 SDLAudioPlayer::SDL_EVENT_BUFFER_END = 1;
const double SDLAudioPlayer::d = 12;
const double SDLAudioPlayer::factor = pow(10, d / 20);

SDLAudioPlayer::SDLAudioPlayer() : running(false){

}

SDLAudioPlayer::~SDLAudioPlayer() {

}

int SDLAudioPlayer::onInit(char *audioPath) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }

    //init audio info
    audioFormat = AUDIO_S16LSB;

    audioSpec = SDL_AudioSpec();
    audioSpec.samples = 1024;
    audioSpec.freq = 44100;
    audioSpec.format = audioFormat;
    audioSpec.channels = 2;
    audioSpec.silence = 0;
    audioSpec.callback = fillDataCallBack;

    if ((audioDeviceId = SDL_OpenAudioDevice(nullptr,0,&audioSpec,&audioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }

    return APP_START;
}

void SDLAudioPlayer::onClean() {
    audioStream.close();
    SDL_Quit();
}

void SDLAudioPlayer::onEvent(SDL_Event *event) {


    switch (event->type) {
        case SDL_USEREVENT: {
            //判断是否数据读完
            if(!audioStream.read(pcm_buffer, pcm_buffer_size)) {
                //循环播放
                audioStream.clear();
                audioStream.seekg(0, std::ios_base::beg);
                audioStream.read(pcm_buffer, pcm_buffer_size);
            }
            int len;
            char *temp;
            speedupVolumn(len, temp);

            //取值
            audio_chunk = (Uint8 *)temp;
            audio_length = len;
            audio_pos = audio_chunk;
            break;
        }
        case SDL_QUIT:
            running = false;
            break;
    }
}

void SDLAudioPlayer::speedupVolumn(int &len, char *&temp) const {
    len= pcm_buffer_size / 2;
    temp= new char[len];
    // 音频加速，简单通过数据采样，丢弃掉一部分数据实现
    // 此处未考虑最后一次数据读取的buffer大小是小于pcm_buffer_size的
    for (int i = 0; i < len; ) {
        temp[i] = pcm_buffer[i*2];
        temp[i+1] = pcm_buffer[i*2+1];
        i+=2;
    }
}

void SDLAudioPlayer::play(char *audioPath) {
    int state = onInit(audioPath);
    if (state != APP_START) {
        return;
    }

    SDL_Event sdlEvent;

    //open file
    audioStream.open(audioPath, std::ios::in|std::ios::binary);

    //running = true;
    pcm_buffer = new char[pcm_buffer_size];
    running = true;

    SDL_PauseAudioDevice(audioDeviceId, 0);
    SDL_CreateThread(sdl_thread_custom_event, "custom_event", nullptr);

    while(running) {
        while(SDL_PollEvent(&sdlEvent)) {
            onEvent(&sdlEvent);
        }
    }

    free(pcm_buffer);
    free(audio_chunk);
    onClean();
}

void SDLAudioPlayer::fillDataCallBack(void *userdata, Uint8 * stream, int len) {
    SDL_memset(stream, 0, len);
    if (audio_length == 0) {
        return;
    }
    std::cout << "audio length" << len << std::endl;

    len = len > audio_length ? audio_length : len;

    //单声道变换
    playSingleChannel(len);

    // 基于左声道数据，进行音量减半的处理
    halfToVolumn(len);

    //混合播放
    SDL_MixAudioFormat(stream, audio_pos, audioFormat, len, SDL_MIX_MAXVOLUME);
    //SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);

    //单独播放audio_pos
    //memcpy(stream, audio_pos, len);
    audio_pos += len;
    audio_length -= len;

    if (audio_length <= 0) {
        notifyGetAudioFrame();
    }
}

void SDLAudioPlayer::playSingleChannel(int len) {
    for (int i = 0; i < len;) {
        //播放右声道，仅保留右声道的数据，左声道数据置为0
//        if (i % 4 == 0) {
//            audio_pos[i] = 0;
//            audio_pos[i+1] = 0;
//            i += 4;
//        }

        //播放左声道，仅保留左声道的数据，右声道数据置为0
//         L L R R L L R R L L R
//         0 1 2 3 4 5 6 7 8 9 10
        if (i == 0) i += 2;
        if ((i+2) % 4 == 0) {
            audio_pos[i] = 0;
            audio_pos[i+1] = 0;
            i += 4;
        }
    }
}

void SDLAudioPlayer::halfToVolumn(int len) {
    for (int i = 0; i < len; ) {
        short temp = getShort(audio_pos[i+1], audio_pos[i]);
        temp = temp * factor;

        audio_pos[i] = (char)(temp & 0x00FF);
        audio_pos[i+1] = (char)(temp >> 8);
        i += 2;
    }
}

void SDLAudioPlayer::notifyGetAudioFrame() {
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
}

//触发读取音频数据
int SDLAudioPlayer::sdl_thread_custom_event(void *){
    // 延时 5 秒
    SDL_Delay(1000);
    // 创建自定义事件并发送到消息队列中去
    notifyGetAudioFrame();
}

short SDLAudioPlayer::getShort(char high, char low) {
    return (short)((high<< 8)|(low & 0xFF));
}

