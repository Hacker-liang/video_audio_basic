////
////  rgb_yuv_convertor.cpp
////  VideoAudioBasic
////
////  Created by langren on 2019/5/29.
////  Copyright © 2019 Langren. All rights reserved.
////
//
//#include "rgb_yuv_convertor.hpp"
//
//void yuv2rgb(int y, int u, int v, int* r, int* g, int* b)
//{
//    switch (m_mType)
//    {
//        case SYMatrix_normal:
//        {
//            switch (m_cType)
//            {
//                case SYConvert_normal: // 常规方法：浮点运算，精度高
//                {
//                    *r = y                  + (1.370705 * v);
//                    *g = y - (0.337633 * u) - (0.698001 * v);
//                    *b = y + (1.732446 * u);
//                }
//                    break;
//                    
//                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
//                {
//                    *r = ((256 * y             + (351 * v))>>8);
//                    *g = ((256 * y - (86  * u) - (179 * v))>>8);
//                    *b = ((256 * y + (444 * u))            >>8);
//                }
//                    break;
//                    
//                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
//                {
//                    *r = (((y<<8) + (v<<8) + (v<<6) + (v<<4) + (v<<3) + (v<<2) + (v<<1) + v)                   >> 8);
//                    *g = (((y<<8) - (u<<6) - (u<<4) - (u<<2) - (u<<1) - (v<<7) - (v<<5) - (v<<4) - (v<<1) - v) >> 8);
//                    *b = (((y<<8) + (u<<8) + (u<<7) + (u<<5) + (u<<4) + (u<<3) + (u<<2))                       >> 8);
//                }
//                    break;
//                    
//                case SYConvert_table: // 查表法（也是位移计算），精度低
//                {
//                    *r = y + m_rv[v];
//                    *g = y - m_gu[u] - m_gv[v];
//                    *b = y + m_bu[u];
//                }
//                    break;
//                    
//                default:
//                    break;
//            }
//        }
//            break;
//            
//        case SYMatrix_bt_601:
//        {
//            switch (m_cType)
//            {
//                case SYConvert_normal: // 常规方法：浮点运算，精度高
//                {
//                    *r = (y               + 1.13983 * v);
//                    *g = (y - 0.39465 * u - 0.5806  * v);
//                    *b = (y + 2.03211 * u);
//                }
//                    break;
//                    
//                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
//                {
//                    *r = ((256 * y           + 292 * v)>>8);
//                    *g = ((256 * y - 101 * u - 149 * v)>>8);
//                    *b = ((256 * y + 520 * u)          >>8);
//                }
//                    break;
//                    
//                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
//                {
//                    *r = (((y<<8) + (v<<8) + (v<<5) + (v<<2))                                   >>8);
//                    *g = (((y<<8) + (u<<6) + (u<<5) + (u<<2) + u + (v<<7) + (v<<4) + (v<<2) + v)>>8);
//                    *b = (((y<<8) + (u<<9) + (u<<3))                                            >>8);
//                }
//                    break;
//                    
//                case SYConvert_table: // 查表法（也是位移计算），精度低
//                {
//                    *r = y + m_rv[v];
//                    *g = y - m_gu[u] - m_gv[v];
//                    *b = y + m_bu[u];
//                }
//                    break;
//                    
//                default:
//                    break;
//            }
//        }
//            break;
//            
//        case SYMatrix_bt_709:
//        {
//            switch (m_cType)
//            {
//                case SYConvert_normal: // 常规方法：浮点运算，精度高
//                {
//                    *r = (y               + 1.28033 * v);
//                    *g = (y - 0.21482 * u - 0.38059 * v);
//                    *b = (y + 2.12798 * u);
//                }
//                    break;
//                    
//                case SYConvert_bitMult:  // 通过位移来避免浮点运算，精度低
//                {
//                    *r = ((256 * y           + 328 * v)>>8);
//                    *g = ((256 * y - 55  * u - 97  * v)>>8);
//                    *b = ((256 * y + 545 * u)          >>8);
//                }
//                    break;
//                    
//                case SYConvert_bitAdd:   // 通过位移来避免乘法运算，精度低
//                {
//                    *r = (((y<<8) + (v<<8) + (v<<6) + (v<<3))                                   >>8);
//                    *g = (((y<<8) + (u<<5) + (u<<4) + (u<<2) + (u<<1) + u + (v<<6) + (v<<5) + v)>>8);
//                    *b = (((y<<8) + (u<<9) + (u<<5) +u)                                         >>8);
//                }
//                    break;
//                    
//                case SYConvert_table: // 查表法（也是位移计算），精度低
//                {
//                    *r = y + m_rv[v];
//                    *g = y - m_gu[u] - m_gv[v];
//                    *b = y + m_bu[u];
//                }
//                    break;
//                    
//                default:
//                    break;
//            }
//        }
//            break;
//            
//        default:
//            break;
//    }
//    *r = CLAMP(*r, 0, 255);
//    *g = CLAMP(*g, 0, 255);
//    *b = CLAMP(*b, 0, 255);
//}
