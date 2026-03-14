#include "GlUtils.h"
#include <iostream>

namespace MillSim
{

const mat4x4 identityMat = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool GLLogError()
{
    bool isError = false;
    while (GLenum err = glGetError()) {
        std::cout << "[Opengl Error] (" << err << ")" << std::endl;
        isError = true;
    }
    return isError;
}

}  // namespace MillSim
