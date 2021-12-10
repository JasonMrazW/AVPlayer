//
// Created by bo on 2021/12/7.
//

#include "header/FLVParaser.h"
#include "../io/AVFileUtil.h"
#include "../util/BinaryUtil.h"
#include <string>

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
    uint16_t  flv_tag_count = 0;
    FLVTag *tag_array = parseFLVTag(file_length, tag_content, flv_tag_count);

    //取第一个Script Tag进行数据解析
    uint16_t index = 0;
    FLVTag script_tag;

    //script tag常规情况下肯定是第一个tag
    if (tag_array[0].tag_type == SCRIPT) {
        script_tag = tag_array[index];
    } else {
        cout << "parse error : script tag is not the first tag!!!" << endl;
        return;
    }

    //解析onMetaData
    //stringLength----stringdata----type----value
    //16bits
    //parse first name：
    cout << "script data info:" << endl;
    ScriptTagBody *script_tag_body = new ScriptTagBody();
    script_tag_body->name = ScriptDataValue();
    script_tag_body->value = ScriptDataValue();

    uint32_t name_length = parseScriptData(script_tag.data, &script_tag_body->name);
    uint8_t *temp_name = static_cast<ScriptString*>(script_tag_body->name.script_data_value)->string_data;
    cout << "script header key:" << &temp_name [ '\0'] << endl;
    parseScriptData(script_tag.data + name_length, &script_tag_body->value);
}

uint32_t FLVParaser::parseScriptData(uint8_t *script_data_p, ScriptDataValue *data_value) {
    //1个字节的type
    data_value->type = script_data_p[0];
    //根据type来确定value部分的长度
    //cout << "type:" << unsigned (data_value->type) << endl;
    uint32_t script_data_length = 1;
    uint8_t * script_data = script_data_p+1;
    //根据类型解析接下来的数据
    switch (data_value->type) {
        case NUMBER: {// double
            ScriptDataNumber temp;
            data_value->script_data_value = &temp;
            std::stringstream stream;
            temp.value = BinaryUtil::getDouble(script_data, 8);

            script_data_length += 8;
            break;
        }
        case BOOLEAN:{ // boolean
            data_value->script_data_value = &script_data[0];

            script_data_length += 1;
            break;
        }
        case STRING: {
            ScriptString string_value;
            uint32_t length = readScriptString(script_data, string_value);
            data_value->script_data_value = &string_value;

            script_data_length += length;
            break;
        }
        case OBJECT: {
            ScriptDataObject  data_object;
            data_value->script_data_value = &data_object;
            uint32_t length = readScriptDataObject(script_data, data_object);
            script_data_length += length;
            break;
        }
        case MOVIE_CLIP:
        case NULL_TYPE:
        case UNDEFINED_TYPE:
            //do nothing
            break;
        case REFERENCE: {
            data_value->script_data_value = reinterpret_cast<void *>(BinaryUtil::getUint16(script_data[0],
                                                                                           script_data[1]));

            script_data_length += 2;
            break;
        }
        case ECMA_ARRAY: {
            ScriptEMCAArray array;
            data_value->script_data_value = &array;

            array.array_length = BinaryUtil::getUint32(script_data, 4);
            array.variables = new ScriptDataObjectProperty [array.array_length];

            uint32_t obj_length = 4;

            for (int i = 0; i < array.array_length; ++i) {
                ScriptDataObjectProperty aProperty = array.variables[i];
                //read name
                uint32_t name_length = readScriptString(script_data + obj_length, aProperty.name);

                //read value
                uint32_t value_length = parseScriptData(script_data + obj_length + name_length, &aProperty.value);

                if (aProperty.value.type == NUMBER) {
                    double result = (static_cast<ScriptDataNumber*>(aProperty.value.script_data_value)->value);
                    cout << aProperty.name.string_data << ":"  << fixed <<  setprecision(0) << result << endl;
                } else if (aProperty.value.type == STRING){
                    uint8_t *result = static_cast<ScriptString*>(aProperty.value.script_data_value)->string_data;
                    cout << aProperty.name.string_data << ":" <<  result << endl;
                }

                obj_length += name_length + value_length;
            }

            obj_length += 3;

            script_data_length += obj_length;
            break;
        }
        case OBJECT_AND_MARKER:
            //do nothing
            break;
        case STRICT_ARRAY: {
            ScriptStrictArray  array;
            data_value->script_data_value = &array;

            array.array_length = BinaryUtil::getUint32(script_data, 4);
            array.array_value = new ScriptDataValue [array.array_length];
            uint32_t obj_length = 0;

            for (int i = 0; i < array.array_length; ++i) {
                ScriptDataValue dataValue = array.array_value[i];
                obj_length += parseScriptData(script_data + 4, &dataValue);
            }

            script_data_length += obj_length;
            break;
        }

        case DATE: {
            ScriptDataDate dataDate;
            data_value->script_data_value = &dataDate;

            dataDate.date_time = BinaryUtil::getUint32(script_data, 4);
            dataDate.offset = (short)BinaryUtil::getUint16(script_data[4], script_data[5]);

            script_data_length += 6;
            break;
        }
        case LONG_STRING: {
            ScriptLongString longString;
            data_value->script_data_value = &longString;

            longString.string_length = BinaryUtil::getUint32(script_data, 4);
            longString.string_data = BinaryUtil::getUint8(script_data + 4, longString.string_length);

            cout << endl << longString.string_data;
            script_data_length += longString.string_length;
            break;
        }
    }

    return script_data_length;
}

