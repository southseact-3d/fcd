#ifndef __texture_h__
#define __texture_h__
#include "OpenGlWrapper.h"

namespace MillSim
{

class Texture
{
public:
    Texture()
    {}
    ~Texture();
    void DestroyTexture();
    bool LoadImage(unsigned int* image, int x, int y);
    bool Activate();
    bool unbind();
    float getTexX(int imgX)
    {
        return (float)imgX / (float)width;
    }
    float getTexY(int imgY)
    {
        return (float)imgY / (float)height;
    }

public:
    int width = 0;
    int height = 0;


protected:
    unsigned int mTextureId = 0;
};


}  // namespace MillSim

#endif  // !__texture_h__
