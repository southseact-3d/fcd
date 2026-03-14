#include <QCoreApplication>
#include <QFile>
#include <QDomDocument>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <iostream>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();
    if (args.size() != 2) {
        std::cerr << "Requires input file\n";
        return -1;
    }

    QFile file(args[1]);
    if (!file.open(QFile::ReadOnly)) {
        std::cerr << "Failed to read file\n";
        return -1;
    }

    QXmlStreamReader reader(file.readAll());
    file.close();

    if (!file.open(QFile::WriteOnly)) {
        std::cerr << "Failed to write file\n";
        return -1;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(4);

    auto findAttr = [](const QXmlStreamAttributes& attr, const QString& name) {
        for (int i = 0; i < attr.size(); ++i) {
            if (attr.at(i).name() == name) {
                return i;
            }
        }

        return -1;
    };

    // ----------------------

    auto sortAttr = [&findAttr](QXmlStreamAttributes& attr) {
        QStringList list = {
            "Name",
            "Namespace",
            "Twin",
            "TwinPointer",
            "PythonName",
            "FatherInclude",
            "Include",
            "Father",
            "FatherNamespace"
        };
        QXmlStreamAttributes sorted;
        for (const auto& it : list) {
            int index = findAttr(attr, it);
            if (index > -1) {
                sorted.append(attr.at(index));
                attr.remove(index);
            }
        }

        // add the rest
        for (const auto& it : std::as_const(attr)) {
            sorted.append(it);
        }

        return sorted;
    };

    // ----------------------

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == QLatin1String("PythonExport")) {
            QXmlStreamAttributes attr = reader.attributes();
            attr = sortAttr(attr);

            writer.writeStartElement(QString("PythonExport"));
            for (int i = 0; i < attr.size(); ++i) {
                file.write("\n       ");
                writer.writeAttribute(attr.at(i));
            }
        }
        else if (reader.isStartElement() && reader.name() == QLatin1String("UserDocu")) {
            QString text = reader.readElementText().trimmed();
            text.replace("\t", "    ");
            writer.writeTextElement(QString("UserDocu"), text);
        }
        else if (!reader.isWhitespace()) {
            writer.writeCurrentToken(reader);
        }
    }

    file.close();

    return 0;
}
