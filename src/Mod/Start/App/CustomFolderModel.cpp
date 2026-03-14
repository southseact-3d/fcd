#include <QDir>

#include "CustomFolderModel.h"
#include <App/Application.h>
#include <QStringList>

using namespace Start;


CustomFolderModel::CustomFolderModel(QObject* parent)
    : DisplayedFilesModel(parent)
{

    Base::Reference<ParameterGrp> parameterGroup = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Start"
    );

    _customFolderPathSpec = QString::fromStdString(parameterGroup->GetASCII("CustomFolder", ""));

    _showOnlyFCStd = parameterGroup->GetBool("ShowOnlyFCStd", false);
}

/// If the custom folder path contains multiple paths separated by ';;', split them into individual
/// paths. This is used to allow the user to specify multiple paths in the preferences dialog.
/// We use ';;' as a separator because ';' is a valid character in a file path (e.g. NTFS on
/// Windows).
void CustomFolderModel::loadCustomFolder()
{
    beginResetModel();
    clear();
    auto pathDelimiter = QStringLiteral(";;");
    auto paths = _customFolderPathSpec.split(pathDelimiter, Qt::SkipEmptyParts);

    for (const auto& path : paths) {
        QDir customFolderDirectory(path);
        if (!customFolderDirectory.exists()) {
            Base::Console().warning(
                "BaseApp/Preferences/Mod/Start/CustomFolder: custom folder %s does not exist\n",
                customFolderDirectory.absolutePath().toStdString().c_str()
            );
            continue;
        }
        if (!customFolderDirectory.isReadable()) {
            Base::Console().warning(
                "BaseApp/Preferences/Mod/Start/CustomFolder: cannot read custom folder %s\n",
                customFolderDirectory.absolutePath().toStdString().c_str()
            );
            continue;
        }
        if (_showOnlyFCStd) {
            customFolderDirectory.setNameFilters(QStringList() << QStringLiteral("*.FCStd"));
        }

        auto entries = customFolderDirectory.entryList(
            QDir::Filter::Files | QDir::Filter::Readable,
            QDir::SortFlag::Name
        );
        for (const auto& entry : entries) {
            addFile(customFolderDirectory.filePath(entry));
        }
    }

    endResetModel();
}
