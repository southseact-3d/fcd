#include <boost/random.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#include <Base/Reader.h>
#include <Base/Writer.h>

#include "Tag.h"

using namespace TechDraw;

// lint complains if tag is not initialized here.
Tag::Tag() :
    tag(boost::uuids::uuid())
{
    createNewTag();
}

boost::uuids::uuid Tag::getTag() const
{
    return tag;
}

std::string Tag::getTagAsString() const
{
    return boost::uuids::to_string(getTag());
}

boost::uuids::uuid Tag::fromString(const std::string& tagString)
{
    boost::uuids::string_generator gen;
    boost::uuids::uuid u1 = gen(tagString);
    return u1;

}

void Tag::setTag(const boost::uuids::uuid& newTag)
{
    tag = newTag;
}

void Tag::createNewTag()
{
    // Initialize a random number generator, to avoid Valgrind false positives.
    // The random number generator is not threadsafe so we guard it.  See
    // https://www.boost.org/doc/libs/1_62_0/libs/uuid/uuid.html#Design%20notes
    static boost::mt19937 ran;
    static bool seeded = false;
    static boost::mutex random_number_mutex;

    boost::lock_guard<boost::mutex> guard(random_number_mutex);

    if (!seeded) {
        ran.seed(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
    static boost::uuids::basic_random_generator<boost::mt19937> gen(&ran);

    tag = gen();
}

void Tag::Save(Base::Writer& writer) const
{
    writer.Stream() << writer.ind() << "<Tag value=\"" <<  getTagAsString() << "\"/>" << std::endl;
}

void Tag::Restore(Base::XMLReader& reader, std::string_view elementName)
{
    // Setting elementName is only for backwards compatibility!
    reader.readElement(elementName.data());
    std::string temp = reader.getAttribute<const char*>("value");
    tag = fromString(temp);
}


