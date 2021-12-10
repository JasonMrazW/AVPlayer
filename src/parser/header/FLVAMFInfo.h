//
// Created by bo on 2021/12/8.
//

#ifndef AVPLAYER_FLVAMFINFO_H
#define AVPLAYER_FLVAMFINFO_H
static const uint32_t FLV_SCRIPT_TAG_END = 0x000009;//24 bits 结尾



enum Script_TYPE {
    NUMBER = 0, //Double,uint32_t
    BOOLEAN, // uint8_t
    STRING, // script data string
    OBJECT, // script data object
    MOVIE_CLIP, //没有定义
    NULL_TYPE, //没有定义
    UNDEFINED_TYPE, //没有定义
    REFERENCE, //uint16_t
    ECMA_ARRAY = 8, //script data ecma array
    OBJECT_AND_MARKER,//没有定义
    STRICT_ARRAY,//script data strict array
    DATE = 11, //script data date
    LONG_STRING = 12 //script data long string
};
typedef struct Script_Data_Value {
    uint8_t type;
    void    *script_data_value;
} ScriptDataValue;

typedef struct Script_Data_Number {
    double value;
} ScriptDataNumber;

typedef struct Script_Data_String {
    uint16_t string_length;
    uint8_t *string_data;
} ScriptString;

typedef struct Script_Data_LongString {
    uint32_t string_length;
    uint8_t *string_data;
} ScriptLongString;

typedef struct Script_Data_ObjectProperty {
    ScriptString name;
    ScriptDataValue value;
} ScriptDataObjectProperty;

typedef struct Script_Data_Object {
    Script_Data_ObjectProperty *objectProperties;
} ScriptDataObject;

typedef struct Script_EMCA_Array {
    uint32_t array_length;//item数目
    ScriptDataObjectProperty *variables;
} ScriptEMCAArray;

typedef struct Script_Strict_Array {
    uint32_t array_length; //item数目
    ScriptDataValue *array_value; //items
} ScriptStrictArray;

typedef struct Script_Data_Date {
    uint32_t date_time;
    int16_t offset;
} ScriptDataDate;

typedef struct Script_Tag_Body {
    Script_Data_Value name;
    Script_Data_Value value;
} ScriptTagBody;

#endif //AVPLAYER_FLVAMFINFO_H
