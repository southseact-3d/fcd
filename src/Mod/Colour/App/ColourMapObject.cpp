// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ColourMapObject.h"

#include <Base/Console.h>
#include <Base/XMLTools.h>

namespace Colour
{

PROPERTY_SOURCE(Colour::ColourMapObject, App::DocumentObject)

ColourMapObject::ColourMapObject()
{
    ADD_PROPERTY_TYPE(Source, (nullptr), "ColourMap", App::Prop_None,
                      "Link to the source shape object");
    ADD_PROPERTY_TYPE(ColourDataXml, (""), "ColourMap", App::Prop_Hidden,
                      "Serialized colour map data");
}

ColourMapObject::~ColourMapObject() = default;

short ColourMapObject::mustExecute() const
{
    if (Source.isTouched() || ColourDataXml.isTouched()) {
        return 1;
    }
    return DocumentObject::mustExecute();
}

App::DocumentObjectExecReturn* ColourMapObject::execute()
{
    return StdReturn;
}

void ColourMapObject::Save(Base::Writer& writer) const
{
    DocumentObject::Save(writer);

    writer.beginElement("ColourMapData");
    writer.addAttribute("version", "1");

    std::string xmlData = m_colourMap.serializeXml();
    writer.addCharacters(xmlData.c_str());

    writer.endElement("ColourMapData");
}

void ColourMapObject::Restore(Base::XMLReader& reader)
{
    m_restoring = true;
    DocumentObject::Restore(reader);

    reader.readElement("ColourMapData");
    if (reader.hasAttribute("version")) {
        int version = reader.getAttribute<int>("version");
        (void)version;
    }

    const char* data = reader.getCharacters();
    if (data && *data) {
        m_colourMap.deserializeXml(data);
        ColourDataXml.setValue(m_colourMap.serializeXml());
    }

    reader.readEndElement("ColourMapData");
    m_restoring = false;
}

void ColourMapObject::onChanged(const App::Property* prop)
{
    if (!m_restoring) {
        if (prop == &ColourDataXml) {
            const std::string& xml = ColourDataXml.getValue();
            if (!xml.empty()) {
                m_colourMap.deserializeXml(xml);
            }
        }
    }

    DocumentObject::onChanged(prop);
}

void ColourMapObject::onDocumentRestored()
{
    const std::string& xml = ColourDataXml.getValue();
    if (!xml.empty()) {
        m_colourMap.deserializeXml(xml);
    }

    DocumentObject::onDocumentRestored();
}

void ColourMapObject::setColourMap(const ColourMap& map)
{
    m_colourMap = map;
    ColourDataXml.setValue(m_colourMap.serializeXml());
    touch();
}

} // namespace Colour
