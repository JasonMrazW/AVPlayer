//
// Created by bo on 2021/12/10.
//

#ifndef AVPLAYER_FLVVIDEOTAG_H
#define AVPLAYER_FLVVIDEOTAG_H
using namespace std;

enum AVCPacketType {
    AVC_SEQUENCE_HEADER = 0,
    AVC_NALU,
    AVC_END_OF_SEQUENCE //deprecated
};

enum AVCFrameType {
    KEY_FRAME = 1,
    INTER_FRAME,
    DISPOSABLE_INTER_FRAME,
    GENERATED_KEY_FRAME
};

typedef struct AVC_Decoder_Configuration_Record {

} AVCDecoderConfigurationRecord;

typedef struct Video_Tag {
    uint8_t frame_type; //4bit, 1 = key frame, 2 = inter frame, 3 = disposable inter frame, 4 = generated key frame,
                        // 5 = video info/command frame
    uint8_t codec_id; // 4 bit, 7 = avc(h264), 4 = VP6, 5 = VP6 with alpha channel

    uint8_t avc_packet_type; // 8 bit, if codec_id = 7时有值，0 = avc sequence header, 1 = AVC_NALU, 2 = deprecated

    uint32_t composition_time; // 24 bit, if avc_packet_type = 1时有值

    AVCDecoderConfigurationRecord record; //avc sequence header

    uint8_t *avc_data; //NALUs
} VideoTag;

static string getAVCFrameTypeDescription(uint8_t frame_type) {
    string result;
    switch (frame_type) {
        case KEY_FRAME:
            result = "key frame";
            break;
        case INTER_FRAME:
            result = "inter frame";
            break;
        case DISPOSABLE_INTER_FRAME:
            result = "disposable inter frame";
        case GENERATED_KEY_FRAME:
            result = "generated key frame";
            break;
    }
    return result;
}

#endif //AVPLAYER_FLVVIDEOTAG_H
