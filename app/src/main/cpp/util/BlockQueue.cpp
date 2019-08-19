//
// Created by yetote on 2019/8/16.
//

#include "BlockQueue.h"

void BlockQueue::push(AVPacket *packet) {
    std::unique_lock<decltype(mutex)> lock(mutex);
//    while (queue.size() >= 10) {
//        LOGE("blockQueue", "队列已满，阻塞中：%d", queue.size());
//        cond.wait(lock);
//    }
    AVPacket *temp = av_packet_alloc();
    av_packet_ref(temp, packet);
    queue.push(temp);
    LOGE("blockQueue", "packet入队，队列容量:%d", queue.size());
    LOGE("blockQueue", "packet入队，地址为:%p", this->packet);

    cond.notify_all();
}

bool BlockQueue::pop(AVPacket *packet1) {
    std::unique_lock<decltype(mutex)> lock(mutex);
//    mutex.lock();
    if (queue.empty()) {
//        LOGE("blockQueue", "播放完成");
        cond.notify_all();
        return true;
    } else if (queue.empty()) {
        LOGE("blockQueue", "the messagequeue is empty ,waiting producer add message ");
        cond.wait(lock);
    } else {
        int rst = av_packet_ref(packet1, queue.front());
        if (rst == 0) {
            LOGE("blockQueue", "packet出队，队列剩余:%d", queue.size());
            LOGE("blockQueue", "packet出队，地址为:%p,\npacket1=%p", queue.front(), packet1);
            queue.pop();
        } else {
            LOGE(BlockQueue_TAG, "line in 35:复制packet失败%d", rst);
        }
    }
    cond.notify_all();
    return false;
}

void BlockQueue::init() {
}

void BlockQueue::stop() {
    av_packet_free(&packet);
}

void BlockQueue::clear() {
    std::unique_lock<decltype(mutex)> lock(mutex);
    for (;;) {
        if (!queue.empty()) {
            queue.pop();
        } else {
            break;
        }
    }
    cond.notify_all();
}

void BlockQueue::setMaxSize(int size) {
    maxSize = size;
}

BlockQueue::BlockQueue(int maxSize) : maxSize(maxSize) {
    packet = av_packet_alloc();
}

BlockQueue::~BlockQueue() {

}
