#ifndef GUI_INVENTOR_MARKERBITMAPS_H
#define GUI_INVENTOR_MARKERBITMAPS_H

#include <list>
#include <map>
#include <string>
#include <FCGlobal.h>


namespace Gui
{
namespace Inventor
{

class GuiExport MarkerBitmaps
{

public:
    static void initClass();
    static int getMarkerIndex(const std::string&, int px);
    static std::list<int> getSupportedSizes(const std::string&);

private:
    static void createBitmap(const std::string&, int px, int width, int height, const char* marker);

private:
    using Marker = std::pair<std::string, int>;
    static std::map<Marker, int> markerIndex;  // NOLINT
};

}  // namespace Inventor

}  // namespace Gui

#endif  // GUI_INVENTOR_MARKERBITMAPS_H
