#ifndef APP_FEATURECUSTOM_H
#define APP_FEATURECUSTOM_H

#include <App/PropertyContainer.h>

namespace App
{

class DocumentObjectExecReturn;
class Property;

/**
 * FeatureCustomT is a template class to be used with DocumentObject or
 * any of its subclasses as template parameter.
 * FeatureCustomT offers a way to add or remove a property at runtime.
 * This class is similar to \ref FeaturePythonT with the difference that
 * it has no support for in Python written feature classes.
 * @author Werner Mayer
 */
template<class FeatureT>
class FeatureCustomT: public FeatureT  // NOLINT
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::FeatureCustomT<FeatureT>);

public:
    FeatureCustomT() = default;

    ~FeatureCustomT() override = default;

    /** @name methods override DocumentObject */
    //@{
    short mustExecute() const override
    {
        return FeatureT::mustExecute();
    }
    /// recalculate the Feature
    DocumentObjectExecReturn* execute() override
    {
        return FeatureT::execute();
    }
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return FeatureT::getViewProviderName();
    }

    PyObject* getPyObject() override
    {
        return FeatureT::getPyObject();
    }
    void setPyObject(PyObject* obj) override
    {
        FeatureT::setPyObject(obj);
    }

protected:
    void onBeforeChange(const Property* prop) override
    {
        FeatureT::onBeforeChange(prop);
    }
    void onChanged(const Property* prop) override
    {
        FeatureT::onChanged(prop);
    }
    void onDocumentRestored() override
    {
        FeatureT::onDocumentRestored();
    }
    void onSettingDocument() override
    {
        FeatureT::onSettingDocument();
    }

public:
    FeatureCustomT(const FeatureCustomT&) = delete;
    FeatureCustomT(FeatureCustomT&&) = delete;
    FeatureCustomT& operator=(const FeatureCustomT&) = delete;
    FeatureCustomT& operator=(FeatureCustomT&&) = delete;
};

}  // namespace App

#endif  // APP_FEATURECUSTOM_H
