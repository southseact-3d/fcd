#include <QAbstractListModel>
#include <QString>
#include <Base/Parameter.h>

#include "DisplayedFilesModel.h"
#include "../StartGlobal.h"

namespace Start
{

/// A model for displaying a list of files including a thumbnail or icon, plus various file
/// statistics.
class StartExport CustomFolderModel: public DisplayedFilesModel
{
    Q_OBJECT
public:
    explicit CustomFolderModel(QObject* parent = nullptr);

    void loadCustomFolder();

private:
    QString _customFolderPathSpec;
    bool _showOnlyFCStd;  // Show only FreeCAD files
};

}  // namespace Start

#endif  // FREECAD_START_CUSTOMFOLDERMODEL_H
