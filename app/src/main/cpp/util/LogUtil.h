//
// Created by yetote on 2019/8/7.
//

#ifndef BAMBOOMUSIC_LOGUTIL_H
#define BAMBOOMUSIC_LOGUTIL_H


#endif //BAMBOOMUSIC_LOGUTIL_H

#include <android/log.h>

#define LOGE(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)