#ifndef TECHDRAW_DrawTemplate_h_
#define TECHDRAW_DrawTemplate_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyUnits.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDraw
{

class DrawPage;

class TechDrawExport DrawTemplate : public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawTemplate);

public:
    DrawTemplate(); /// Constructor
    ~DrawTemplate() override;

    // Page Physical Properties
    App::PropertyLength Width;
    App::PropertyLength Height;
    App::PropertyEnumeration Orientation;

    App::PropertyMap EditableTexts;

public:
    /// Returns template width in mm
    virtual double getWidth() const;
    /// Returns template height in mm
    virtual double getHeight() const;

    virtual DrawPage* getParentPage() const;
    virtual std::pair<int, int> getPageNumbers() const;

    virtual QString getAutofillValue(const QString &id) const;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const override{
        return "TechDrawGui::ViewProviderTemplate";
    }

    // from base class
    PyObject *getPyObject(void) override;

    class Autofill
    {
        public:
            static constexpr const char *Author       = "author";
            static constexpr const char *Date         = "date";
            static constexpr const char *Organization = "organization";
            static constexpr const char *Organisation = "organisation";
            static constexpr const char *Owner        = "owner";
            static constexpr const char *Company      = "company";
            static constexpr const char *Scale        = "scale";
            static constexpr const char *Sheet        = "sheet";
            static constexpr const char *Title        = "title";
            static constexpr const char *PageNumber   = "page_number";
            static constexpr const char *PageCount    = "page_count";
    };

private:
    static const char* OrientationEnums[];

};

using DrawTemplatePython = App::FeaturePythonT<DrawTemplate>;

} //namespace TechDraw



#endif //TECHDRAW_DrawTemplate_h_
