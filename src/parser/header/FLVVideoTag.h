//
// Created by bo on 2021/12/10.
//

#ifndef AVPLAYER_FLVVIDEOTAG_H
#define AVPLAYER_FLVVIDEOTAG_H
using namespace std;

typedef struct Video_Tag {
    uint8_t frame_type; //4bit, 1 = key frame, 2 = inter frame, 3 = disposable inter frame, 4 = generated key frame,
                        // 5 = video info/command frame
    uint8_t codec_id; // 4 bit, 7 = avc(h264), 4 = VP6, 5 = VP6 with alpha channel

    uint8_t avc_packet_type; // 8 bit, if codec_id = 7时有值，0 = avc sequence header, 1 = AVC_NALU, 2 = deprecated

    uint32_t composition_time; // 24 bit, if avc_packet_type = 1时有值

    
} VideoTag;
#endif //AVPLAYER_FLVVIDEOTAG_H
