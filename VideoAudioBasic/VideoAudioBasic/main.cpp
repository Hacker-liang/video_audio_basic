//
//  main.cpp
//  VideoAudioBasic
//
//  Created by langren on 2019/5/28.
//  Copyright © 2019 Langren. All rights reserved.
//

#include <iostream>
#include "yuvprocessor.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
//    yuv420_split((char *)"helloyuv.yuv", 720, 960);
//    yuv420_gray((char *)"helloyuv.yuv", 720, 960);
//    yuv420_rgb((char *)"yuv2rgb500*500.yuv", 500, 500);
//    rgb24_yuv420((char *)"output_yuv2rgb.rgb", 720, 960);
    yuv420_rgb((char *)"output_rgb2_yuv1.yuv", 500, 500);
//    rgb24_yuv420((char *)"cie1931_500x500.rgb", 500, 500);

    return 0;
}
