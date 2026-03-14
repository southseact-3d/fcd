#include <Base/Reader.h>
#include <Base/Writer.h>

#include "VisualLayer.h"

using namespace SketcherGui;

//**************** VisualClassConfig **************************************//

VisualLayer::VisualLayer(unsigned int linePattern, float lineWidth, bool visible)
    : linePattern(linePattern)
    , lineWidth(lineWidth)
    , visible(visible)
{}

unsigned int VisualLayer::getLinePattern() const
{
    return linePattern;
}

float VisualLayer::getLineWidth() const
{
    return lineWidth;
}

void VisualLayer::setLinePattern(unsigned int linepattern)
{
    linePattern = linepattern;
}

void VisualLayer::setLineWidth(float linewidth)
{
    lineWidth = linewidth;
}

bool VisualLayer::isVisible() const
{
    return visible;
}

void VisualLayer::setVisible(bool show)
{
    visible = show;
}

void VisualLayer::Save(Base::Writer& writer) const
{
    writer.Stream() << writer.ind() << "<VisualLayer "
                    << "visible=\"" << (visible ? std::string("true") : std::string("false")).c_str()
                    << "\" linePattern=\"" << linePattern << "\" lineWidth=\"" << lineWidth
                    << "\"/>" << std::endl;
}

void VisualLayer::Restore(Base::XMLReader& reader)
{
    reader.readElement("VisualLayer");

    std::string str = reader.getAttribute<const char*>("visible");
    visible = (str == "true");

    linePattern = reader.getAttribute<unsigned long>("linePattern");
    lineWidth = reader.getAttribute<double>("lineWidth");
}
