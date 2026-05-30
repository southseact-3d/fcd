// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 The FreeCAD Project Association AISBL               *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#include <QFile>
#include <QFileIconProvider>
#include <QImageReader>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QLabel>
#include <QModelIndex>
#include <QVBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QString>
#include <QAbstractItemView>

#include "FileCardDelegate.h"
#include "../App/DisplayedFilesModel.h"
#include "App/Application.h"
#include <Base/Color.h>
#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>

using namespace Start;

QCache<QString, QPixmap> FileCardDelegate::_thumbnailCache;

FileCardDelegate::FileCardDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    _parameterGroup = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Start"
    );
    setObjectName(QStringLiteral("thumbnailWidget"));

    // Initialize cache size based on thumbnail size (only once)
    if (_thumbnailCache.maxCost() == 0) {
        int thumbnailSize = static_cast<int>(_parameterGroup->GetInt("FileThumbnailIconsSize", 128));
        int thumbnailMemory = thumbnailSize * thumbnailSize * 4;  // rgba
        int maxCacheItems = (CACHE_SIZE_MB * 1024 * 1024) / thumbnailMemory;
        _thumbnailCache.setMaxCost(maxCacheItems);
        Base::Console().log(
            "FileCardDelegate: Initialized thumbnail cache for %d items (%d MB)\n",
            maxCacheItems,
            CACHE_SIZE_MB
        );
    }
}

void FileCardDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    auto thumbnailSize = static_cast<int>(_parameterGroup->GetInt("FileThumbnailIconsSize", 128));

    // Step 1: Custom card background (matches Option C .file-card)
    QRect cardRect = option.rect.adjusted(1, 1, -1, -1);

    // Determine hover state
    bool isHovered = (option.state & QStyle::State_MouseOver) != 0;

    // Outer card: white background, subtle border, rounded corners
    QColor cardBg(255, 255, 255);       // #ffffff
    QColor borderColor = isHovered ? QColor(209, 213, 219) : QColor(229, 231, 235);  // #d1d5db or #e5e7eb

    painter->setPen(QPen(borderColor, 1));
    painter->setBrush(cardBg);
    painter->drawRoundedRect(cardRect, 10, 10);

    // Thumbnail area: light grey background with bottom border
    int thumbH = std::min(thumbnailSize, cardRect.height() - 40);  // leave room for text
    QRect thumbArea(cardRect.x(), cardRect.y(), cardRect.width(), thumbH);
    painter->setPen(QPen(QColor(243, 244, 246), 1));  // #f3f4f6 bottom border
    painter->setBrush(QColor(249, 250, 251));          // #f9fafb background
    // Draw only bottom line as border, fill the area
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(249, 250, 251));
    painter->drawRect(thumbArea);
    // Bottom border line
    painter->setPen(QPen(QColor(243, 244, 246), 1));
    painter->drawLine(thumbArea.bottomLeft(), thumbArea.bottomRight());

    // Step 2: Fetch required data
    auto baseName = index.data(static_cast<int>(DisplayedFilesModelRoles::baseName)).toString();
    auto elidedName = painter->fontMetrics().elidedText(baseName, Qt::ElideRight, cardRect.width() - 2 * margin);
    auto size = index.data(static_cast<int>(DisplayedFilesModelRoles::size)).toString();
    auto image = index.data(static_cast<int>(DisplayedFilesModelRoles::image)).toByteArray();
    auto path = index.data(static_cast<int>(DisplayedFilesModelRoles::path)).toString();

    QPixmap pixmap;
    if (!image.isEmpty()) {
        pixmap.loadFromData(image);
    }
    else {
        pixmap = generateThumbnail(path);
    }

    QPixmap scaledPixmap = pixmap.scaled(
        QSize(thumbnailSize, thumbnailSize),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    // Step 3: Position thumbnail centered in thumb area
    QRect pixmapRect(thumbArea.topLeft(), scaledPixmap.size());
    pixmapRect.moveCenter(thumbArea.center());
    painter->drawPixmap(pixmapRect.topLeft(), scaledPixmap);

    // Step 4: Draw text below thumbnail area
    int textY = thumbArea.bottom() + margin;
    QRect textRect(
        cardRect.x() + margin,
        textY,
        cardRect.width() - 2 * margin,
        painter->fontMetrics().lineSpacing()
    );

    QRect sizeRect(
        cardRect.x() + margin,
        textRect.bottom() + textspacing,
        cardRect.width() - 2 * margin,
        painter->fontMetrics().lineSpacing()
    );

    // Step 5: Draw text with Option C colors
    painter->setPen(QColor(55, 65, 81));  // #374151 for filename
    QFont nameFont = painter->font();
    nameFont.setWeight(QFont::Medium);
    nameFont.setPointSizeF(nameFont.pointSizeF() * 0.9);
    painter->setFont(nameFont);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedName);

    painter->setPen(QColor(156, 163, 175));  // #9ca3af for filesize
    QFont sizeFont = painter->font();
    sizeFont.setPointSizeF(sizeFont.pointSizeF() * 0.92);
    painter->setFont(sizeFont);
    painter->drawText(sizeRect, Qt::AlignLeft | Qt::AlignTop, size);

    painter->restore();
}


