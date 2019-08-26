//
// Created by ether on 2019/8/26.
//

#ifndef FFMPEGANDOBOE_RINGARRAY_H
#define FFMPEGANDOBOE_RINGARRAY_H

#include <cstdint>
#include <cstring>
#define RingArray_TAG "RingArray"

class RingArray {
public:
    RingArray(int sampleRate, int channelCount);

    virtual ~RingArray();

    void read(uint8_t *dst, int size);

    void write(uint8_t *dst, int size);

    int getDataSize() const;

private:
    int readPos = 0;
    int dataSize = 0;
    int writePos = 0;
    int maxSize = 0;
    uint8_t *dataArr;
};


#endif //FFMPEGANDOBOE_RINGARRAY_H
