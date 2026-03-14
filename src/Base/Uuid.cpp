#include <QUuid>

#include <stdexcept>

#include "Uuid.h"


using namespace Base;


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
Uuid::Uuid()
{
    _uuid = createUuid();
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
Uuid::~Uuid() = default;

//**************************************************************************
//**************************************************************************
// Get the UUID
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::string Uuid::createUuid()
{
    std::string Uuid;
    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1);
    uuid.chop(1);
    Uuid = uuid.toLatin1().constData();
    return Uuid;
}

void Uuid::setValue(const char* sString)
{
    if (sString) {
        QUuid uuid(QString::fromLatin1(sString));
        if (uuid.isNull()) {
            throw std::runtime_error("invalid uuid");
        }
        // remove curly braces
        QString id = uuid.toString();
        id = id.mid(1);
        id.chop(1);
        _uuid = id.toLatin1().constData();
    }
}

void Uuid::setValue(const std::string& sString)
{
    setValue(sString.c_str());
}

const std::string& Uuid::getValue() const
{
    return _uuid;
}
