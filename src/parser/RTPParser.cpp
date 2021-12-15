//
// Created by bo on 2021/12/15.
//

#include "header/RTPParser.h"

void RTPParser::parseOnePacket(uint8_t *data, uint32_t length) {
    uint8_t *tmp_data = data;
    //解析header部分
    RTP_HEADER *header = new RTP_HEADER();

    uint8_t tmp_byte = *tmp_data;
    header->version = (tmp_byte & 0x80) >> 7;
    header->padding = (tmp_byte & 0x60) >> 6;
    header->extension = (tmp_byte & 0x40) >> 5;
    header->csrc_count = (tmp_byte &0x30);
    tmp_data++;

    tmp_byte = *tmp_data;
    header->maker = (tmp_byte & 0x80) >> 7;
    header->payload_type = (tmp_byte & 0x7F);
    tmp_data++;

    header->sequence_number = BinaryUtil::getUint16(*tmp_data, *(tmp_data+1));
    tmp_data+=2;

    header->timestamp = BinaryUtil::getUint32(tmp_data, 4);
    tmp_data+=4;

    header->ssrc = BinaryUtil::getUint32(tmp_data,4);
    tmp_data+=4;

    header->csrc = new uint32_t [header->csrc_count];
    for (int i = 0; i < header->csrc_count; ++i) {
        header->csrc[i] = BinaryUtil::getUint32(tmp_data,4);
        tmp_data+=4;
    }

//    std::clog << "version:" <<  (unsigned )header->version << std::endl;
//    std::clog << "padding:" <<  (unsigned )header->padding << std::endl;
//    std::clog << "extension:" <<  (unsigned )header->extension << std::endl;
//    std::clog << "csrc_count:" <<  (unsigned )header->csrc_count << std::endl;
//    std::clog << "maker:" <<  (unsigned )header->maker << std::endl;
//    std::clog << "payload_type:" <<  (unsigned )header->payload_type << std::endl;
//    std::clog << "sequence_number:" <<  (unsigned )header->sequence_number << std::endl;
//    std::clog << "timestamp:" <<  (unsigned )header->timestamp << std::endl;
//    std::clog << "ssrc:" <<  (unsigned )header->ssrc << std::endl;

    //load payload
    uint32_t payload_length = length - 12 - header->csrc_count * 4;
    uint8_t payload_type = tmp_data[0] & 0x1F;
    std::clog << "payload type:" << (unsigned )payload_type << std::endl;

    switch (payload_type) {
        case NALU_TYPE_STAP_A:
        case NALU_TYPE_STAP_B:
        case NALU_TYPE_MTAP_16:
        case NALU_TYPE_MTAP_24:
            //todo parse
            break;
        case NALU_TYPE_FU_A: {
            tmp_data++;
            uint8_t nalu_fragment_type = tmp_data[0] & 0xC0; // s e r nal type
            switch (nalu_fragment_type) {
                case 0x80: // NALU的开始位置
                std::clog << "nal start." << std::endl;
                loadH264HeaderInfo(tmp_data);
                    break;
                case 0x00:// NALU的中间位置
                std::clog << "nal middle, nal type:" << (unsigned )(tmp_data[0] &0x1F) << std::endl;
                    break;
                case 0x40:// NALU的结尾
                std::clog << "nal end, nal type:"  << (unsigned )(tmp_data[0] &0x1F) << std::endl;
                    break;
            }
            break;
        }
        case NALU_TYPE_FU_B:

            break;
        default:
            //一个packet对应一个nalu
            loadH264HeaderInfo(tmp_data);
            break;
    }

}

void RTPParser::loadH264HeaderInfo(const uint8_t *tmp_data) {
    H264Parser::NALU *nalu = new H264Parser::NALU();
    nalu->header = new H264Parser::NALU_Header();
    h264Parser->loadHeader(tmp_data, nalu, 0);
    std::clog << "nalu type:" << (unsigned )nalu->header->nal_type << std::endl;
}

RTPParser::~RTPParser() {
    delete h264Parser;
}

RTPParser::RTPParser() {
    h264Parser = new H264Parser();
}
