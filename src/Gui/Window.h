#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <Base/Parameter.h>
#include <functional>

namespace Gui
{

/** Adapter class to the parameter of FreeCAD for all windows
 * Retrieve the parameter group of the specific window by the windowname.
 * @author Jürgen Riegel
 */
class GuiExport WindowParameter: public ParameterGrp::ObserverType
{
public:
    WindowParameter(const char* name);
    ~WindowParameter() override;

    void OnChange(Base::Subject<const char*>& rCaller, const char* sReason) override;

    /// get the parameters
    static ParameterGrp::handle getDefaultParameter();
    /// return the parameter group of this window
    ParameterGrp::handle getWindowParameter();

protected:
    bool setGroupName(const char* name);
    /// connect slot to ParameterManager signal
    template<typename S, typename T>
    void setSlotParamChanged(S slot, T* obsPtr);

private:
    ParameterGrp::handle _handle;
    fastsignals::connection connParamChanged;
};


template<typename S, typename T>
inline void WindowParameter::setSlotParamChanged(S slot, T* obsPtr)
{
    namespace bp = std::placeholders;
    if (_handle->Manager()) {
        connParamChanged = _handle->Manager()->signalParamChanged.connect(
            std::bind(slot, obsPtr, bp::_1, bp::_2, bp::_3, bp::_4)
        );
    }
}

}  // namespace Gui

#endif  // GUI_WINDOW_H
