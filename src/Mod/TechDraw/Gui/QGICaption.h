#ifndef DRAWINGGUI_QGICAPTION_H
#define DRAWINGGUI_QGICAPTION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include "QGCustomText.h"
#include "QGIUserTypes.h"

namespace TechDrawGui
{

class TechDrawGuiExport QGICaption : public QGCustomText
{
public:
    explicit QGICaption();
    ~QGICaption() override {}

    enum {Type = UserType::QGICaption};
    int type() const override { return Type;}

};

}

#endif // DRAWINGGUI_QGICAPTION_H
