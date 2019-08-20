//
//  h264_praser.cpp
//  VideoAudioBasic
//
//  Created by langren on 2019/6/3.
//  Copyright © 2019 Langren. All rights reserved.
//

#include "h264_praser.hpp"

#include <stdlib.h>
#include <string.h>
#include <iostream>

typedef enum {
    NALU_TYPE_SLICE = 1,  //非IDR图像中不采用数据划分的片段
    NALU_TYPT_DPA = 2,  //非idr图像中A类数据划分的片段
    NALU_TYPE_DPB = 3, //非idr图像中B类数据划分的片段
    NALU_TYPE_DPC = 4, //非idr图像中C类数据划分的片段
    NALU_TYPE_IDR = 5, //idr图像的片段
    NALU_TYPE_SEI = 6, //补充增强信息
    NALU_TYPE_SPS = 7,  //序列参数集合
    NALU_TYPE_PPS = 8,   //图像参数集合
    NALU_TYPE_AUD = 9,  //分界符
    NALU_TYPE_EOSEQ = 10,  //序列结束符
    NALU_TYPE_EOSTREAM = 11,   //码流结束符合
    NALU_TYPE_FILL = 12,   //填充
}NaluType;

typedef enum {  //nalu优先级
    NALU_PRIORITY_DISPOSABLE = 0,
    NALU_PRIORITY_LOW = 1,
    NALU_PRIORITY_HIGH = 2,
    NALU_PRIORITY_HIGHEST = 3,
}NaluPriority;

typedef struct {
    int startcodeprefix_len;   //nalu startcode占用的长度
    unsigned int len;   //去掉startcode的nalu的长度
    unsigned int max_size;
    
    //这三个属性是nalu的header
    int forbidden_bit;    //禁止位
    int nalu_refrence_idc;  //nal重要性指示，标志该NAL单元的重要性，值越大，越重要，解码器在解码处理不过来的时候，可以丢掉重要性为0的NALU。
    int nalu_unit_type;    //NALU类型，占用5bit
    
    //这个是nalu的具体data，也就是RBSP  RBSP+HEADER组成了nalu
    char *buf;
}NALU_t;

//找到以0x00 00 01开头的nalu
static int findStartCode0x0000001(unsigned char *buf) {
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 1) {
        return 0;
    }
    return 1;
}

//找到以0x00 00 00 01开头的nalu
static int findStartCode0x000000001(unsigned char *buf) {
    if (buf[0] != 0 || buf[1] != 0 || buf[2] != 0 || buf[3] !=1) {
        return 0;
    }
    return 1;
}

//在码流中分割nalu，通过startcode（0x000001或者0x00000001）分割
//返回值是一个nalu的length
int splitNalu(FILE *h264file, NALU_t *unit) {
    int pos = 0;
    unsigned char* buf = (unsigned char *)calloc(unit->max_size, sizeof(char));
    if (buf == NULL) {
        free(buf);
        return 0;
    }
    
    unit->startcodeprefix_len = 3;
    
    //如果读取的数据不够三个字节，直接return
    if (3 != fread(buf, 1, 3, h264file)) {
        free(buf);
        return 0;
    }
    
    if (!findStartCode0x0000001(buf)) { //如果不是以0x000001开始，继续找是不是以0x00000001开始
        if (1 != fread(buf+3, 1, 1, h264file)) {  //如果往后不够一个字节了，直接return
            free(buf);
            return 0;
        }
        if (!findStartCode0x000000001(buf)) {//如果也不是以0x00000001开始，说明这个nalu是废弃的
            free(buf);
            return -1;
        } else {  //说明是以0x00000001开始的nalu
            pos = 4;
            unit->startcodeprefix_len = 4;
        }
    } else { //说明是以0x000001开始的nalu
        pos = 3;
        unit->startcodeprefix_len = 3;
    }
    
    //如果成功跑到这里说明已经找到了合法的startcode。
    //下面开始找下一个startcode，为什么要找下一个startcode呢？因为下一个start是上一个的结束，找到了下一个startcode也就完成了整个nalu的分离。
    int netxstartcode_found = 0;
    int rewind = 0;  //后退的步伐，因为想要分离出完整的nalu，必须要先把后面的startcode也读出来，最后返回的时候要减去startcode占用的长度
    
    while (!netxstartcode_found) {  //如果没有找到下一个startcode，一直读取后面的字节
        if (feof(h264file)) {  //如果已经读到结尾了
            unit->len = (pos-1) - unit->startcodeprefix_len;
            memcpy(unit->buf, &buf[unit->startcodeprefix_len], unit->len);   //读取nalu的其它内容(也就是去掉startcode的其它内容)
            unit->forbidden_bit = buf[0]&0x80;  //占用1个bit
            unit->nalu_refrence_idc = buf[0]&0x60; //占用2bit
            unit->nalu_unit_type = buf[0]&0x1f;  //占用5个bit
            free(buf);
            return pos-1;
        }
        buf[pos++] = fgetc(h264file);

        if (findStartCode0x000000001(&buf[pos-4])) {  //要先看是不是满足4个字节的startcode，再看是不是三个字节
            netxstartcode_found = 1;
            rewind = -4;  //多读了4个字节，需要回退回去
        } else if (findStartCode0x0000001(&buf[pos-3])) {
            netxstartcode_found = 1;
            rewind = -3;  //多读了3个字节，需要回退回去
        }
    }
    
    if (0 != fseek(h264file, rewind, SEEK_CUR)) {  //回退回去
        free(buf);
        printf("error seek back");
    }
    
    unit->len = (pos+rewind)-unit->startcodeprefix_len;
    memcpy(unit->buf, &buf[unit->startcodeprefix_len], unit->len);
    unit->forbidden_bit = buf[0]&0x80;  //占用1个bit
    unit->nalu_refrence_idc = buf[0]&0x60; //占用2bit
    unit->nalu_unit_type = buf[0]&0x1f;  //占用5个bit
    free(buf);
    
    return pos+rewind;
}

int h264_prase(char *url) {
    NALU_t *unit;
    int buffersize = 100000;
    
    FILE *h264file = fopen(url, "rb+");
    if (h264file == NULL) {
        printf("open h264file failed");
        return 0;
    }
    
    unit = (NALU_t *)calloc(1, sizeof(NALU_t));
    if (unit == NULL) {
        free(unit);
        printf("alloc unit error");
        return 0;
    }
    unit->max_size = buffersize;
    unit->buf = (char *)calloc(buffersize, sizeof(char));
    int data_offset = 0;
    while (!feof(h264file)) {
        int data_length = splitNalu(h264file, unit);
        std::cout<<"unit type"<<unit->nalu_unit_type<<"  length  "<<unit->len<<"\n";
        data_offset += data_length;
    }
    
    if (unit) {
        if (unit->buf) {
            free(unit->buf);
        }
        free(unit);
    }
    return 1;
}
