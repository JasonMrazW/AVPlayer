//
// Created by bo on 2021/12/7.
//

#include "header/FLVParaser.h"
#include "../io/AVFileUtil.h"
#include "../util/BinaryUtil.h"

using namespace std;

const char* flv_file_path = "resources/video/sample.flv";

FLVParaser::FLVParaser() {}

FLVParaser::~FLVParaser() {}

void FLVParaser::init() {
    uint32_t file_length = 0;
    uint8_t *file_content = AVFileUtil::readBinaryFile(flv_file_path, file_length);

    //parser header
    parserHeader(file_content);

    //parser tags
    //FLV Header size:9byte
    uint32_t start_position = 9;
    //pre tag size：4byte
    uint8_t  first_pre_tag_size = 4;
    //计算出指针的偏移位置
    uint8_t *tag_content = file_content + start_position + first_pre_tag_size;
    //解析具体的tag
    parseFLVTag(file_length, tag_content);

}

/**
 * 解析flv tag数据，返回对应的tag数组
 * @param file_length
 * @param file_content
 * @return
 */
FLVTag *FLVParaser::parseFLVTag(uint32_t file_length, uint8_t *tag_content) {
    //找出所有的tag，并统计出对应的类型
    FLVTag *tag_array = new FLVTag [file_length/1000];
    FLVTag temp_tag;
    uint32_t index = 0;

    uint16_t audio_count = 0;
    uint16_t video_count = 0;
    uint16_t script_count = 0;

    for (uint32_t i = 0; i < file_length; ) {
        temp_tag = FLVTag();
        tag_array[index++] = temp_tag;

        //1个字节
        temp_tag.tag_type = tag_content[i];
        //3个字节
        temp_tag.data_size = BinaryUtil::getUint32(tag_content+ i + 1,3);
        //4个字节
        temp_tag.timestamp = BinaryUtil::getUint32(tag_content + i + 4,4);
        //3个字节
        temp_tag.stream_id = BinaryUtil::getUint32(tag_content + i + 8,3);

        switch (temp_tag.tag_type) {
            case AUDIO:
                audio_count++;
                break;
            case VIDEO:
                video_count++;
                break;
            case SCRIPT:
                script_count++;
                break;
            default:
                cout << "unknown flv tag type: " << temp_tag.tag_type << endl;
                break;
        }
        //下一个tag
        i += 11 + 4 + temp_tag.data_size;
    }

    cout << "audio tag count: " << audio_count << endl;
    cout << "video tag count: " << video_count << endl;
    cout << "script tag count: " << script_count << endl;

    return tag_array;
}

/**
 * 解析FLV Header
 * @param file_content
 */
void FLVParaser::parserHeader(const uint8_t *file_content) {
    auto *flv_header = new FLVHeader() ;
    //前3个字节
    flv_header->signature = new uint8_t [3];
    memcpy(flv_header->signature, file_content, 3);
    //第4个字节
    flv_header->version = file_content[3];
    //第5个字节
    flv_header->hasAudio = (file_content[4] & 0x04) >> 2;
    flv_header->hasVideo = (file_content[4] & 0x01);
    //后4个字节
    flv_header->header_size = file_content[8];

    //print signature
    cout << "signature: " << flv_header->signature << endl;
    cout << "version: " <<  (int)flv_header->version << endl;
    cout << "hasAudio: " << flv_header->hasAudio << endl;
    cout << "hasVideo: " << flv_header->hasVideo << endl;
    cout << "header_size: " << flv_header->header_size << endl;
}
