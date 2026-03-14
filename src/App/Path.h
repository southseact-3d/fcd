#ifndef APP_Path_H
#define APP_Path_H

#include <Base/Persistence.h>
#include <FCGlobal.h>


namespace App
{

/** Base class of all geometric document objects.
 */
class AppExport Path
{
protected:
    std::vector<Base::Persistence*> _PathVector;

public:
    /// Constructor
    Path() = default;
    explicit Path(const std::vector<Base::Persistence*>& PathVector);

    virtual ~Path() = default;

    const std::vector<Base::Persistence*>& getVector() const
    {
        return _PathVector;
    }
};

}  // namespace App


#endif  // APP_Path_H
