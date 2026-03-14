#ifndef EXPRESSIONTOKENIZER_H
#define EXPRESSIONTOKENIZER_H

#include <QString>
#include <FCGlobal.h>

namespace App
{

class AppExport ExpressionTokenizer
{
public:
    QString perform(const QString& text, int pos);

    void getPrefixRange(int& start, int& end) const
    {
        start = prefixStart;
        end = prefixEnd;
    }

    void updatePrefixEnd(int end)
    {
        prefixEnd = end;
    }

private:
    int prefixStart = 0;
    int prefixEnd = 0;
};

}  // namespace App

#endif  // EXPRESSIONTOKENIZER_H
