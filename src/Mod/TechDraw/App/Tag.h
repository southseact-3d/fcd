#ifndef TECHDRAW_TAG_H
#define TECHDRAW_TAG_H

#include <string>
#include <string_view>

#include <boost/uuid/uuid.hpp>

#include <Mod/TechDraw/TechDrawGlobal.h>

namespace Base {
class XMLReader;
class Writer;
}

namespace TechDraw {


//NOLINTNEXTLINE    (default destructor but no copy)
class TechDrawExport Tag {
public:
    virtual ~Tag() = default;
    //Uniqueness
    boost::uuids::uuid getTag() const;
    virtual std::string getTagAsString() const;

    static boost::uuids::uuid fromString(const std::string& tagString);


protected:
    Tag();
    void setTag(const boost::uuids::uuid& newTag);
    void Save(Base::Writer& writer) const;
    // Setting elementName is only for backwards compatibility!
    void Restore(Base::XMLReader& reader, std::string_view elementName="Tag");

private:
    void createNewTag();
    boost::uuids::uuid tag{};
};
}

#endif
