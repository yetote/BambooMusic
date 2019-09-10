//
// Created by ether on 2019/8/29.
//

#include "Test.h"

using namespace std;

int Test::removeDuplicates(std::vector<int> &nums) {
    int changePos = 0;
    for (int i = 1; i < nums.size() - 1; ++i) {
        if (nums[i] == nums[i - 1]) {
            changePos = i;
            continue;
        }
        nums[i] = nums[changePos];
        changePos = i;
    }

    return changePos;
}

int Test::singleNumber(std::vector<int> &nums) {
    int temp = 0;
    for (int i = 0; i < nums.size(); i++) {
        temp ^= nums[i];
    }
    return temp;
}
