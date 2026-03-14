#ifndef START_PRECOMPILED_H
#define START_PRECOMPILED_H

#include <FCConfig.h>

// standard
#include <cinttypes>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

// boost
#include <boost/algorithm/string/predicate.hpp>

// fmt
#include <fmt/format.h>

// Qt (should never include GUI files, only QtCore)
#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaObject>
#include <QMutexLocker>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QString>
#include <QThreadPool>
#include <QTimeZone>
#include <QTimer>
#include <QUrl>

#endif  // START_PRECOMPILED_H
