//
// Created by ether on 2019/8/26.
//

#ifndef FFMPEGANDOBOE_RINGARRAY_H
#define FFMPEGANDOBOE_RINGARRAY_H

#include <cstdint>
#include <cstring>
#include <cstdio>
#include "LogUtil.h"
#include <string>
#include <mutex>

#define RingArray_TAG "RingArray"
template <typename T>
class RingArray {
public:
    RingArray(int sampleRate, int channelCount);

    virtual ~RingArray();

    void read(T *dst, int size);

    void write(T *dst, int size);

    int getDataSize() const;
    bool canWrite(size_t size);
private:
    int readPos = 0;
    int dataSize = 0;
    int writePos = 0;
    int maxSize = 0;
    FILE *file;
    T *dataArr;
    std::mutex mutex;
};


#endif //FFMPEGANDOBOE_RINGARRAY_H
