//
//  main.cpp
//  VideoAudioBasic
//
//  Created by langren on 2019/5/28.
//  Copyright © 2019 Langren. All rights reserved.
//

#include <iostream>
#include "color_space/yuvprocessor.hpp"
#include "audio/pcm_processor.hpp"
#include "video/h264_praser.hpp"
int main(int argc, const char * argv[]) {
    // insert code here...
//    yuv420_split((char *)"helloyuv.yuv", 720, 960);
//    yuv420_gray((char *)"helloyuv.yuv", 720, 960);
//    yuv420_rgb((char *)"yuv2rgb500*500.yuv", 500, 500);
//    rgb24_yuv420((char *)"output_yuv2rgb.rgb", 720, 960);
//    yuv420_rgb((char *)"output_rgb2_yuv1.yuv", 500, 500);
//    rgb24_yuv420((char *)"cie1931_500x500.rgb", 500, 500);
//    pcm16le_split((char *)"./audio/original_pcm.pcm");
    
//    pcm16le_2_pcm8((char *)"./audio/original_pcm.pcm");
    
//    char a = 'a';
    
//    std::cout<<(int)a;

//    pcm16_2_wave((char *)"./audio/original_pcm.pcm", 2, 44100);
    h264_prase((char *)"./video/h264.h264");
    return 0;
}


