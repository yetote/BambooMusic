//
// Created by ether on 2019/8/15.
//

#ifndef BAMBOOMUSIC_GLUTIL_H
#define BAMBOOMUSIC_GLUTIL_H

#include <GLES2/gl2.h>
#include "../util/LogUtil.h"
#include <string>

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


#endif //BAMBOOMUSIC_GLUTIL_H
