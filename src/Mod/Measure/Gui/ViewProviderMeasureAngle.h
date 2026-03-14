#ifndef GUI_VIEWPROVIDERMEASUREANGLE_H
#define GUI_VIEWPROVIDERMEASUREANGLE_H

#include <Mod/Measure/MeasureGlobal.h>

#include <QObject>

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFMatrix.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <Mod/Measure/App/MeasureAngle.h>

#include "ViewProviderMeasureBase.h"

// NOLINTBEGIN
class SoText2;
class SoTranslation;
class SoCoordinate3;
class SoIndexedLineSet;
class SoTransform;
// NOLINTEND

namespace MeasureGui
{

class MeasureGuiExport ViewProviderMeasureAngle: public MeasureGui::ViewProviderMeasureBase
{
    PROPERTY_HEADER_WITH_OVERRIDE(MeasureGui::ViewProviderMeasureAngle);

public:
    /// Constructor
    ViewProviderMeasureAngle();

    Measure::MeasureAngle* getMeasureAngle();
    void redrawAnnotation() override;
    void positionAnno(const Measure::MeasureBase* measureObject) override;

private:
    // Fields
    SoSFFloat fieldAngle;  // radians.

    SbMatrix getMatrix();
};


}  // namespace MeasureGui


#endif  // GUI_VIEWPROVIDERMEASUREANGLE_H
