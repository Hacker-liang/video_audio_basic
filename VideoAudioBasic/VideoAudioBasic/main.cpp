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
    rgb24_yuv420((char *)"test.rgb", 720, 960);
    return 0;
}
