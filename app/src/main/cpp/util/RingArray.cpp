//
// Created by ether on 2019/8/26.
//

#include "RingArray.h"

using namespace std;

template<typename T>
RingArray<T>::~RingArray() {
    readPos = 0;
    maxSize = 0;
    writePos = 0;
    delete dataArr;
}

template<typename T>
void RingArray<T>::read(T *dst, int size) {
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

template<typename T>
void RingArray<T>::write(T *dst, int size) {
    std::lock_guard<std::mutex> guard(mutex);
//    LOGE(RingArray_TAG, "%s:写入了%d字节", __func__, size);
//    fwrite(dst, size, 1, file);
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

template<typename T>
int RingArray<T>::getDataSize() const {
    std::lock_guard<std::mutex> guard(mutex);
    return dataSize;
}

template<typename T>
RingArray<T>::RingArray(int sampleRate, int channelCount) : dataArr(
        new T[sampleRate * channelCount * 2 * sizeof(int)]) {
    maxSize = sampleRate * channelCount * 2;
    std::string path = "/storage/emulated/0/Android/data/com.yetote.bamboomusic/files/test.pcm";
    file = fopen(path.c_str(), "wb+");
    LOGE(RingArray_TAG, "%s:数组大小%d", __func__, maxSize);
}

template<typename T>
bool RingArray<T>::canWrite(size_t size) {
    std::lock_guard<std::mutex> guard(mutex);
    return abs(maxSize - writePos + readPos) >= size;
}

