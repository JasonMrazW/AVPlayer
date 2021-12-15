//
// Created by bo on 2021/12/15.
//

#ifndef AVPLAYER_RTPPARSER_H
#define AVPLAYER_RTPPARSER_H
#include <cstdlib>
#include "../../util/BinaryUtil.h"
#include "H264Parser.h"

//常规情况下，12个字节
struct RTP_HEADER {
    uint8_t version; // 2bit
    bool padding; // 1bit, 标识packet尾部是否存在补齐的数据
    bool extension; // 1bit, 标识是否存在扩展的header
    uint32_t csrc_count; // 4bit， 标识csrc的长度
    uint8_t maker; //1bit，对于video，标识结束帧；对于audio，标识会话的开始
    uint8_t payload_type; // 7bit，标识后续payload部分的数据类型视频or音频，定义见https://datatracker.ietf.org/doc/html/rfc3551#page-32
    uint16_t sequence_number; // 16bit，packet唯一标识符
    uint32_t timestamp; // 32bit，标识采样时间
    uint32_t ssrc; // 32bit,Synchronization source identifier，具有唯一性
    uint32_t *csrc;// 32bit，个数由csrc_count决定
};

class RTPParser {
private:
    H264Parser *h264Parser;
public:
    RTPParser();
    ~RTPParser();

    void parseOnePacket(uint8_t *data, uint32_t length);

    void loadH264HeaderInfo(const uint8_t *tmp_data);
};


#endif //AVPLAYER_RTPPARSER_H
