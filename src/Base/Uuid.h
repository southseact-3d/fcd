#ifndef BASE_UUID_H
#define BASE_UUID_H

// Std. configurations

#include <string>
#ifndef FC_GLOBAL_H
# include <FCGlobal.h>
#endif

namespace Base
{

/** Creates a Uuid
 * \author Jürgen Riegel
 */
class BaseExport Uuid
{
public:
    /// Construction
    Uuid();
    Uuid(const Uuid&) = default;
    Uuid(Uuid&&) = default;
    Uuid& operator=(const Uuid&) = default;
    Uuid& operator=(Uuid&&) = default;
    /// Destruction
    virtual ~Uuid();

    void setValue(const char* sString);
    void setValue(const std::string& sString);
    const std::string& getValue() const;
    static std::string createUuid();

    bool operator==(const Uuid& other) const
    {
        return _uuid == other._uuid;
    }
    bool operator<(const Uuid& other) const
    {
        return _uuid < other._uuid;
    }

private:
    std::string _uuid;
};

}  // namespace Base

#endif  // BASE_UUID_H
