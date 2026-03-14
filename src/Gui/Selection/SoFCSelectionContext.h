#ifndef GUI_SOFCSELECTIONCONTEXT_H
#define GUI_SOFCSELECTIONCONTEXT_H

#include <limits>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include <Inventor/SbColor.h>

#include <App/Material.h>

class SoState;

namespace Gui
{

class SoFCSelectionRoot;
struct SoFCSelectionContextBase;
using SoFCSelectionContextBasePtr = std::shared_ptr<SoFCSelectionContextBase>;

struct GuiExport SoFCSelectionContextBase
{
    virtual ~SoFCSelectionContextBase() = default;
    using MergeFunc = int(
        int status,
        SoFCSelectionContextBasePtr& output,
        SoFCSelectionContextBasePtr input,
        SoNode* node
    );
};

struct SoFCSelectionContext;
using SoFCSelectionContextPtr = std::shared_ptr<SoFCSelectionContext>;

struct GuiExport SoFCSelectionContext: SoFCSelectionContextBase
{
    int highlightIndex = -1;
    std::set<int> selectionIndex;
    SbColor selectionColor;
    SbColor highlightColor;
    std::shared_ptr<int> counter;

    ~SoFCSelectionContext() override;

    bool isSelected() const
    {
        return !selectionIndex.empty();
    }

    void selectAll()
    {
        selectionIndex.clear();
        selectionIndex.insert(-1);
    }

    bool isSelectAll() const
    {
        return !selectionIndex.empty() && *selectionIndex.begin() < 0;
    }

    bool isHighlighted() const
    {
        return highlightIndex >= 0;
    }

    bool isHighlightAll() const
    {
        return highlightIndex == std::numeric_limits<int>::max()
            && (selectionIndex.empty() || isSelectAll());
    }

    void highlightAll()
    {
        highlightIndex = std::numeric_limits<int>::max();
    }

    void removeHighlight()
    {
        highlightIndex = -1;
    }

    bool removeIndex(int index);
    bool checkGlobal(SoFCSelectionContextPtr ctx);

    virtual SoFCSelectionContextBasePtr copy()
    {
        return std::make_shared<SoFCSelectionContext>(*this);
    }

    static MergeFunc merge;
};

struct SoFCSelectionContextEx;
using SoFCSelectionContextExPtr = std::shared_ptr<SoFCSelectionContextEx>;

struct GuiExport SoFCSelectionContextEx: SoFCSelectionContext
{
    std::map<int, Base::Color> colors;
    float trans0 = 0.0;

    bool setColors(const std::map<std::string, Base::Color>& colors, const std::string& element);
    uint32_t packColor(const Base::Color& c, bool& hasTransparency);
    bool applyColor(int idx, std::vector<uint32_t>& packedColors, bool& hasTransparency);
    bool isSingleColor(uint32_t& color, bool& hasTransparency);

    SoFCSelectionContextBasePtr copy() override
    {
        return std::make_shared<SoFCSelectionContextEx>(*this);
    }

    static MergeFunc merge;
};

class SoHighlightElementAction;
class SoSelectionElementAction;

class GuiExport SoFCSelectionCounter
{
public:
    SoFCSelectionCounter();
    virtual ~SoFCSelectionCounter();
    bool checkRenderCache(SoState* state);
    void checkAction(SoHighlightElementAction* hlaction);
    void checkAction(SoSelectionElementAction* selaction, SoFCSelectionContextPtr ctx);

protected:
    std::shared_ptr<int> counter;
    bool hasSelection {false};
    bool hasPreselection {false};
    static int cachingMode;
};

}  // namespace Gui
#endif  // GUI_SOFCSELECTIONCONTEXT_H
