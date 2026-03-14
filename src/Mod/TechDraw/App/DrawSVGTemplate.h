#ifndef TECHDRAW_DrawSVGTemplate_h_
#define TECHDRAW_DrawSVGTemplate_h_

# include <QDomDocument>

#include <App/DocumentObserver.h>
#include <App/FeaturePython.h>
#include <App/PropertyFile.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawTemplate.h"


namespace TechDraw
{

class TechDrawExport DrawSVGTemplate: public TechDraw::DrawTemplate,
                                      public App::DocumentObserver
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawSVGTemplate);

public:
    DrawSVGTemplate();
    ~DrawSVGTemplate() override;

    App::PropertyFileIncluded PageResult;
    App::PropertyFile Template;

    void onChanged(const App::Property* prop) override;
   /// returns the type name of the ViewProvider
    const char* getViewProviderName(void) const override {
        return "TechDrawGui::ViewProviderTemplate";
    }

    PyObject *getPyObject(void) override;

    double getWidth() const override;
    double getHeight() const override;

    QString processTemplate();
    void extractTemplateAttributes(QDomDocument& templateDocument);
    bool getTemplateDocument(std::string sourceFile, QDomDocument& templateDocument) const;
    QString getAutofillByEditableName(QString nameToMatch);

    void translateLabel(std::string context, std::string baseName, std::string uniqueName);


protected:
    void onSettingDocument() override;

    void replaceFileIncluded(std::string newTemplateFileName);
    std::map<std::string, std::string> getEditableTextsFromTemplate();

private:
    void slotCreatedObject(const App::DocumentObject& obj) override;
    void slotDeletedObject(const App::DocumentObject& obj) override;

};

using DrawSVGTemplatePython = App::FeaturePythonT<DrawSVGTemplate>;

} //namespace TechDraw

#endif //TECHDRAW_DrawSVGTemplate_h_
