//
// Created by ether on 2019/8/26.
//

#include "RingArray.h"
#include "Log.h"


RingArray::RingArray(int sampleRate, int channelCount) : dataArr(
        new uint8_t[sampleRate * channelCount * 2]) {
    maxSize = sampleRate * channelCount * 2;
    int readPos = 0;
    int dataSize = 0;
    int writePos = 0;
}

RingArray::~RingArray() {

}

void RingArray::read(uint8_t *dst, int size) {
    if (maxSize - readPos >= size) {
        //可用数据足够，顺序读取
        memcpy(dst + readPos, dataArr, size);
        readPos += size;
        LOGE(RingArray_TAG, "%s:顺序读取，索引=%d", __func__, readPos);
    } else {
        //可用数据不够，循环读取
        auto remainingSize = maxSize - readPos;
        memcpy(dst, dataArr + readPos, remainingSize);
        readPos = 0;
        memcpy(dst + remainingSize, dataArr + dataSize, size - remainingSize);
        readPos += remainingSize;
        LOGE(RingArray_TAG, "%s:循环读取，索引=%d", __func__, readPos);
    }
    dataSize -= size;
}

void RingArray::write(uint8_t *dst, int size) {
    if (maxSize - dataSize >= size) {
        //容量够用，顺序存储
        memcpy(dataArr + writePos, dst, size);
        LOGE(RingArray_TAG, "%s:顺序写入，索引=%d", __func__, writePos);
    } else {
        //容量不够，循环存储
        auto remainingSize = maxSize - writePos;
        memcpy(dataArr + writePos, dst, remainingSize);
        writePos = 0;
        memcpy(dataArr, dst + remainingSize, size - remainingSize);
        writePos += size - remainingSize;
        LOGE(RingArray_TAG, "%s:循环写入，索引=%d", __func__, writePos);
    }
    dataSize += size;
}

int RingArray::getDataSize() const {
    return dataSize;
}


