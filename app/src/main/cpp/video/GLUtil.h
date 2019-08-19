//
// Created by ether on 2018/10/24.
//

#ifndef BAMBOO_GLUTIL_H
#define BAMBOO_GLUTIL_H

#include <GLES2/gl2.h>
#include <android/log.h>
#include <string>
#include "../util/LogUtil.h"
#define GLUtil_TAG "GLUtil"
class GLUtil {
public:
    GLUtil(const std::string& vertexCode, const std::string& fragCode);

    virtual ~GLUtil();

    GLuint program{};


    GLuint *createTexture();

private:
    void createProgram(const std::string& vertexCode, const std::string& fragCode);

    GLuint loadShader(GLenum type, const char *code);
};


#endif //BAMBOO_GLUTIL_H
