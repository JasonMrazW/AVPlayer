//
// Created by bo on 2021/12/2.
//

#ifndef AVPLAYER_AACPARSER_H
#define AVPLAYER_AACPARSER_H
#include <cstdlib>

typedef struct ADTS_Fixed_Header {
    uint16_t    sync_word;
    uint8_t     id;
    uint8_t     layer;
    bool        protection_absent;
    uint8_t     profile;
    uint8_t     sampling_frequency_index;
    uint8_t     private_bit;
    uint8_t     channel_configuration;
    uint8_t     original_copy;
    uint8_t     home;
} FixedHeader;

typedef struct ADTS_Variable_Header {
    uint16_t frame_length;
    uint16_t buffer_fullness;
    uint8_t number_of_raw_blocks_in_frame;
} VariableHeader;

typedef struct ADTS_Frame {
    ADTS_Fixed_Header *fixed_header;
    ADTS_Variable_Header *variable_header;
} ADTSFrame;

#define SYNC_WORD 0xFFF

class AACParser {
public:
    AACParser();
    ~AACParser();

    void init();
};


#endif //AVPLAYER_AACPARSER_H
