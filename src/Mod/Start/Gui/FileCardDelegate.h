#include <Base/Parameter.h>
#include <QCache>
#include <QEvent>
#include <QFileInfo>
#include <QImage>
#include <QPushButton>
#include <QStyledItemDelegate>

class FileCardDelegate: public QStyledItemDelegate
{

public:
    explicit FileCardDelegate(QObject* parent = nullptr);

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
    ) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
    QPixmap generateThumbnail(const QString& path) const;

private:
    QString getCacheKey(const QString& path, int thumbnailSize) const;
    QPixmap loadAndCacheThumbnail(const QString& path, int thumbnailSize) const;

    Base::Reference<ParameterGrp> _parameterGroup;
    const int margin = 11;
    const int textspacing = 2;
    QPushButton styleButton;

    static QCache<QString, QPixmap> _thumbnailCache;  // cache key structure: "path:modtime:size"
    static constexpr const int CACHE_SIZE_MB = 50;    // 50MB cache limit
};


#endif  // FREECAD_START_FILECARDDELEGATE_H
