//
//  yuvprocessor.hpp
//  VideoAudioBasic
//
//  Created by langren on 2019/5/28.
//  Copyright © 2019 Langren. All rights reserved.
//

#ifndef yuvprocessor_hpp
#define yuvprocessor_hpp

#include <stdio.h>

int yuv420_split(char *url, int w, int h);

int yuv420_gray(char *url, int w, int h);

int rgb24_yuv420(char *rgbUrl, int w, int h);

#endif /* yuvprocessor_hpp */
