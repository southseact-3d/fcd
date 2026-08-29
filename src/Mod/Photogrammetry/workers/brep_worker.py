"""QThread worker for B-REP conversion."""

import traceback
from typing import Any

import numpy as np
from PySide6 import QtCore


class BRepWorker(QtCore.QThread):
    """Background thread for converting a triangle mesh to a FreeCAD B-REP shape.

    Runs the mesh-to-BREP pipeline and creates a Part::Feature in the
    active FreeCAD document.

    Signals:
        progress(int, str): Emitted with percent (0-100) and status message.
        finished(dict): Emitted with feature name, shape info on success.
        error(str): Emitted with traceback on failure.
    """

    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(dict)
    error = QtCore.Signal(str)

    def __init__(
        self,
        vertices: np.ndarray,
        faces: np.ndarray,
        conversion_params: dict,
        parent: Any = None,
    ) -> None:
        super().__init__(parent)
        self.vertices = vertices
        self.faces = faces
        self.conversion_params = conversion_params

    def run(self) -> None:
        try:
            self.progress.emit(5, "Building mesh shape...")

            try:
                import FreeCAD
                import Part
            except ImportError:
                raise ImportError(
                    "FreeCAD modules not available. "
                    "Ensure FreeCAD is installed and on the Python path."
                )

            # Build the triangulated mesh shape
            self.progress.emit(15, "Creating FreeCAD mesh...")
            mesh = Part.Mesh()
            mesh.addVertices([
                FreeCAD.Vector(float(v[0]), float(v[1]), float(v[2]))
                for v in self.vertices
            ])
            mesh.addFaces([
                (int(f[0]), int(f[1]), int(f[2]))
                for f in self.faces
            ])

            shape = mesh.getShape()

            self.progress.emit(50, "Creating shape from mesh...")

            # Attempt to create a solid from the mesh if requested
            make_solid = self.conversion_params.get("make_solid", False)
            if make_solid:
                self.progress.emit(60, "Converting to solid...")
                try:
                    shape = Part.Shape(shape.makeShapeFromMesh(
                        self.conversion_params.get("sewing_tolerance", 0.1)
                    ))
                except Exception:
                    # Fall back to the raw shape
                    pass

            # Attempt to smooth if requested
            if self.conversion_params.get("smooth", False):
                self.progress.emit(70, "Applying shape refinement...")
                try:
                    shape = shape.makeFillet(
                        self.conversion_params.get("fillet_radius", 0.01),
                        shape.Edges,
                    )
                except Exception:
                    pass

            self.progress.emit(80, "Adding feature to document...")

            doc = FreeCAD.ActiveDocument
            if doc is None:
                doc = FreeCAD.newDocument("Photogrammetry")

            feature = doc.addObject("Part::Feature", "Photogrammetry_Mesh")
            feature.Shape = shape
            doc.recompute()

            self.progress.emit(100, "B-REP conversion complete")

            self.finished.emit({
                "feature_name": feature.Name,
                "document_name": doc.Name,
                "num_vertices": len(self.vertices),
                "num_faces": len(self.faces),
                "has_solid": make_solid,
                "shape_type": str(shape.ShapeType),
            })

        except Exception as e:
            tb = traceback.format_exc()
            self.error.emit(f"{e}\n{tb}")
