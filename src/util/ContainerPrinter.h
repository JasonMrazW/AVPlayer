//
// Created by bo on 2022/2/21.
//

#ifndef AVPLAYER_CONTAINERPRINTER_H
#define AVPLAYER_CONTAINERPRINTER_H
extern "C" {
    #include "libavutil/avutil.h"
};
#include <iostream>

class ContainerPrinter {
public:
    static void printAVDictionary(AVDictionary *dic) {
        AVDictionaryEntry *entry = nullptr;
        int count = av_dict_count(dic);
        std::cout << "stream dic count:" << count << std::endl;
        for (int i = 0; i < count; ++i) {
            entry = av_dict_get(dic, "", entry, AV_DICT_IGNORE_SUFFIX);
            std::cout << entry->key << ":" << entry->value << std::endl;
        }
    }
};
#endif //AVPLAYER_CONTAINERPRINTER_H
