//
// Created by ether on 2019/8/13.
//

#include "PlayStates.h"

PlayStates::PlayStates() {
    isEOF = false;
    isPause = false;
    isStop = false;
    LOGE(PlayStates_TAG, "%s:初始化", __func__);
}
