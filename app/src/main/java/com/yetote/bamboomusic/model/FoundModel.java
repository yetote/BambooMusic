package com.yetote.bamboomusic.model;

/**
 * @author yetote QQ:503779938
 * @name BambooMusic
 * @class name：com.yetote.bamboomusic.model
 * @class describe
 * @time 2019/8/15 13:16
 * @change
 * @chang time
 * @class describe
 */
public class FoundModel {
    private String path;
    private int praiseNum,discussNum;
    private String tag;

    public FoundModel(String path, int praiseNum, int discussNum, String tag) {
        this.path = path;
        this.praiseNum = praiseNum;
        this.discussNum = discussNum;
        this.tag = tag;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public int getPraiseNum() {
        return praiseNum;
    }

    public void setPraiseNum(int praiseNum) {
        this.praiseNum = praiseNum;
    }

    public int getDiscussNum() {
        return discussNum;
    }

    public void setDiscussNum(int discussNum) {
        this.discussNum = discussNum;
    }

    public String getTag() {
        return tag;
    }

    public void setTag(String tag) {
        this.tag = tag;
    }
}
