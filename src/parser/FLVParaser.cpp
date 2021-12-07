//
// Created by bo on 2021/12/7.
//

#include "header/FLVParaser.h"
#include "../io/AVFileUtil.h"

using namespace std;

const char* flv_file_path = "resources/video/sample.flv";

FLVParaser::FLVParaser() {}

FLVParaser::~FLVParaser() {}

void FLVParaser::init() {
    uint32_t file_length = 0;
    uint8_t *file_content = AVFileUtil::readBinaryFile(flv_file_path, file_length);

    //parser header
    parserHeader(file_content);
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
