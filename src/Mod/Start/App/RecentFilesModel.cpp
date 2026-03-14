#include "RecentFilesModel.h"
#include <App/Application.h>
#include <App/ProjectFile.h>

using namespace Start;

RecentFilesModel::RecentFilesModel(QObject* parent)
    : DisplayedFilesModel(parent)
{
    _parameterGroup = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/RecentFiles"
    );
}

void RecentFilesModel::loadRecentFiles()
{
    beginResetModel();
    clear();
    auto numRows {_parameterGroup->GetInt("RecentFiles", 0)};
    for (int i = 0; i < numRows; ++i) {
        auto entry = fmt::format("MRU{}", i);
        auto path = _parameterGroup->GetASCII(entry.c_str(), "");
        addFile(QString::fromStdString(path));
    }
    endResetModel();
}
