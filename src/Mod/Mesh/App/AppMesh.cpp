#include <App/Application.h>
#include <App/MeasureManager.h>
#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "EdgePy.h"
#include "Exporter.h"
#include "FacetPy.h"
#include "FeatureMeshCurvature.h"
#include "FeatureMeshDefects.h"
#include "FeatureMeshExport.h"
#include "FeatureMeshImport.h"
#include "FeatureMeshSegmentByMesh.h"
#include "FeatureMeshSetOperations.h"
#include "FeatureMeshSolid.h"
#include "FeatureMeshTransform.h"
#include "FeatureMeshTransformDemolding.h"
#include "Mesh.h"
#include "MeshFeaturePy.h"
#include "MeshPointPy.h"
#include "MeshPy.h"


namespace Mesh
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(Mesh)
{
    PyObject* meshModule = Mesh::initModule();
    Base::Console().log("Loading Mesh module... done\n");

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
    ParameterGrp::handle handle = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Mesh"
    );
    ParameterGrp::handle asy = handle->GetGroup("Asymptote");
    MeshCore::MeshOutput::SetAsymptoteSize(asy->GetASCII("Width", "500"), asy->GetASCII("Height"));

    // clang-format off
    // add mesh elements
    Base::Interpreter().addType(&Mesh::MeshPointPy  ::Type,meshModule,"MeshPoint");
    Base::Interpreter().addType(&Mesh::EdgePy       ::Type,meshModule,"Edge");
    Base::Interpreter().addType(&Mesh::FacetPy      ::Type,meshModule,"Facet");
    Base::Interpreter().addType(&Mesh::MeshPy       ::Type,meshModule,"Mesh");
    Base::Interpreter().addType(&Mesh::MeshFeaturePy::Type,meshModule,"Feature");

    Mesh::Extension3MFFactory::addProducer(new Mesh::GuiExtension3MFProducer);

    // This registration is sufficient to allow one to measure free distances with a mesh
    App::MeasureManager::addMeasureHandler("Mesh", [](App::DocumentObject*, const char*) {
        return App::MeasureElementType::INVALID;
    });

    // init Type system
    Mesh::PropertyNormalList    ::init();
    Mesh::PropertyCurvatureList ::init();
    Mesh::PropertyMaterial      ::init();
    Mesh::PropertyMeshKernel    ::init();

    Mesh::MeshObject            ::init();
    Mesh::MeshSegment           ::init();

    Mesh::Feature               ::init();
    Mesh::FeatureCustom         ::init();
    Mesh::FeaturePython         ::init();
    Mesh::Import                ::init();
    Mesh::Export                ::init();
    Mesh::Transform             ::init();
    Mesh::TransformDemolding    ::init();
    Mesh::Curvature             ::init();
    Mesh::SegmentByMesh         ::init();
    Mesh::SetOperations         ::init();
    Mesh::FixDefects            ::init();
    Mesh::HarmonizeNormals      ::init();
    Mesh::FlipNormals           ::init();
    Mesh::FixNonManifolds       ::init();
    Mesh::FixDuplicatedFaces    ::init();
    Mesh::FixDuplicatedPoints   ::init();
    Mesh::FixDegenerations      ::init();
    Mesh::FixDeformations       ::init();
    Mesh::FixIndices            ::init();
    Mesh::FillHoles             ::init();
    Mesh::RemoveComponents      ::init();

    Mesh::Sphere                ::init();
    Mesh::Ellipsoid             ::init();
    Mesh::Cylinder              ::init();
    Mesh::Cone                  ::init();
    Mesh::Torus                 ::init();
    Mesh::Cube                  ::init();
    // clang-format on

    PyMOD_Return(meshModule);
}
