//
// Created by yetote on 2019/8/16.
//

#ifndef BAMBOOMUSIC_BLOCKQUEUE_H
#define BAMBOOMUSIC_BLOCKQUEUE_H


#include "../util/LogUtil.h"
#include "../util/PlayStates.h"

#include <queue>
#include <mutex>

#define BlockQueue_TAG "BlockQueue"
extern "C" {
#include <libavcodec/avcodec.h>
}


class BlockQueue {
public:

    void push(AVPacket *packet);

    void init();

    bool pop(AVPacket *packet1);

    void clear();

    void setMaxSize(int size);

    BlockQueue(int maxSize);

    virtual ~BlockQueue();

    std::queue<AVPacket *> queue;
private:
    AVPacket *packet;
    std::condition_variable cond;
    std::mutex mutex;
    int maxSize = 0;

    void stop();
};


#endif //BAMBOOMUSIC_BLOCKQUEUE_H
