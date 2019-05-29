//
//  yuvprocessor.cpp
//  VideoAudioBasic
//
//  Created by langren on 2019/5/28.
//  Copyright © 2019 Langren. All rights reserved.
//

#include "yuvprocessor.hpp"
#include <iostream>

//分离yuv420像素数据的y，u，v分量
int yuv420_split(char *url, int w, int h) {
    FILE *fp = fopen(url, "rb+");
    FILE *y_fp = fopen("output_420_y.y", "wb+");
    FILE *u_fp = fopen("output_420_u.y", "wb+");
    FILE *v_fp = fopen("output_420_v.y", "wb+");

    //yuv420格式的图片采样是4:2:0，每个分量需要8bit（1个字节），每个像素需要1个Y分量，1/4个U分量，1/4个V分量
    //所以这里分配了w*h*3/2个字节
    unsigned char *pic = (unsigned char *)malloc(w*h*3/2);
    //读取yuv图片数据
    fread(pic, 1, w*h*3/2, fp);
    
    //将y分量写入文件里，这里可以看出Y分量全部存储在前w*h个字节里。
    //也就是说(w*h到w*h*3/2)这个存储区间里存放了UV分量的数据。
    fwrite(pic, 1, w*h, y_fp);
    
    //从w*h这个位置到w*h*5/4存储这个U分量，占了w*h*1/4个字节
    fwrite(pic+w*h, 1, w*h/4, u_fp);
    
    //从w*h*5/4到w*h*6/4这个位置到存储这个U分量，占了w*h*1/4个字节
    fwrite(pic+w*h+w*h*1/4, 1, w*h/4, v_fp);
    
    free(pic);
    fclose(fp);
    fclose(y_fp);
    fclose(u_fp);
    fclose(v_fp);
    
    return 0;
}

//将yuv图片去掉颜色值，变成灰度图
int yuv420_gray(char *url, int w, int h) {
    FILE *fp = fopen(url, "rb+");
    FILE *fp1 = fopen("output_gray.yuv", "wb+");
    unsigned char *pic = (unsigned char *)malloc(w*h*3/2);
    fread(pic, 1, w*h*3/2, fp);
    //此处为啥要写128？？？？
    //because：色度分量的取值范围是：-128到127，0代表无颜色，但是存色度分量的时候我们要进行偏移处理，
    //把-128到127映射到0-255这个区间，因此128代表没有颜色。
    memset(pic+w*h, 128, w*h/2);
    fwrite(pic, 1, w*h*3/2, fp1);
    return 0;
}

unsigned char clip_value(unsigned char x, unsigned char min, unsigned char max) {
    if (x>max) {
        return max;
    }
    if (x<min) {
        return min;
    }
    return x;
}

int rgb24_yuv420(char *rgbUrl, int w, int h) {
    
    FILE *fp = fopen(rgbUrl, "rb+");
    FILE *yuvFp = fopen("output_rgb2_yuv1.yuv", "wb+");
    unsigned char *yuvBuf = (unsigned char *)malloc(w*h*3/2);
    unsigned char *rgbBuf = (unsigned char *)malloc(w*h*3);
    
    fread(rgbBuf, 1, w*h*3, fp);
    memset(yuvBuf, 0, w*h*3/2);

    unsigned char *ptrY, *ptrU, *ptrV, *ptrRGB;
    
    ptrY = yuvBuf;
    ptrU = yuvBuf + w*h;
    ptrV = ptrU + w*h*1/4;
    unsigned char y, u, v, r, g, b;
    for (int j=0; j<h; j++) {
        ptrRGB = rgbBuf+w*j*3;  //RGB24每个像素需要3个字节
        for (int i = 0; i<w; i++) {
            //分别获取rgb三个通道颜色数据
            r = *(ptrRGB++);
            g = *(ptrRGB++);
            b = *(ptrRGB++);

            /*在 Keith Jack’s 的书 “Video Demystified” (ISBN 1-878707-09-4) 给出的公式是这样的。
            int Y1 = 0.257*r + 0.504*g + 0.098*b + 16;
            int U1 = -0.148*r - 0.291*g + 0.439*b + 128;
            int V1 = 0.439*r - 0.368*g - 0.071*b + 128;
             */
            
            //根据转换矩阵，转换成y，u，v
            y = (unsigned char)((66 * r + 129 * g +  25 * b + 128) >> 8) + 16;
            *(ptrY++) = clip_value(y, 0, 255);
            
            if (j%2==0 && i%2==0) {  //yuv420对应的是每2x2个矩阵对应一个uv分量。
                u = (unsigned char)((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
                *(ptrU++) = clip_value(u, 0, 255);
                v = (unsigned char)((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;
                *(ptrV++) = clip_value(v, 0, 255);
            }
        }
    }
    fwrite(yuvBuf, 1, w*h*3/2, yuvFp);
    free(yuvBuf);
    free(rgbBuf);
    fclose(fp);
    fclose(yuvFp);
    return 0;
}


//YUV420P （yyyyyyyyyyyyyuuuuuuvvvvvv）排列的yuv420图片转换成rgb24格式
int yuv420_rgb(char *yuvUrl, int w, int h) {
    FILE *fp = fopen(yuvUrl, "rb+");
    FILE *rgbFp = fopen("output_yuv2rgb.rgb", "wb+");
    
    unsigned char *yuvBuffer = (unsigned char *)malloc(w*h*3/2);
    unsigned char *rgbBuffer = (unsigned char *)malloc(w*h*3);
    
    fread(yuvBuffer, 1, w*h*3/2, fp);
    memset(rgbBuffer, 0, w*h*3);
    
    unsigned char *ptrRGB = rgbBuffer;
    unsigned char *ptrY, *ptrU, *ptrV;  //y u v三个通量指针地址
    int y, u, v, r, g, b;

    ptrY = yuvBuffer;
    ptrU = yuvBuffer + w*h;
    ptrV = ptrU + ((w*h)>>2);

    for (int j=0; j<h; j++) {
        for (int i=0; i<w; i++) {            
            y = (int)ptrY[j * w + i];
            
            //每2*2个矩阵共用一个u和v
            u = (int)(ptrU[(j>>1) * (w>>1) + (i>>1)] - 128);
            v = (int)(ptrV[(j>>1) * (w>>1) + (i>>1)] - 128);
    
            r = ((256 * y + (351 * v))>>8);
            g = ((256 * y - (86  * u) - (179 * v))>>8);
            b = ((256 * y + (444 * u))>>8);

            *(ptrRGB++) = (char)r;
            *(ptrRGB++) = (char)g;
            *(ptrRGB++) = (char)b;
        }
    }
    fwrite(rgbBuffer, 1, w*h*3, rgbFp);
    free(rgbBuffer);
    free(yuvBuffer);
    fclose(fp);
    fclose(rgbFp);
    return 0;
}
