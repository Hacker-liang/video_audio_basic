//
//  pcm_processor.hpp
//  VideoAudioBasic
//
//  Created by langren on 2019/5/30.
//  Copyright © 2019 Langren. All rights reserved.
//

#ifndef pcm_processor_hpp
#define pcm_processor_hpp

#include <stdio.h>

int pcm16le_split(char *pcmFile);

int pcm16le_2_pcm8(char *pcmFile);

int pcm16_2_wave(char *pcmFile, int channels, int sample_rate);

#endif /* pcm_processor_hpp */
