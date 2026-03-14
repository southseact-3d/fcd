#ifndef IMPORT_STEPSHAPE_H
#define IMPORT_STEPSHAPE_H

#include <Mod/Import/ImportGlobal.h>

namespace Import
{

/** The StepShape helper class
 * The MeshFacet class provides an interface for the MeshFacetPy class for
 * convenient access to the Mesh data structure. This class should not be used
 * for programming algorithms in C++. Use Mesh Core classes instead!
 */
class ImportExport StepShape
{
public:
    StepShape();
    ~StepShape();

    int read(const char* fileName);
};

}  // namespace Import


#endif  // IMPORT_STEPSHAPE_H
