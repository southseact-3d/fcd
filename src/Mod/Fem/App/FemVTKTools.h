#ifndef FEM_VTK_TOOLS_H
#define FEM_VTK_TOOLS_H

#include <vtkDataSet.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <App/DocumentObject.h>

#include "FemMeshObject.h"


namespace Fem
{
// utility class to import/export read/write vtk mesh and result
class FemExport FemVTKTools
{
public:
    // extract data from vtkUnstructuredGrid instance and fill a FreeCAD FEM mesh object with that
    // data
    static void importVTKMesh(vtkSmartPointer<vtkDataSet> grid, FemMesh* mesh, float scale = 1.0);

    // extract data from FreCAD FEM mesh and fill a vtkUnstructuredGrid instance with that data. Set
    // `highest` to false to export all elements levels.
    static void exportVTKMesh(
        const FemMesh* mesh,
        vtkSmartPointer<vtkUnstructuredGrid> grid,
        bool highest = true,
        float scale = 1.0
    );

    // extract data from vtkUnstructuredGrid object and fill a FreeCAD FEM result object with that
    // data (needed by readResult)
    static void importFreeCADResult(vtkSmartPointer<vtkDataSet> dataset, App::DocumentObject* result);

    // extract data from a FreeCAD FEM result object and fill a vtkUnstructuredGrid object with that
    // data (needed by writeResult)
    static void exportFreeCADResult(const App::DocumentObject* result, vtkSmartPointer<vtkDataSet> grid);

    // FemMesh read from vtkUnstructuredGrid data file
    static FemMesh* readVTKMesh(const char* filename, FemMesh* mesh);

    // FemMesh write to vtkUnstructuredGrid data file
    static void writeVTKMesh(const char* Filename, const FemMesh* mesh, bool highest = true);

    // FemResult (activeObject or created if res= NULL) read from vtkUnstructuredGrid dataset file
    static App::DocumentObject* readResult(const char* Filename, App::DocumentObject* res = nullptr);

    // write FemResult (activeObject if res= NULL) to vtkUnstructuredGrid dataset file
    static void writeResult(const char* filename, const App::DocumentObject* res = nullptr);

    static void frdToVTK(const char* filename, bool binary = true);
};
}  // namespace Fem

#endif  // FEM_VTK_TOOLS_H
