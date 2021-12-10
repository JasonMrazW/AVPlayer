//
// Created by bo on 2021/12/3.
//

#ifndef AVPLAYER_BINARYUTIL_H
#define AVPLAYER_BINARYUTIL_H
#include <cstdlib>
#include <iostream>
#include <cmath>

class BinaryUtil {
public:
    static uint16_t getUint16(uint8_t high, uint8_t low);

    static uint32_t getUint32(uint8_t *content, uint8_t size);

    static uint64_t getUint64(uint8_t *content, uint8_t size);

    static uint8_t *getUint8(uint8_t *content, uint16_t size);

    static double getDouble(uint8_t *content, uint8_t size);

    static uint8_t ReadByte(uint8_t *data);

    static uint64_t Read8Bytes(uint8_t *data);

    static double stringToDouble(uint8_t *hex, uint32_t length);

    bool isLittleEndian2() {
        int x = 1;
        return *((char*) (&x)) == 1;
    }

    template<class T>
            void printBinary(T d) {
        char* p = (char*)&d;
        int sz = sizeof(T); // bytes

        char* buff = new char[sz * 8 + 1];
        buff[sz * 8] = '\0';
        int used = 0;

        for (int n = 0; n < sz; n++) {
            for (int m = 0; m < 8; m++) {
                if ((p[n] >> m) & 1)
                    used += sprintf(buff + used, "1");
                else
                    used += sprintf(buff + used, "0");
            }
        }

        if (isLittleEndian2()) {
            int a = 0;
            int b = sz * 8 - 1;
            while (a < b) {
                buff[a] ^= buff[b];
                buff[b] ^= buff[a];
                buff[a] ^= buff[b];
                a++;
                b--;
            }
        }

        printf("%s\n", buff);
        delete [] buff;
    }




    float parseFloat(char* s) {
        int sign = s[0] - '0';
        float M = 0;
        int E = 0;

        for (int n = 1; n <= 8; n++) {
            E = E * 2 + (s[n] - '0');
            std::cout << E << ",";
        }

        for (int n = 9; n <= 31; n++) {
            M += pow(2, 8 - n) * (s[n] - '0');
        }

        printf("sign=%d, E=%d, M=%f\n", sign, E, M);

        return pow((-1), sign) * (M + 1) * pow(2, (E - 127));
    }
};



#endif //AVPLAYER_BINARYUTIL_H
