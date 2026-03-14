#ifndef __texture_loader_h__
#define __texture_loader_h__
#include <string>
#include <vector>
#include <QImage>

namespace MillSim
{

struct TextureItem
{
    int tx {}, ty {};  // texture location
    int w {}, h {};    // item size
};

class TextureLoader
{
public:
    TextureLoader(std::string imgFolder, std::vector<std::string> fileNames, int textureSize);
    ~TextureLoader();
    unsigned int* GetRawData();
    TextureItem* GetTextureItem(int i);

protected:
    bool AddImage(TextureItem* guiItem, QImage& pixmap, unsigned int* buffPos, int stride);

protected:
    unsigned int* mRawData = nullptr;
    std::string mImageFolder;
};

}  // namespace MillSim
#endif  // !__texture_loader_h__
