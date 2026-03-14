#ifndef FEATURE_MESH_IMPORT_H
#define FEATURE_MESH_IMPORT_H

#include "MeshFeature.h"

#include <App/PropertyFile.h>


namespace Mesh
{

/**
 * The Import class reads the any supported mesh format
 * into the FreeCAD workspace.
 * @author Werner Mayer
 */
class Import: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Import);

public:
    Import();

    App::PropertyFile FileName;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}
};

}  // namespace Mesh

#endif  // FEATURE_MESH_IMPORT_H
