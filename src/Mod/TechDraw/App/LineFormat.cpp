#include "LineGroup.h"
#include "LineGenerator.h"
#include "Preferences.h"

#include "LineFormat.h"

using namespace TechDraw;

//! general purpose line format specifier

LineFormat::LineFormat()
{
    m_style = getDefEdgeStyle();
    m_weight = getDefEdgeWidth();
    m_color= getDefEdgeColor();
    m_visible = true;
    m_lineNumber = LineGenerator::fromQtStyle((Qt::PenStyle)m_style);
}

// static loader of default format
void LineFormat::initCurrentLineFormat()
{
    getCurrentLineFormat().setStyle(getDefEdgeStyle());
    getCurrentLineFormat().setWidth(getDefEdgeWidth());
    getCurrentLineFormat().setColor(getDefEdgeColor());
    getCurrentLineFormat().setVisible(true);
    getCurrentLineFormat().setLineNumber(LineGenerator::fromQtStyle((Qt::PenStyle)getCurrentLineFormat().getStyle()));
}

LineFormat& LineFormat::getCurrentLineFormat()
{
    static TechDraw::LineFormat currentLineFormat;
    return currentLineFormat;
}

void LineFormat::setCurrentLineFormat(LineFormat& newFormat)
{
    getCurrentLineFormat().setStyle(newFormat.getStyle());
    getCurrentLineFormat().setWidth(newFormat.getWidth());
    getCurrentLineFormat().setColor(newFormat.getColor());
    getCurrentLineFormat().setVisible(newFormat.getVisible());
    getCurrentLineFormat().setLineNumber(newFormat.getLineNumber());
}

LineFormat::LineFormat(const int style,
                       const double weight,
                       const Base::Color& color,
                       const bool visible) :
    m_style(style),
    m_weight(weight),
    m_color(color),
    m_visible(visible),
    m_lineNumber(LineGenerator::fromQtStyle((Qt::PenStyle)m_style))
{
}

void LineFormat::dump(const char* title)
{
    Base::Console().message("LF::dump - %s \n", title);
    Base::Console().message("LF::dump - %s \n", toString().c_str());
}

std::string LineFormat::toString() const
{
    std::stringstream ss;
    ss << m_style << ", " <<
          m_weight << ", " <<
          m_color.asHexString() << ", " <<
          m_visible;
    return ss.str();
}

//static preference getters.
double LineFormat::getDefEdgeWidth()
{
    return TechDraw::LineGroup::getDefaultWidth("Graphic");
}

Base::Color LineFormat::getDefEdgeColor()
{
    return Preferences::normalColor();
}

int LineFormat::getDefEdgeStyle()
{
    return Preferences::getPreferenceGroup("Decorations")->GetInt("CenterLineStyle", 2);   //dashed
}


