#ifndef GUI_WORKBENCHFACTORY_H
#define GUI_WORKBENCHFACTORY_H

#include <list>
#include <string>
#include <Base/Factory.h>

namespace Gui
{
class Workbench;

/**
 * The workbench factory provides methods for the dynamic creation of
 * special workbenches for each module. To create these workbenches once
 * they must be registered through a subclass of WorkbenchProducer.
 * @note To create workbenches you should use the API of WorkbenchManager.
 * @author Werner Mayer
 */
class GuiExport WorkbenchFactoryInst: public Base::Factory
{
public:
    /// The one and only instance.
    static WorkbenchFactoryInst& instance();
    /// Destructs the sole instance.
    static void destruct();

    /** Creates the workbench with \a name. If no such workbench is registered
     * 0 is returned.
     */
    Workbench* createWorkbench(const char* sName) const;
    /** Returns a list of all registered workbench classes. */
    std::list<std::string> workbenches() const;

private:
    static WorkbenchFactoryInst* _pcSingleton;

    WorkbenchFactoryInst() = default;
    ~WorkbenchFactoryInst() override = default;
};

inline GuiExport WorkbenchFactoryInst& WorkbenchFactory()
{
    return WorkbenchFactoryInst::instance();
}

// --------------------------------------------------------------------

/**
 * The WorkbenchProducer template class allows the registration and the creation of workbench
 * classes.
 * @author Werner Mayer
 */
template<class CLASS>
class WorkbenchProducer: public Base::AbstractProducer
{
public:
    WorkbenchProducer() = default;

    ~WorkbenchProducer() override = default;

    void* Produce() const override
    {
        return (new CLASS);
    }
};

}  // namespace Gui

#endif  // GUI_WORKBENCHFACTORY_H
