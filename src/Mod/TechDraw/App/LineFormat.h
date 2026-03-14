#ifndef TECHDRAW_LINEFORMAT_H
#define TECHDRAW_LINEFORMAT_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <QColor>

#include <Base/Color.h>


namespace TechDraw {

//! general purpose line format specifier
class TechDrawExport LineFormat
{
public:
    static constexpr size_t InvalidLine{0};

    LineFormat();
    LineFormat(const int style,
               const double weight,
               const Base::Color& color,
               const bool visible,
               const int lineNumber);
   // TODO: phase out the old 4 parameter constructor
   LineFormat(const int style,
               const double weight,
               const Base::Color& color,
               const bool visible);
    ~LineFormat() = default;

    // style was used to specify QPen styles.  line number (from LineGenerator) should be used now.
    int getStyle() const { return m_style; }
    void setStyle(int style) { m_style = style; }

    double getWidth() const { return m_weight; }
    void setWidth(double width) {m_weight = width; }

    Base::Color getColor() const { return m_color; }
    void setColor(Base::Color color) { m_color = color; }
    QColor getQColor() const { return m_color.asValue<QColor>(); }
    void setQColor(QColor qColor) { m_color.set(qColor.redF(), qColor.greenF(), qColor.blueF(), 1.0 - qColor.alphaF()); }

    bool getVisible() const { return m_visible; }
    void setVisible(bool viz) { m_visible = viz; }

    int getLineNumber() const { return m_lineNumber; }
    void setLineNumber(int number) { m_lineNumber = number; }

    static double getDefEdgeWidth();
    static Base::Color getDefEdgeColor();
    static int getDefEdgeStyle();

    void dump(const char* title);
    std::string toString() const;

    static void initCurrentLineFormat();
    static LineFormat& getCurrentLineFormat();
    static void setCurrentLineFormat(LineFormat& newformat);

private:
    int m_style;
    double m_weight;
    Base::Color m_color;
    bool m_visible;
    int m_lineNumber {1};
};

} //end namespace TechDraw

#endif //TECHDRAW_LINEFORMAT_H

