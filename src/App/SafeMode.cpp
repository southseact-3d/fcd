#include <QTemporaryDir>

#include "Application.h"
#include "FCConfig.h"

#include "SafeMode.h"

static QTemporaryDir* tempDir = nullptr;

static bool _createTemporaryBaseDir()
{
    tempDir = new QTemporaryDir();
    if (!tempDir->isValid()) {
        delete tempDir;
        tempDir = nullptr;
    }
    return tempDir;
}

static void _replaceDirs()
{
    auto& config = App::GetApplication().Config();

    auto const temp_base = tempDir->path().toStdString();
    auto const dirs = {
        "UserAppData",
        "UserConfigPath",
        "UserCachePath",
        "AppTempPath",
        "UserMacroPath",
        "UserHomePath",
    };

    for (auto const d : dirs) {
        auto const path = temp_base + PATHSEP + d + PATHSEP;
        auto const qpath = QString::fromStdString(path);
        QDir().mkpath(qpath);
        config[d] = path;
    }
}

void SafeMode::StartSafeMode()
{
    if (_createTemporaryBaseDir()) {
        _replaceDirs();
    }
}

bool SafeMode::SafeModeEnabled()
{
    return tempDir;
}

void SafeMode::Destruct()
{
    delete tempDir;
}
