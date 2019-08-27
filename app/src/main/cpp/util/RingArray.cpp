//
// Created by ether on 2019/8/26.
//

#include "RingArray.h"

using namespace std;

RingArray::~RingArray() {

}

void RingArray::read(uint8_t *dst, int size) {
    if (maxSize - readPos >= size) {
        //可用数据足够，顺序读取
        memcpy(dst, dataArr + readPos, size * sizeof(uint8_t));
        readPos += size;
//        cout << "顺序读取,pos=" << readPos << endl;
    } else {
        //可用数据不够，循环读取
        auto remainingSize = maxSize - readPos;
        memcpy(dst, dataArr + readPos, remainingSize * sizeof(uint8_t));
        readPos = 0;
        memcpy(dst + remainingSize, dataArr + readPos, (size - remainingSize) * sizeof(uint8_t));
        readPos += (size - remainingSize);
//        cout << "循环读取,pos=" << readPos << endl;
    }

    dataSize -= size;
}

void RingArray::write(uint8_t *dst, int size) {
    if (maxSize - writePos >= size) {
        //容量够用，顺序存储
        memcpy(dataArr + writePos, dst, size * sizeof(uint8_t));
        writePos += size;
//        cout << "顺序写入,pos=" << writePos << endl;
    } else {
        //容量不够，循环存储
        auto remainingSize = maxSize - writePos;
        memcpy(dataArr + writePos, dst, remainingSize * sizeof(uint8_t));
        writePos = 0;
        memcpy(dataArr, dst + remainingSize, (size - remainingSize) * sizeof(uint8_t));
        writePos += size - remainingSize;
//        cout << "循环写入,pos=" << writePos << endl;
    }
    dataSize += size;
}

int RingArray::getDataSize() const {
    return dataSize;
}


RingArray::RingArray(int sampleRate, int channelCount) : dataArr(new uint8_t[sampleRate * channelCount * 2 * sizeof(int)]) {
    maxSize = sampleRate * channelCount * 2;
    LOGE(RingArray_TAG,"%s:数组大小%d",__func__,maxSize);
    int readPos = 0;
    int dataSize = 0;
    int writePos = 0;
}