uint32_t FLVParaser::readScriptDataObject(uint8_t *script_data, ScriptDataObject &data_object) {

    data_object.objectProperties = new ScriptDataObjectProperty[1000];

    //读取直到遇到END终止符
    uint32_t i = 0;
    uint32_t index = 0;

    while (FLV_SCRIPT_TAG_END != BinaryUtil::getUint32(script_data + i, 3)) {
        ScriptDataObjectProperty aProperty = data_object.objectProperties[index++];

        //read name
        uint32_t name_length = readScriptString(script_data + i, aProperty.name);

        //read value
        uint32_t value_length = parseScriptData(script_data + i + name_length, &aProperty.value);
        i += name_length + value_length;

        if (aProperty.value.type == NUMBER) {
            double result = (static_cast<ScriptDataNumber*>(aProperty.value.script_data_value)->value);
            cout << aProperty.name.string_data << ":"  << fixed <<  setprecision(0) << result << endl;
        } else if (aProperty.value.type == STRING){
            uint8_t *result = static_cast<ScriptString*>(aProperty.value.script_data_value)->string_data;
            cout << aProperty.name.string_data << ":" <<  result << endl;
        }
    }
    i += 3;
    return i;
}

uint32_t FLVParaser::readScriptString(uint8_t *script_data, ScriptString &scriptString) {
    scriptString.string_length = BinaryUtil::getUint16(script_data[0], script_data[1]);
    scriptString.string_data = BinaryUtil::getUint8(script_data+2, scriptString.string_length);

    return 2 + scriptString.string_length;
}


/**
 * 解析flv tag数据，返回对应的tag数组
 * @param file_length
 * @param file_content
 * @return
 */
FLVTag *FLVParaser::parseFLVTag(uint32_t file_length, uint8_t *tag_content, uint16_t &flv_tag_count) {
    //找出所有的tag，并统计出对应的类型
    FLVTag *tag_array = new FLVTag [1200];
    FLVTag *temp_tag;

    uint16_t audio_count = 0;
    uint16_t video_count = 0;
    uint16_t script_count = 0;

    int tag_count = 0;
    for (uint32_t i = 0; i < file_length; ) {
        temp_tag = &tag_array[tag_count++];
        //tag_array[tag_count++] = *temp_tag;

        //1个字节
        temp_tag->tag_type = tag_content[i];
        //3个字节
        temp_tag->data_size = BinaryUtil::getUint32(tag_content+ i + 1,3);
        //4个字节
        temp_tag->timestamp = BinaryUtil::getUint32(tag_content + i + 4,4);
        //3个字节
        temp_tag->stream_id = BinaryUtil::getUint32(tag_content + i + 8,3);

        switch (temp_tag->tag_type) {
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
                cout << "unknown flv tag type: " << unsigned (temp_tag->tag_type) << endl;
                break;
        }

        //数据所在位置
        temp_tag->data = tag_content + i + 11;

        //下一个tag
        //下一个tag位置 = 上个tag的位置 + 上个tag的长度 + 4
        i += temp_tag->data_size + 11 + 4;
    }

    cout << "audio tag count: " << audio_count << endl;
    cout << "video tag count: " << video_count << endl;
    cout << "script tag count: " << script_count << endl;
    flv_tag_count = tag_count;

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


