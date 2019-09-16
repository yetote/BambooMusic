//
// Created by ether on 2019/9/16.
//

#include "MediaInfo.h"

MediaInfo::MediaInfo(MediaInfo::MEDIA_TYPE _type) : type(_type) {

}

MediaInfo::~MediaInfo() {


}

void MediaInfo::destroy() {
    LOGE(MediaInfo_TAG, "%s:释放", __func__);
    if (pFmt != nullptr) {
        AMediaFormat_delete(pFmt);
        pFmt = nullptr;
    }

    if (codec != nullptr) {
        AMediaCodec_stop(codec);
        AMediaCodec_delete(codec);
        codec = nullptr;
    }
    if (extractor != nullptr) {
        AMediaExtractor_delete(extractor);
        extractor = nullptr;
    }
}
