//
//  pcm_processor.cpp
//  VideoAudioBasic
//
//  Created by langren on 2019/5/30.
//  Copyright © 2019 Langren. All rights reserved.
//

#include "pcm_processor.hpp"
#include <iostream>

//将pcm双通道分割成左右通道
int pcm16le_split(char *pcmFile) {
    FILE *fp = fopen(pcmFile, "rb+");
    FILE *leftFp = fopen("./audio/output_left_pcm.pcm", "wb+");
    FILE *rightFp = fopen("./audio/output_right_pcm.pcm", "wb+");
    
    //每个通道的每个采样点需要16bit存储，所以此处分配2*2个字节
    unsigned char *sample = (unsigned char *)malloc(4);
    while (!feof(fp)) {
        fread(sample, 1, 4, fp);
        //左右通道数据交叉存放
        fwrite(sample, 1, 2, leftFp);
        fwrite(sample+2, 1, 2, rightFp);
    }
    free(sample);
    fclose(fp);
    fclose(leftFp);
    fclose(rightFp);
    return 0;
}

//将pcm16数据转换为pcm8格式数据
int pcm16le_2_pcm8(char *pcmFile) {
    FILE *fp = fopen(pcmFile, "rb+");
    FILE *outputFp = fopen("./audio/output_pcm8.pcm", "wb+");
    //每个通道的每个采样点需要16bit存储，所以此处分配2*2个字节
    unsigned char *sample = (unsigned char *)malloc(4);
    while (!feof(fp)) {
        fread(sample, 1, 4, fp);
        
        //将sample的前两个字节存放到sample16指向的内存里（c++中short占用两个字节，char占用一个字节）
        //pcm16存储的是short类型
        short *sample16 = (short *)sample;
        //sample8的取值范围是-128到127
        char sample8 = (*sample16)>>8;
        
        //转为无符号char类型，因为pcm8里存放的是无符号char类型
        unsigned char sample_u8 = sample8+128;
        //将最终转换的 unsigned char数据存到文件里（左声道）
        fwrite(&sample_u8, 1, 1, outputFp);
        
        //同理存储右声道
        sample16 = (short *)(sample+2);
        //sample8的取值范围是-128到127
        sample8 = (*sample16)>>8;
        //转为无符号char类型，因为pcm8里存放的是无符号char类型
        sample_u8 = sample8+128;
        //将最终转换的 unsigned char数据存到文件里（右声道）
        fwrite(&sample_u8, 1, 1, outputFp);
    }
    free(sample);
    fclose(fp);
    fclose(outputFp);
    
    return 0;
}

//将PCM16LE双声道音频采样数据转换为WAVE格式音频数据,wave音频格式的后缀是.wav
int pcm16_2_wave(char *pcmFile, int channels, int sample_rate) {
    
    typedef struct WAVE_HEADER {
        char fccID[4];   // must be "RIFF"
        
        //存储文件的总大小，但是不包含自身结构体里的其他两个变量（共8个字节）
        //这里雷神以及很多其他的blog都写的是44+sizeof(pcm),其实应该是36+sizeof(pcm)
        //参考wave格式的官方文档：
        /*36 + SubChunk2Size, or more precisely:
         4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
         This is the size of the rest of the chunk
         following this number.  This is the size of the
         entire file in bytes minus 8 bytes for the
         two fields not included in this count:
         ChunkID and ChunkSize.*/
        unsigned long dwSize;
        
        char fccType[4];   // must be "WAVE"
    }WAVE_HEADER;
    
    typedef struct WAVE_FMT {
        char fccID[4]; //must be "fmt "注意最后有个空格
        unsigned long dwSize;  //数值为16或18，18则最后有附加信息
        unsigned short wFormatTag;  // 1: linear,6: a law,7:u-law
        unsigned short wChannels;  //声道数量
        
        //声明的数据类型unsigned long在有些C编译器上是64位的，这时候要改成unsigned int才可以
        unsigned long dwSamplesPerSec; //采样率
        unsigned long dwAvgBytesPerSec; //每个采样的内存大小（Byte）dwSamplesPerSec*pcm数据格式占用的字节(sizeof(short) or sizeof(char))*wChannels
        unsigned short wBlockAlign; //每个采样需要的字节数: pcm数据格式占用的字节(sizeof(short) or sizeof(char))*wChannels
        unsigned short uiBitsPerSample; // 8 bits = 8, 16 bits = 16, etc.
    } WAVE_FMT;
    
    typedef struct WAVE_DATA {
        char fccID[4]; // must be "fact"
        unsigned long dwSize;  //pcm数据的大小
    } WAVE_DATA;
    
    if (sample_rate <= 0) {
        sample_rate = 44100;
    }
    if (channels <= 0) {
        channels = 2;
    }
    
    WAVE_HEADER wave_header;
    WAVE_FMT wave_fmt;
    WAVE_DATA wave_data;
    
    //用来存储读取的pcm值，这里是pcm16，
    unsigned   short   m_pcmData;

    FILE *fp = fopen(pcmFile, "rb+");
    FILE *waveFp = fopen("./audio/output_wavfmt.wav", "wb+");
    
    memcpy(wave_header.fccID, "RIFF", strlen("RIFF"));
    memcpy(wave_header.fccType, "WAVE", strlen("WAVE"));
    
    //因为WAVE_HEADER里的dwSize这个值需要后续计算才能确定，所以先将文件指针seek到后面，先写WAVE_FMT
    fseek(waveFp, sizeof(WAVE_HEADER), 1);
    
    wave_fmt.dwSamplesPerSec = sample_rate;
    wave_fmt.dwAvgBytesPerSec = sample_rate * sizeof(m_pcmData) * channels;
    wave_fmt.uiBitsPerSample = 16;  //因为是pcm16格式
    memcpy(wave_fmt.fccID, "fmt ", strlen("fmt "));   //注意有空格
    wave_fmt.dwSize = 16;
    wave_fmt.wBlockAlign = 2;
    wave_fmt.wChannels = channels;
    wave_fmt.wFormatTag = 1;
    //先将format信息写入文件里
    fwrite(&wave_fmt, sizeof(WAVE_FMT), 1, waveFp);
    
    memcpy(wave_data.fccID, "data", strlen("data"));
    //先置位0，待会读pcm文件的时候再累加
    wave_data.dwSize = 0;
    
    //wave_header这个头也先不存，先存后面的pcm数据,所以先将文件指针seek到后面
    fseek(waveFp, sizeof(WAVE_DATA), SEEK_CUR);
    
    //开始读取pcm数据
    fread(&m_pcmData, sizeof(unsigned short), 1, fp);
    
    while (!feof(fp)) {
        wave_data.dwSize += 2;
        fwrite(&m_pcmData, sizeof(unsigned short), 1, waveFp);
        fread(&m_pcmData, sizeof(unsigned short), 1, fp);
    }
    
    wave_header.dwSize = 44-8+wave_data.dwSize;
    
    //重新回到文件开始
    rewind(waveFp);
    //写第一个header
    fwrite(&wave_header, sizeof(WAVE_HEADER), 1, waveFp);
    //跳过第二header
    fseek(waveFp, sizeof(WAVE_FMT), SEEK_CUR);
    //写第三个header
    fwrite(&wave_data, sizeof(WAVE_HEADER), 1, waveFp);
    fclose(fp);
    fclose(waveFp);
    return 0;
}
