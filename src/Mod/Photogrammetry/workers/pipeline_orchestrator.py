"""Pipeline orchestrator for the Photogrammetry workbench.

Runs the full photogrammetry pipeline by sequencing QThread workers
and forwarding their progress/signals. Supports pause, resume, and cancel.
"""

import os
import traceback
from typing import Any

import numpy as np
from PySide6 import QtCore

from workers.sfm_worker import SfMWorker
from workers.dense_worker import DenseWorker
from workers.mesh_worker import MeshWorker
from workers.brep_worker import BRepWorker


# Stage indices
STAGE_IMPORT = 0
STAGE_FEATURE_EXTRACTION = 1
STAGE_FEATURE_MATCHING = 2
STAGE_SPARSE_RECONSTRUCTION = 3
STAGE_DENSE_RECONSTRUCTION = 4
STAGE_MESH_GENERATION = 5
STAGE_TEXTURE_MAPPING = 6
STAGE_BREP_CONVERSION = 7

STAGE_NAMES = {
    STAGE_IMPORT: "Import",
    STAGE_FEATURE_EXTRACTION: "Feature Extraction",
    STAGE_FEATURE_MATCHING: "Feature Matching",
    STAGE_SPARSE_RECONSTRUCTION: "Sparse Reconstruction",
    STAGE_DENSE_RECONSTRUCTION: "Dense Reconstruction",
    STAGE_MESH_GENERATION: "Mesh Generation",
    STAGE_TEXTURE_MAPPING: "Texture Mapping",
    STAGE_BREP_CONVERSION: "B-REP Conversion",
}


