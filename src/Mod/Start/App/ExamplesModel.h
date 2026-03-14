#include <QAbstractListModel>
#include <QDir>
#include <Base/Parameter.h>

#include "DisplayedFilesModel.h"
#include "../StartGlobal.h"

namespace Start
{

/// A model for displaying a list of files including a thumbnail or icon, plus various file
/// statistics.
class StartExport ExamplesModel: public DisplayedFilesModel
{
    Q_OBJECT
public:
    explicit ExamplesModel(QObject* parent = nullptr);

    void loadExamples();

private:
    QDir _examplesDirectory;
};

}  // namespace Start

#endif  // FREECAD_START_EXAMPLESMODEL_H