QSize FileCardDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    auto thumbnailSize = _parameterGroup->GetInt("FileThumbnailIconsSize", 128);

    QFontMetrics qfm(QGuiApplication::font());
    int textHeight = textspacing + qfm.lineSpacing() * 2;  // name + size
    int cardWidth = std::max(145, static_cast<int>(thumbnailSize) + 2 * margin);
    int cardHeight = static_cast<int>(thumbnailSize) + textHeight + 3 * margin;

    return {cardWidth, cardHeight};
}

QPixmap FileCardDelegate::generateThumbnail(const QString& path) const
{
    auto thumbnailSize = static_cast<int>(_parameterGroup->GetInt("FileThumbnailIconsSize", 128));  // NOLINT

    // check if we have this thumbnail already inside cache, don't load it once again
    QString cacheKey = getCacheKey(path, thumbnailSize);
    if (!cacheKey.isEmpty()) {
        if (QPixmap* cachedThumbnail = _thumbnailCache.object(cacheKey)) {
            return *cachedThumbnail;  // cache hit - we bail out
        }
    }

    // cache miss - go and load the thumbnail as it could be changed
    return loadAndCacheThumbnail(path, thumbnailSize);
}

QString FileCardDelegate::getCacheKey(const QString& path, int thumbnailSize) const
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        return {};
    }

    // create cache key: path:modtime:size
    QString modTime = QString::number(fileInfo.lastModified().toSecsSinceEpoch());
    return QStringLiteral("%1:%2:%3").arg(path, modTime, QString::number(thumbnailSize));
}

QPixmap FileCardDelegate::loadAndCacheThumbnail(const QString& path, int thumbnailSize) const
{
    QPixmap thumbnail;

    if (path.endsWith(QLatin1String(".fcstd"), Qt::CaseSensitivity::CaseInsensitive)) {
        // This is a fallback, the model will have pulled the thumbnail out of the FCStd file if it
        // existed.
        QImageReader reader(QLatin1String(":/icons/freecad-doc.svg"));
        reader.setScaledSize(QSize(thumbnailSize, thumbnailSize));
        thumbnail = QPixmap::fromImage(reader.read());
    }
    else if (path.endsWith(QLatin1String(".fcmacro"), Qt::CaseSensitivity::CaseInsensitive)) {
        QImageReader reader(QLatin1String(":/icons/MacroEditor.svg"));
        reader.setScaledSize(QSize(thumbnailSize, thumbnailSize));
        thumbnail = QPixmap::fromImage(reader.read());
    }
    else if (!QImageReader::imageFormat(path).isEmpty()) {
        // It is an image: it can be its own thumbnail
        QImageReader reader(path);

        // get original size to calculate proper aspect-preserving scaled size
        QSize originalSize = reader.size();
        if (originalSize.isValid()) {
            QSize scaledSize = originalSize.scaled(thumbnailSize, thumbnailSize, Qt::KeepAspectRatio);
            reader.setScaledSize(scaledSize);
        }

        auto image = reader.read();
        if (!image.isNull()) {
            thumbnail = QPixmap::fromImage(image);
        }
        else {
            Base::Console().log(
                "FileCardDelegate: Failed to load image %s: %s\n",
                path.toStdString().c_str(),
                reader.errorString().toStdString().c_str()
            );
        }
    }

    // fallback to system icon if no thumbnail was generated
    if (thumbnail.isNull()) {
        QIcon icon = QFileIconProvider().icon(QFileInfo(path));
        if (!icon.isNull()) {
            thumbnail = icon.pixmap(thumbnailSize);
        }
        else {
            thumbnail = QPixmap(thumbnailSize, thumbnailSize);
            thumbnail.fill();
        }
    }

    // cache the thumbnail if valid
    if (!thumbnail.isNull()) {
        QString cacheKey = getCacheKey(path, thumbnailSize);
        if (!cacheKey.isEmpty()) {
            _thumbnailCache.insert(cacheKey, new QPixmap(thumbnail), 1);
        }
    }

    return thumbnail;
}
