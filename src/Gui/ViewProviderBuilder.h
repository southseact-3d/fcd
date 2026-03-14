#ifndef GUI_VIEWPROVIDERBUILDER_H
#define GUI_VIEWPROVIDERBUILDER_H

#include <map>
#include <vector>
#include <Base/Type.h>


class SoNode;

namespace App
{
class Property;
}

namespace Gui
{

class ViewProvider;
class SoFCSelection;
class GuiExport ViewProviderBuilder
{
public:
    /// Constructor
    ViewProviderBuilder();
    virtual ~ViewProviderBuilder();
    virtual void buildNodes(const App::Property*, std::vector<SoNode*>&) const = 0;

    static void add(const Base::Type&, const Base::Type&);
    static ViewProvider* create(const Base::Type&);
    static SoFCSelection* createSelection();

private:
    static std::map<Base::Type, Base::Type> _prop_to_view;
};

class GuiExport ViewProviderColorBuilder: public ViewProviderBuilder
{
public:
    /// Constructor
    ViewProviderColorBuilder();
    ~ViewProviderColorBuilder() override;
    void buildNodes(const App::Property*, std::vector<SoNode*>&) const override;
};

}  // namespace Gui


#endif  // GUI_VIEWPROVIDERBUILDER_H
