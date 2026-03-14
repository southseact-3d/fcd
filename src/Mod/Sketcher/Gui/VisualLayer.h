#ifndef SKETCHERGUI_VisualLayer_H
#define SKETCHERGUI_VisualLayer_H

#include <map>
#include <memory>

namespace Base
{
class Writer;
class XMLReader;
}  // namespace Base

namespace SketcherGui
{

/** Provides the visual layer configuration for a class of geometry.
 *  A class of geometry can be any grouping of geometry, for
 * which the user wants to provide a per coin layer specific
 * configuration.
 */
class VisualLayer
{
public:
    explicit VisualLayer(unsigned int linePattern = 0xFFFF, float lineWidth = 3.0, bool visible = true);

    unsigned int getLinePattern() const;
    float getLineWidth() const;

    void setLinePattern(unsigned int linepattern);
    void setLineWidth(float linewidth);

    bool isVisible() const;
    void setVisible(bool show);

    void Save(Base::Writer& /*writer*/) const;
    void Restore(Base::XMLReader& /*reader*/);

private:
    unsigned int linePattern;
    float lineWidth;
    bool visible;

    friend inline bool operator==(VisualLayer const& lhs, VisualLayer const& rhs);
};

bool operator==(VisualLayer const& lhs, VisualLayer const& rhs)
{
    return (lhs.linePattern == rhs.linePattern) && (lhs.lineWidth == rhs.lineWidth)
        && (lhs.visible == rhs.visible);
}

}  // namespace SketcherGui


#endif  // SKETCHERGUI_VisualLayer_H
