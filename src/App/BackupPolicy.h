#include "FCGlobal.h"

#include <boost/regex.hpp>
#include <string>
#include <Base/FileInfo.h>

namespace App
{
// Helper class to handle different backup policies: originally a private class in Document.cpp,
// and extracted for public access to enable direct testing since the logic involved is quite
// complex.
class AppExport BackupPolicy
{
public:
    enum Policy
    {
        Standard,
        TimeStamp
    };
    void setPolicy(const Policy p);
    void setNumberOfFiles(const int count);
    void useBackupExtension(const bool on);
    void setDateFormat(const std::string& fmt);
    void apply(const std::string& sourcename, const std::string& targetname);

private:
    void applyStandard(const std::string& sourcename, const std::string& targetname) const;
    void applyTimeStamp(const std::string& sourcename, const std::string& targetname);
    static bool fileComparisonByDate(const Base::FileInfo& i, const Base::FileInfo& j);
    bool startsWith(const std::string& st1, const std::string& st2) const;
    bool checkValidString(const std::string& cmpl, const boost::regex& e) const;
    bool checkValidComplement(const std::string& file,
                              const std::string& pbn,
                              const std::string& ext) const;
    bool checkDigits(const std::string& cmpl) const;
    bool renameFileNoErase(Base::FileInfo fi, const std::string& newName);

private:
    Policy policy {Standard};
    int numberOfFiles {1};
    bool useFCBakExtension {true};
    std::string saveBackupDateFormat {"%Y%m%d-%H%M%S"};
};
}  // namespace App