class PipelineOrchestrator(QtCore.QObject):
    """Orchestrates the full photogrammetry pipeline sequentially.

    Runs workers in order, stores intermediate results, and supports
    pause/resume/cancel at stage boundaries.

    Signals:
        stage_changed(int, str): Emitted when a stage starts or completes.
        progress_update(int, str): Emitted with percent and message from workers.
        pipeline_finished(dict): Emitted with all results on success.
        pipeline_error(str): Emitted with error message on failure.
    """

    stage_changed = QtCore.Signal(int, str)
    progress_update = QtCore.Signal(int, str)
    pipeline_finished = QtCore.Signal(dict)
    pipeline_error = QtCore.Signal(str)

    def __init__(
        self,
        zip_path: str,
        settings: dict,
        parent: Any = None,
    ) -> None:
        super().__init__(parent)
        self.zip_path = zip_path
        self.settings = settings

        self._paused = False
        self._cancelled = False
        self._current_stage = STAGE_IMPORT
        self._active_worker: QtCore.QThread | None = None

        # Intermediate results
        self._images_dir: str = ""
        self._poses: list = []
        self._calibration: dict = {}
        self._features: dict = {}
        self._matches: dict = {}
        self._sparse_result: dict = {}
        self._dense_result: dict = {}
        self._mesh_result: dict = {}
        self._brep_result: dict = {}

    @property
    def current_stage(self) -> int:
        return self._current_stage

    def start(self) -> None:
        """Begin the pipeline from the current stage."""
        self._paused = False
        self._cancelled = False
        self._run_stage(self._current_stage)

    def pause(self) -> None:
        """Pause after the current stage completes."""
        self._paused = True

    def cancel(self) -> None:
        """Cancel the pipeline at the next stage boundary."""
        self._cancelled = True

    def run_brep_conversion(self, params: dict) -> None:
        """Run B-REP conversion separately as an isolated stage."""
        if self._mesh_result is None:
            self.pipeline_error.emit("No mesh available for B-REP conversion.")
            return

        self._cancelled = False
        self._paused = False
        worker = BRepWorker(
            vertices=self._mesh_result["vertices"],
            faces=self._mesh_result["faces"],
            conversion_params=params,
            parent=self,
        )
        self._active_worker = worker
        worker.progress.connect(self._on_worker_progress)
        worker.finished.connect(self._on_brep_finished)
        worker.error.connect(self._on_worker_error)
        worker.start()

    def _run_stage(self, stage: int) -> None:
        """Dispatch the appropriate worker for the given stage."""
        if self._cancelled:
            self.pipeline_error.emit("Pipeline cancelled.")
            return

        if stage == STAGE_BREP_CONVERSION:
            # B-REP is optional and run separately
            self._advance_stage()
            return

        self._current_stage = stage
        self.stage_changed.emit(stage, f"Starting {STAGE_NAMES[stage]}...")

        try:
            if stage == STAGE_IMPORT:
                self._run_import()
            elif stage in (STAGE_FEATURE_EXTRACTION, STAGE_FEATURE_MATCHING,
                           STAGE_SPARSE_RECONSTRUCTION):
                self._run_sfm(stage)
            elif stage == STAGE_DENSE_RECONSTRUCTION:
                self._run_dense()
            elif stage == STAGE_MESH_GENERATION:
                self._run_mesh()
            elif stage == STAGE_TEXTURE_MAPPING:
                self._run_texture_mapping()
        except Exception as e:
            tb = traceback.format_exc()
            self.pipeline_error.emit(f"Stage '{STAGE_NAMES[stage]}' failed: {e}\n{tb}")

    def _advance_stage(self) -> None:
        """Move to the next stage if not paused or cancelled."""
        if self._cancelled:
            self.pipeline_error.emit("Pipeline cancelled.")
            return
        if self._paused:
            self.stage_changed.emit(
                self._current_stage,
                f"Paused at {STAGE_NAMES[self._current_stage]}",
            )
            return
        next_stage = self._current_stage + 1
        if next_stage > STAGE_TEXTURE_MAPPING:
            self.pipeline_finished.emit(self._collect_results())
        else:
            self._run_stage(next_stage)

    def _collect_results(self) -> dict:
        """Aggregate all intermediate results into a single dict."""
        return {
            "images_dir": self._images_dir,
            "poses": self._poses,
            "calibration": self._calibration,
            "features": self._features,
            "matches": self._matches,
            "sparse_result": self._sparse_result,
            "dense_result": self._dense_result,
            "mesh_result": self._mesh_result,
            "brep_result": self._brep_result,
        }

    # --- Stage implementations ---

    def _run_import(self) -> None:
        """Validate and extract the input zip file."""
        import zipfile

        if not os.path.isfile(self.zip_path):
            raise FileNotFoundError(f"Zip file not found: {self.zip_path}")

        extract_dir = self.settings.get("extract_dir", "")
        if not extract_dir:
            extract_dir = os.path.join(
                os.path.dirname(self.zip_path),
                "photogrammetry_extracted",
            )

        self.progress_update.emit(0, "Extracting zip archive...")

        with zipfile.ZipFile(self.zip_path, "r") as zf:
            zf.extractall(extract_dir)

        self._images_dir = extract_dir

        # Try to load metadata from zip
        meta_path = os.path.join(extract_dir, "metadata.json")
        if os.path.isfile(meta_path):
            import json
            with open(meta_path, "r") as f:
                meta = json.load(f)
            self._poses = meta.get("poses", [])
            self._calibration = meta.get("calibration", {})
        else:
            # Try to load from poses.json
            poses_path = os.path.join(extract_dir, "poses.json")
            if os.path.isfile(poses_path):
                import json
                with open(poses_path, "r") as f:
                    self._poses = json.load(f)

        self.progress_update.emit(100, "Import complete")
        self.stage_changed.emit(STAGE_IMPORT, "Import complete")
        self._advance_stage()

    def _run_sfm(self, start_stage: int) -> None:
        """Run the SfM pipeline starting from the given stage.

        The SfMWorker handles extraction, matching, and sparse
        reconstruction as a single pipeline.
        """
        worker = SfMWorker(
            images_dir=self._images_dir,
            poses=self._poses,
            calibration=self._calibration,
            settings=self.settings,
            parent=self,
        )
        self._active_worker = worker
        worker.progress.connect(self._on_worker_progress)
        worker.finished.connect(self._on_sfm_finished)
        worker.error.connect(self._on_worker_error)
        worker.start()

    def _on_sfm_finished(self, results: dict) -> None:
        """Handle SfM completion and advance."""
        self._sparse_result = results
        self._features = results.get("features", {})
        self._matches = results.get("matches", {})
        self.stage_changed.emit(
            STAGE_SPARSE_RECONSTRUCTION, "Sparse reconstruction complete"
        )
        self._active_worker = None
        self._advance_stage()

    def _run_dense(self) -> None:
        """Run dense reconstruction using sparse results."""
        camera_poses = self._sparse_result.get("camera_poses", {})
        calibration = self._sparse_result.get("calibration", self._calibration)

        worker = DenseWorker(
            images_dir=self._images_dir,
            camera_poses=camera_poses,
            calibration=calibration,
            settings=self.settings,
            parent=self,
        )
        self._active_worker = worker
        worker.progress.connect(self._on_worker_progress)
        worker.finished.connect(self._on_dense_finished)
        worker.error.connect(self._on_worker_error)
        worker.start()

    def _on_dense_finished(self, results: dict) -> None:
        """Handle dense reconstruction completion."""
        self._dense_result = results
        self.stage_changed.emit(
            STAGE_DENSE_RECONSTRUCTION, "Dense reconstruction complete"
        )
        self._active_worker = None
        self._advance_stage()

    def _run_mesh(self) -> None:
        """Run mesh generation using dense point cloud."""
        dense_cloud = self._dense_result.get(
            "dense_cloud", self._sparse_result.get("point_cloud", np.empty((0, 3)))
        )
        colors = self._dense_result.get(
            "colors",
            self._sparse_result.get(
                "point_colors", np.empty((0, 3), dtype=np.uint8)
            ),
        )

        worker = MeshWorker(
            point_cloud=dense_cloud,
            colors=colors,
            settings=self.settings,
            parent=self,
        )
        self._active_worker = worker
        worker.progress.connect(self._on_worker_progress)
        worker.finished.connect(self._on_mesh_finished)
        worker.error.connect(self._on_worker_error)
        worker.start()

    def _on_mesh_finished(self, results: dict) -> None:
        """Handle mesh generation completion."""
        self._mesh_result = results
        self.stage_changed.emit(STAGE_MESH_GENERATION, "Mesh generation complete")
        self._active_worker = None
        self._advance_stage()

    def _run_texture_mapping(self) -> None:
        """Texture mapping stage (placeholder for future implementation)."""
        self.progress_update.emit(50, "Texture mapping not yet implemented")
        self.stage_changed.emit(STAGE_TEXTURE_MAPPING, "Texture mapping skipped")
        self._advance_stage()

    def _on_brep_finished(self, results: dict) -> None:
        """Handle B-REP conversion completion."""
        self._brep_result = results
        self.stage_changed.emit(STAGE_BREP_CONVERSION, "B-REP conversion complete")
        self._active_worker = None

    # --- Signal forwarding ---

    def _on_worker_progress(self, pct: int, msg: str) -> None:
        """Forward worker progress signals."""
        self.progress_update.emit(pct, msg)

    def _on_worker_error(self, msg: str) -> None:
        """Handle worker errors."""
        self._active_worker = None
        self.pipeline_error.emit(
            f"Worker failed at stage '{STAGE_NAMES.get(self._current_stage, '?')}': {msg}"
        )
