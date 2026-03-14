#ifndef TECHDRAW_XMLQuery_h_
#define TECHDRAW_XMLQuery_h_

#include <Mod/TechDraw/TechDrawGlobal.h>
#include <QList>

QT_BEGIN_NAMESPACE
class QDomDocument;
class QDomElement;
QT_END_NAMESPACE

namespace TechDraw
{

class TechDrawExport XMLQuery
{
public:
    XMLQuery(QDomDocument&);
    bool processItems(const QString& queryStr, const std::function<bool(QDomElement&)>& process);

private:
    QDomDocument& domDocument;
};

} //namespace TechDraw

#endif //TECHDRAW_XMLQuery_h_
