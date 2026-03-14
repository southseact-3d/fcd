#ifndef MESH_FEATUREMESHEXPORT_H
#define MESH_FEATUREMESHEXPORT_H

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>

#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif

namespace Mesh
{

/**
 * The Export class writes any supported mesh format into a file.
 * @author Werner Mayer
 */
class MeshExport Export: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Export);

public:
    Export();

    App::PropertyLink Source;
    App::PropertyString FileName;
    App::PropertyString Format;
    const char* getViewProviderName() const override
    {
        return "MeshGui::ViewProviderExport";
    }

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}
};

}  // namespace Mesh

#endif  // MESH_FEATUREMESHEXPORT_H
