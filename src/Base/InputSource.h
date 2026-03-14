#ifndef BASE_IINPUTSOURCE_H
#define BASE_IINPUTSOURCE_H

#include <iosfwd>
#include <memory>

#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/sax/InputSource.hpp>
#ifndef FC_GLOBAL_H
# include <FCGlobal.h>
#endif

namespace XERCES_CPP_NAMESPACE
{
class BinInputStream;
}

namespace Base
{


class BaseExport StdInputStream: public XERCES_CPP_NAMESPACE::BinInputStream
{
public:
    // clang-format off
    explicit StdInputStream(std::istream& Stream,
                   XERCES_CPP_NAMESPACE::MemoryManager* const manager =
                   XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager);
    ~StdInputStream() override;
    // clang-format on

    // -----------------------------------------------------------------------
    //  Implementation of the input stream interface
    // -----------------------------------------------------------------------
    XMLFilePos curPos() const override;
    XMLSize_t readBytes(XMLByte* const toFill, const XMLSize_t maxToRead) override;
    const XMLCh* getContentType() const override
    {
        return nullptr;
    }

    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    StdInputStream(const StdInputStream&) = delete;
    StdInputStream(StdInputStream&&) = delete;
    StdInputStream& operator=(const StdInputStream&) = delete;
    StdInputStream& operator=(StdInputStream&&) = delete;

private:
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSource
    //      The source file that we represent. The FileHandle type is defined
    //      per platform.
    // -----------------------------------------------------------------------
    std::istream& stream;
    struct TextCodec;
    std::unique_ptr<TextCodec> codec;
};


class BaseExport StdInputSource: public XERCES_CPP_NAMESPACE::InputSource
{
public:
    StdInputSource(
        std::istream& Stream,
        const char* filePath,
        XERCES_CPP_NAMESPACE::MemoryManager* const manager
        = XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager
    );
    ~StdInputSource() override;

    XERCES_CPP_NAMESPACE::BinInputStream* makeStream() const override;

    StdInputSource(const StdInputSource&) = delete;
    StdInputSource(StdInputSource&&) = delete;
    StdInputSource& operator=(const StdInputSource&) = delete;
    StdInputSource& operator=(StdInputSource&&) = delete;

private:
    std::istream& stream;
};

}  // namespace Base

#endif  // BASE_IINPUTSOURCE_H
