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


template<typename T>
class RingArray {
public:
    RingArray(int sampleRate, int channelCount) : dataArr(
            new T[sampleRate * channelCount * 2]) {
        maxSize = sampleRate * channelCount * 2;
        LOGE(RingArray_TAG, "%s:数组大小%d", __func__, maxSize);
    }

    ~RingArray() {
        readPos = 0;
        maxSize = 0;
        writePos = 0;
        delete dataArr;
    }

    void read(T *dst, int size) {
        std::lock_guard<std::mutex> guard(mutex);
        if (maxSize - readPos >= size) {
            //可用数据足够，顺序读取
            memcpy(dst, dataArr + readPos, size * sizeof(T));
            readPos += size;
        } else {
            //可用数据不够，循环读取
            auto remainingSize = maxSize - readPos;
            memcpy(dst, dataArr + readPos, remainingSize * sizeof(T));
            readPos = 0;
            memcpy(dst + remainingSize, dataArr + readPos, (size - remainingSize) * sizeof(T));
            readPos += (size - remainingSize);
        }
        dataSize -= size;
    }

    void write(const T *dst, int size) {
        std::lock_guard<std::mutex> guard(mutex);
        if (maxSize - writePos >= size) {
            //容量够用，顺序存储
            memcpy(dataArr + writePos, dst, size * sizeof(T));
            writePos += size;
        } else {
            //容量不够，循环存储
            auto remainingSize = maxSize - writePos;
            memcpy(dataArr + writePos, dst, remainingSize * sizeof(T));
            writePos = 0;
            memcpy(dataArr, dst + remainingSize, (size - remainingSize) * sizeof(T));
            writePos += size - remainingSize;
        }
        dataSize += size;
    }

    int getDataSize() {
        std::lock_guard<std::mutex> guard(mutex);
        return dataSize;
    }

    bool canWrite(size_t size) {
        std::lock_guard<std::mutex> guard(mutex);
        if (writePos > readPos) {
            return maxSize - writePos + readPos >= size * sizeof(T);
        } else if (writePos < readPos) {
            return readPos - writePos >= size * sizeof(T);
        } else {
            if (dataSize == 0) {
                return maxSize;
            } else {
                return 0;
            }
        }
    }

    void clear() {
        memset(dataArr, 0, 200 * sizeof(T));
        readPos = 0;
        dataSize = 0;
        writePos = 0;
    }

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
