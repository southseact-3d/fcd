// clazy:excludeall=qstring-arg

# include <cstdlib>
# include <QBuffer>
# include <QByteArray>
# include <QImage>
# include <QTextCursor>


#include "mtextedit.h"


MTextEdit::MTextEdit(QWidget *parent) : QTextEdit(parent) {
}


bool MTextEdit::canInsertFromMimeData(const QMimeData *source) const {
    return source->hasImage() || QTextEdit::canInsertFromMimeData(source);
}


void MTextEdit::insertFromMimeData(const QMimeData *source) {
    if (source->hasImage()) {
        QStringList formats = source->formats();
        QString format;
        for (int i=0; i<formats.size(); i++) {
            if (formats[i] == QLatin1String("image/bmp"))  { format = QString::fromLatin1("BMP");  break; }
            if (formats[i] == QLatin1String("image/jpeg")) { format = QString::fromLatin1("JPG");  break; }
            if (formats[i] == QLatin1String("image/jpg"))  { format = QString::fromLatin1("JPG");  break; }
            if (formats[i] == QLatin1String("image/gif"))  { format = QString::fromLatin1("GIF");  break; }
            if (formats[i] == QLatin1String("image/png"))  { format = QString::fromLatin1("PNG");  break; }
            if (formats[i] == QLatin1String("image/pbm"))  { format = QString::fromLatin1("PBM");  break; }
            if (formats[i] == QLatin1String("image/pgm"))  { format = QString::fromLatin1("PGM");  break; }
            if (formats[i] == QLatin1String("image/ppm"))  { format = QString::fromLatin1("PPM");  break; }
            if (formats[i] == QLatin1String("image/tiff")) { format = QString::fromLatin1("TIFF"); break; }
            if (formats[i] == QLatin1String("image/xbm"))  { format = QString::fromLatin1("XBM");  break; }
            if (formats[i] == QLatin1String("image/xpm"))  { format = QString::fromLatin1("XPM");  break; }
            }
        if (!format.isEmpty()) {
//          dropImage(qvariant_cast<QImage>(source->imageData()), format);
            dropImage(qvariant_cast<QImage>(source->imageData()), QString::fromLatin1("JPG")); // Sorry, ale cokoli jiného dlouho trvá
            return;
            }
        }
    QTextEdit::insertFromMimeData(source);
}


QMimeData *MTextEdit::createMimeDataFromSelection() const {
    return QTextEdit::createMimeDataFromSelection();
}


void MTextEdit::dropImage(const QImage& image, const QString& format) {
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, format.toLocal8Bit().data());
    buffer.close();
    QByteArray base64 = bytes.toBase64();
    QByteArray base64l;
    for (int i=0; i<base64.size(); i++) {
        base64l.append(base64[i]);
        if (i%80 == 0) {
            base64l.append("\n");
            }
        }

    QTextCursor cursor = textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth  ( image.width() );
    imageFormat.setHeight ( image.height() );
    imageFormat.setName   ( QString::fromLatin1("data:image/%1;base64, %2")
                                .arg(QString::fromLatin1("%1.%2").arg(rand()).arg(format))
                                .arg(QString::fromLatin1(base64l.data()))
                                );
    cursor.insertImage    ( imageFormat );
}

#include <Mod/TechDraw/Gui/moc_mtextedit.cpp>

