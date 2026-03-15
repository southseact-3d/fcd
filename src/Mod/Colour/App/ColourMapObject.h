// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_COLOURMAPOBJECT_H
#define COLOUR_APP_COLOURMAPOBJECT_H

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>

#include <Mod/Colour/ColourGlobal.h>

#include "ColourMap.h"

namespace Colour
{

class ColourExport ColourMapObject: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Colour::ColourMapObject);

public:
    ColourMapObject();
    ~ColourMapObject() override;

    App::PropertyLink Source;
    App::PropertyString ColourDataXml;

    const char* getViewProviderName() const override
    {
        return "ColourGui::ViewProviderColourMap";
    }

    short mustExecute() const override;
    App::DocumentObjectExecReturn* execute() override;

    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;

    const ColourMap& getColourMap() const { return m_colourMap; }
    ColourMap& getColourMap() { return m_colourMap; }

    void setColourMap(const ColourMap& map);

protected:
    void onChanged(const App::Property* prop) override;
    void onDocumentRestored() override;

private:
    ColourMap m_colourMap;
    bool m_restoring {false};
};

} // namespace Colour

#endif // COLOUR_APP_COLOURMAPOBJECT_H
