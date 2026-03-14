#ifndef APP_CLEANUPPROCESS_H
#define APP_CLEANUPPROCESS_H

#include <functional>
#include <FCGlobal.h>

namespace App
{

/*!
 * \brief The CleanupProcess class
 */
class AppExport CleanupProcess
{
public:
    /*!
     * \brief registerCleanup
     * \param func
     * This adds a callback function that will be called when the application
     * is about to be shut down.
     * @note A callback function is only about to free resources. Accessing
     * stuff of the application like parameter groups should be avoided.
     */
    static void registerCleanup(const std::function<void()>& func);
    /*!
     * \brief callCleanup
     * Calls the functions that are registered with \a registerCleanup.
     */
    static void callCleanup();
};

}  // namespace App

#endif  // APP_CLEANUPPROCESS_H
