//
// Created by bo on 2021/11/29.
//

#include "SDLAudioStreamPlayer.h"

Uint8 * SDLAudioStreamPlayer::audio_chunk;
Uint32 SDLAudioStreamPlayer::audio_length;
Uint8 * SDLAudioStreamPlayer::audio_pos;
char * SDLAudioStreamPlayer::pcm_buffer;
unsigned short SDLAudioStreamPlayer::audioFormat;

const Uint32 SDLAudioStreamPlayer::SDL_EVENT_BUFFER_END = 1;
const double SDLAudioStreamPlayer::d = 12;
const double SDLAudioStreamPlayer::factor = pow(10, d / 20);

SDLAudioStreamPlayer::SDLAudioStreamPlayer() : running(false){

}

SDLAudioStreamPlayer::~SDLAudioStreamPlayer() {

}

void SDLAudioStreamPlayer::onClean() {
    audioStream.close();
    SDL_Quit();
}

void SDLAudioStreamPlayer::onEvent(SDL_Event *event) {
    switch (event->type) {
        case SDL_USEREVENT: {
            //判断是否数据读完
            if(!audioStream.read(pcm_buffer, pcm_buffer_size)) {
                //循环播放
                audioStream.clear();
                audioStream.seekg(0, std::ios_base::beg);
                audioStream.read(pcm_buffer, pcm_buffer_size);
            }

            //16 bit pcm转换为 8 bit pcm，丢弃低位数据，具体步骤：
            //1. signed int16--->signed char
            //2. signed char --->unsigned char
            unsigned char *tempBuffer = _16BitTo8Bit();

            //            int len;
            //            char *temp;
            //            speedupVolumn(len, temp);

            //取值
            audio_chunk = (Uint8 *)tempBuffer;
            audio_length = pcm_buffer_size/2;
            audio_pos = audio_chunk;
            break;
        }
        case SDL_QUIT:
            running = false;
            break;
    }
}

unsigned char *SDLAudioStreamPlayer::_16BitTo8Bit() const {
    //16 bit pcm转换为 8 bit pcm，丢弃低位数据，具体步骤：
    //1. signed int16--->signed char
    //2. signed char --->unsigned char
    unsigned char *tempBuffer = new unsigned char[pcm_buffer_size / 2];


    for (int i = 0; i < pcm_buffer_size;) {
        //short temp = getShort(pcm_buffer[i+1], pcm_buffer[i]);
        signed char sample8 = pcm_buffer[i+1];

        unsigned char u_sample8 = sample8 + 128;

        tempBuffer[i/2] = u_sample8;
        i += 2;
    }

    //8 bit to 16 bit
    //    unsigned char sample8 = ...;
    //    Uint16 sample16 = (Uint16)(sample8-128) << 8;
    return tempBuffer;
}

void SDLAudioStreamPlayer::speedupVolumn(int &len, char *&temp) const {
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

void SDLAudioStreamPlayer::play(char *audioPath) {
    //open file
    audioStream.open(audioPath, std::ios::in|std::ios::binary);
}

void SDLAudioStreamPlayer::startAudioPlayer() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return;
    }

    pcm_buffer = new char[pcm_buffer_size];
    running = true;

    SDL_CreateThread(sdl_thread_custom_event, "custom_event", nullptr);
    SDL_Event sdlEvent;
    while(running) {
        while(SDL_PollEvent(&sdlEvent)) {
            onEvent(&sdlEvent);
        }
    }

    free(pcm_buffer);
    free(audio_chunk);
    onClean();
}

int SDLAudioStreamPlayer::openAudioDevice(uint16_t audio_format, uint16_t nb_samples, uint32_t freq, uint8_t channels) {
    if (audioDeviceId != 0) {
        SDL_CloseAudio();
    }
    //init audio info
    audioFormat = audio_format;
    audioSpec = SDL_AudioSpec();
    audioSpec.samples = nb_samples;
    audioSpec.freq = freq;
    audioSpec.format = audioFormat;
    audioSpec.channels = channels;
    audioSpec.silence = 0;
    audioSpec.callback = fillDataCallBack;

    if ((audioDeviceId = SDL_OpenAudioDevice(nullptr,0,&audioSpec,nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE)) < 2) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return APP_STOP;
    }
    //SDL_PauseAudioDevice(audioDeviceId, 0);
}

void SDLAudioStreamPlayer::fillDataCallBack(void *userdata, Uint8 * stream, int len) {
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

void SDLAudioStreamPlayer::playSingleChannel(int len) {
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

/**
 * 音量调整
 * @param len
 */
void SDLAudioStreamPlayer::halfToVolumn(int len) {
    for (int i = 0; i < len; ) {
        short temp = getShort(audio_pos[i+1], audio_pos[i]);
        temp = temp * factor;

        audio_pos[i] = (char)(temp & 0x00FF);
        audio_pos[i+1] = (char)(temp >> 8);
        i += 2;
    }
}

void SDLAudioStreamPlayer::notifyGetAudioFrame() {
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
}

//触发读取音频数据
int SDLAudioStreamPlayer::sdl_thread_custom_event(void *){
    // 延时 5 秒
    SDL_Delay(1000);
    // 创建自定义事件并发送到消息队列中去
    notifyGetAudioFrame();
}

/**
 * 两个char组合成一个short
 * @param high
 * @param low
 * @return
 */
short SDLAudioStreamPlayer::getShort(char high, char low) {
    return (short)((high<< 8)|(low & 0xFF));
}

