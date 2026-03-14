#include <istream>

#include <qglobal.h>
#if QT_VERSION < 0x060000
# include <QTextCodec>
#else
# include <QByteArray>
# include <QStringDecoder>
# include <QStringEncoder>
#endif

#include "InputSource.h"
#include "XMLTools.h"

using namespace Base;
using namespace std;


// ---------------------------------------------------------------------------
//  StdInputStream: Constructors and Destructor
// ---------------------------------------------------------------------------

#if QT_VERSION < 0x060000
struct StdInputStream::TextCodec
{
    QTextCodec::ConverterState state;
    TextCodec()
    {
        state.flags |= QTextCodec::IgnoreHeader;
        state.flags |= QTextCodec::ConvertInvalidToNull;
    }

    void validateBytes(XMLByte* const toFill, std::streamsize len)
    {
        QTextCodec* textCodec = QTextCodec::codecForName("UTF-8");
        if (textCodec) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            const QString text
                = textCodec->toUnicode(reinterpret_cast<char*>(toFill), static_cast<int>(len), &state);
            if (state.invalidChars > 0) {
                // In case invalid characters were found decode back to 'utf-8' and replace
                // them with '?'
                // First, Qt replaces invalid characters with '\0' (see ConvertInvalidToNull)
                // but Xerces doesn't like this because it handles this as termination. Thus,
                // we have to go through the array and replace '\0' with '?'.
                std::streamsize pos = 0;
                QByteArray ba = textCodec->fromUnicode(text);
                for (int i = 0; i < ba.length(); i++, pos++) {
                    if (pos < len && ba[i] == '\0') {
                        toFill[i] = '?';
                    }
                }
            }
        }
    }
};
#else
struct StdInputStream::TextCodec
{
    void validateBytes(XMLByte* const toFill, std::streamsize len)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        QByteArray encodedString(reinterpret_cast<char*>(toFill), static_cast<int>(len));
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        QString text = toUtf16(encodedString);
        if (toUtf16.hasError()) {
            // In case invalid characters were found decode back to 'utf-8' and replace
            // them with '?'
            // First, Qt replaces invalid characters with '\0'
            // but Xerces doesn't like this because it handles this as termination. Thus,
            // we have to go through the array and replace '\0' with '?'.
            std::streamsize pos = 0;
            auto fromUtf16 = QStringEncoder(QStringEncoder::Utf8);
            QByteArray ba = fromUtf16(text);
            for (int i = 0; i < ba.length(); i++, pos++) {
                if (pos < len && ba[i] == '\0') {
                    toFill[i] = '?';
                }
            }
        }
    }
};
#endif

StdInputStream::StdInputStream(std::istream& Stream, XERCES_CPP_NAMESPACE::MemoryManager* const manager)
    : stream(Stream)
    , codec(new TextCodec)
{
    (void)manager;
}


StdInputStream::~StdInputStream() = default;


// ---------------------------------------------------------------------------
//  StdInputStream: Implementation of the input stream interface
// ---------------------------------------------------------------------------
XMLFilePos StdInputStream::curPos() const
{
    return static_cast<XMLFilePos>(stream.tellg());
}

XMLSize_t StdInputStream::readBytes(XMLByte* const toFill, const XMLSize_t maxToRead)
{
    //
    //  Read up to the maximum bytes requested. We return the number
    //  actually read.
    //
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    stream.read(reinterpret_cast<char*>(toFill), static_cast<std::streamsize>(maxToRead));
    std::streamsize len = stream.gcount();

    codec->validateBytes(toFill, len);

    return static_cast<XMLSize_t>(len);
}


// ---------------------------------------------------------------------------
//  StdInputSource: Constructors and Destructor
// ---------------------------------------------------------------------------
StdInputSource::StdInputSource(
    std::istream& Stream,
    const char* filePath,
    XERCES_CPP_NAMESPACE::MemoryManager* const manager
)
    : InputSource(manager)
    , stream(Stream)
{
    // we have to set the file name in case an error occurs
    XStr tmpBuf(filePath);
    setSystemId(tmpBuf.unicodeForm());
}


StdInputSource::~StdInputSource() = default;


// ---------------------------------------------------------------------------
//  StdInputSource: InputSource interface implementation
// ---------------------------------------------------------------------------
XERCES_CPP_NAMESPACE::BinInputStream* StdInputSource::makeStream() const
{
    StdInputStream* retStrm = new StdInputStream(stream /*, getMemoryManager()*/);
    return retStrm;
}
