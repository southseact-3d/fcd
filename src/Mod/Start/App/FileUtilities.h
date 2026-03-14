#include "Base/FileInfo.h"
#include "Mod/Start/StartGlobal.h"

#include <qglobal.h>
#include <QDir>
#include <QStandardPaths>

class QString;

namespace Start
{

const QLatin1String defaultThumbnailPath("thumbnails/Thumbnail.png");

const QLatin1String defaultThumbnailName
#if defined(Q_OS_LINUX)
    ("thumbnails/normal");
#else
    ("FreeCADStartThumbnails");
#endif

const QDir thumbnailsParentDir {QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)};

const QDir thumbnailsDir {thumbnailsParentDir.absoluteFilePath(defaultThumbnailName)};

StartExport void createThumbnailsDir();

StartExport QString getMD5Hash(const QString& path);

StartExport QString getPathToCachedThumbnail(const QString& path);

StartExport bool useCachedThumbnail(const QString& image, const QString& project);

StartExport std::string humanReadableSize(std::uint64_t bytes);

StartExport std::string getLastModifiedAsString(const Base::FileInfo& file);

}  // namespace Start

#endif  // FREECAD_FILEUTILITIES_H
