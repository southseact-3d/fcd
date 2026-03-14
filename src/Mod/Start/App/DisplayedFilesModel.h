#include <QAbstractListModel>
#include <Base/Parameter.h>

#include "../StartGlobal.h"

namespace Start
{

enum class DisplayedFilesModelRoles
{
    baseName = Qt::UserRole + 1,
    image,
    size,
    author,
    creationTime,
    modifiedTime,
    description,
    company,
    license,
    path
};

using FileStats = std::map<DisplayedFilesModelRoles, std::string>;

/// A model for displaying a list of files including a thumbnail or icon, plus various file
/// statistics.
class StartExport DisplayedFilesModel: public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DisplayedFilesModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addFile(const QString& filePath);

    void clear();

protected:
    /// For communication with QML, define the text version of each role name defined in the
    /// DisplayedFilesModelRoles enumeration
    QHash<int, QByteArray> roleNames() const override;

    /// Process a new thumbnail produces by some sort of worker thread
    void processNewThumbnail(const QString& file, const QByteArray& thumbnail);

private:
    std::vector<FileStats> _fileInfoCache;
    QMap<QString, QByteArray> _imageCache;
};

}  // namespace Start

#endif  // FREECAD_START_DISPLAYED_FILES_MODEL_H
