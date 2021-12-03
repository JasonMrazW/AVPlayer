//
// Created by bo on 2021/12/2.
//

#include "header/AACParser.h"
#include "../io/AVFileUtil.h"
#include "../util/BinaryUtil.h"

using namespace std;

const char *aac_path = "resources/audio/sample_weilan.aac";

AACParser::AACParser() {

}

AACParser::~AACParser() {

}

void AACParser::init() {
    uint32_t file_length; //单位：short
    uint8_t *audio_content = AVFileUtil::readBinaryFile(aac_path, file_length);

    uint16_t temp_word;
    uint32_t *temp_position_array = new uint32_t[file_length / 100];
    uint16_t frame_count = 0;
    uint16_t frame_size;

    for (int i = 0; i < file_length;) {
        frame_size = 0;
        temp_word = BinaryUtil::getUint16(audio_content[i], audio_content[i+1]);
        //获取前12位
        temp_word = temp_word >> 4;

        //判断是否sync word
        if (temp_word == SYNC_WORD) {
            temp_position_array[frame_count++] = i;
            //cout << (int)audio_content[i] << ",";

            //找到frame的长度，存储在第31~43位的位置，共计13bit
            //在第4个字节的后2位，整个第5个字节，第六个字节的前3位
            frame_size |= (audio_content[i+3] & 0x3) << 11;
            frame_size |= audio_content[i+4] << 3;
            frame_size |= (audio_content[i+5] & 0xE0) >> 5;
            i += frame_size;
        } else {
            cout << "error: audio frame size is not match" << endl;
            return;
        }
    }

    //重新分配合适的内存
    uint32_t *start_position_array = new uint32_t[frame_count];
    memcpy(start_position_array, temp_position_array, frame_count);
    delete[] temp_position_array;

    uint16_t index = 0;
    ADTSFrame *frame_array = new ADTSFrame[frame_count];
    ADTSFrame *temp_frame;
    cout << "adts_frame count :" << frame_count << endl;
    do {
        temp_frame = new ADTSFrame();
        temp_frame->fixed_header = new FixedHeader();
        temp_frame->variable_header = new VariableHeader();

        frame_array[index] = *temp_frame;

        uint8_t *temp_frame_content = &audio_content[start_position_array[index]];


        //the second char
        //get id
        temp_frame->fixed_header->id = (temp_frame_content[1] & 0x08) >> 3;
        //get layer
        temp_frame->fixed_header->layer = (temp_frame_content[1] & 0x06) >> 1;
        //get protection_absent
        temp_frame->fixed_header->protection_absent = temp_frame_content[1] & 0x01;

        //the third char
        //get profile
        temp_frame->fixed_header->profile = (temp_frame_content[2] & 0xc0) >> 6;
        //get sampling_frequency_index
        temp_frame->fixed_header->sampling_frequency_index = (temp_frame_content[2] & 0x3c) >> 2;
        //get private_bit
        temp_frame->fixed_header->private_bit = (temp_frame_content[2] & 0x02) >> 1;

        //the third char last bit & the fourth char
        //get channel_configuration
        temp_frame->fixed_header->channel_configuration = (temp_frame_content[2] & 0x01) << 2 | (temp_frame_content[3] & 0xC0) >> 6;
        //get origin_copy
        temp_frame->fixed_header->original_copy = (temp_frame_content[3] & 0x20) >> 5;
        //get home
        temp_frame->fixed_header->home = (temp_frame_content[3] & 0x10) >> 4;

        //the fourth car last 4 bits
        temp_frame->variable_header->frame_length = (temp_frame_content[3] & 0x3) << 11 | temp_frame_content[4] << 3 | (temp_frame_content[5] & 0xE0) >> 5;

        //第6个字节的后5位 + 第7个字节的前6位
        temp_frame->variable_header->buffer_fullness = (temp_frame_content[5] & 0x1F) << 6 | (temp_frame_content[6] & 0xFC) >> 2;

        //第7个字节最后2位
        temp_frame->variable_header->number_of_raw_blocks_in_frame = temp_frame_content[6] & 0x03;

        //print value
        cout << (int)temp_frame->fixed_header->profile << endl;

        index++;
    } while (index < frame_count);
}
