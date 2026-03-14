#include <QAbstractListModel>
#include <Base/Parameter.h>

#include "DisplayedFilesModel.h"
#include "../StartGlobal.h"

namespace Start
{

/// A model for displaying a list of files including a thumbnail or icon, plus various file
/// statistics.
class StartExport RecentFilesModel: public DisplayedFilesModel
{
    Q_OBJECT
public:
    explicit RecentFilesModel(QObject* parent = nullptr);

    void loadRecentFiles();

private:
    Base::Reference<ParameterGrp> _parameterGroup;
};

}  // namespace Start

#endif  // FREECAD_START_RECENTFILESMODEL_H
