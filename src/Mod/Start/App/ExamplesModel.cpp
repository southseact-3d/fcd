#include <QDir>


#include "ExamplesModel.h"
#include <App/Application.h>

using namespace Start;


ExamplesModel::ExamplesModel(QObject* parent)
    : DisplayedFilesModel(parent)
{
    auto examplesPath = QDir(QString::fromStdString(App::Application::getResourceDir()));
    _examplesDirectory.setPath(examplesPath.filePath(QLatin1String("examples")));
}

void ExamplesModel::loadExamples()
{
    beginResetModel();
    clear();
    if (!_examplesDirectory.isReadable()) {
        Base::Console().warning(
            "Cannot read %s",
            _examplesDirectory.absolutePath().toStdString().c_str()
        );
    }
    auto entries = _examplesDirectory.entryList(
        QDir::Filter::Files | QDir::Filter::Readable,
        QDir::SortFlag::Name
    );
    for (const auto& entry : entries) {
        addFile(_examplesDirectory.filePath(entry));
    }
    endResetModel();
}
