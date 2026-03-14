#ifndef APP_BRANDING_H
#define APP_BRANDING_H

#include <string>
#include <QDomDocument>
#include <QMap>
#include <QString>
#include <QVector>


class QIODevice;

namespace App
{

class Branding
{
public:
    using XmlConfig = QMap<std::string, std::string>;
    Branding();

    bool readFile(const QString& fn);
    XmlConfig getUserDefines() const;

private:
    QVector<std::string> filter;
    bool evaluateXML(QIODevice* device, QDomDocument& xmlDocument);
    QDomDocument domDocument;
};

}  // namespace App

#endif  // APP_BRANDING_H
