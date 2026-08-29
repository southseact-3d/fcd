# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Import dialog for photogrammetry scan zip files.

Provides a PySide6 dialog that allows users to browse for and preview
photogrammetry scan archives before importing them into the pipeline.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import io
import json
import zipfile
from pathlib import Path
from typing import Any, Dict, List, Optional

from PySide6 import QtCore, QtGui, QtWidgets


# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

_IMAGE_EXTENSIONS = frozenset({".jpg", ".jpeg", ".png", ".bmp", ".tiff", ".tif", ".webp"})
_MAX_PREVIEW_IMAGES = 20
_THUMBNAIL_SIZE = 128


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _make_label(text: str, tooltip: str = "", bold: bool = False) -> QtWidgets.QLabel:
    """Create a styled QLabel."""
    lbl = QtWidgets.QLabel(text)
    if tooltip:
        lbl.setToolTip(tooltip)
    if bold:
        font = lbl.font()
        font.setBold(True)
        lbl.setFont(font)
    return lbl


def _format_file_size(size_bytes: int) -> str:
    """Format a byte count into a human-readable string."""
    for unit in ("B", "KB", "MB", "GB"):
        if size_bytes < 1024.0:
            return f"{size_bytes:.1f} {unit}"
        size_bytes /= 1024.0
    return f"{size_bytes:.1f} TB"


# ---------------------------------------------------------------------------
# Image thumbnail widget
# ---------------------------------------------------------------------------

class _ThumbnailWidget(QtWidgets.QLabel):
    """Displays a single image thumbnail in the grid."""

    def __init__(
        self,
        pixmap: QtGui.QPixmap,
        filename: str,
        parent: Optional[QtWidgets.QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._filename = filename
        scaled = pixmap.scaled(
            _THUMBNAIL_SIZE,
            _THUMBNAIL_SIZE,
            QtCore.Qt.KeepAspectRatio,
            QtCore.Qt.SmoothTransformation,
        )
        self.setPixmap(scaled)
        self.setFixedSize(_THUMBNAIL_SIZE, _THUMBNAIL_SIZE)
        self.setStyleSheet("border: 1px solid #555; background-color: #2a2a2a;")
        self.setToolTip(filename)
        self.setAlignment(QtCore.Qt.AlignCenter)


# ---------------------------------------------------------------------------
# Camera pose graphics view
# ---------------------------------------------------------------------------

class _CameraPoseScene(QtWidgets.QGraphicsScene):
    """Simple 2D scatter plot of camera positions."""

    def __init__(self, parent: Optional[QtCore.QObject] = None) -> None:
        super().__init__(parent)
        self.setSceneRect(-500, -500, 1000, 1000)

    def draw_camera_poses(self, positions: List[tuple[float, float]]) -> None:
        """Draw camera positions as small dots."""
        self.clear()
        if not positions:
            self.addText("No camera pose data available")
            return

        xs = [p[0] for p in positions]
        ys = [p[1] for p in positions]
        min_x, max_x = min(xs), max(xs)
        min_y, max_y = min(ys), max(ys)
        range_x = max_x - min_x if max_x != min_x else 1.0
        range_y = max_y - min_y if max_y != min_y else 1.0

        scale = min(900.0 / range_x, 900.0 / range_y)
        cx = (min_x + max_x) / 2.0
        cy = (min_y + max_y) / 2.0

        pen = QtGui.QPen(QtCore.Qt.NoPen)
        brush = QtGui.QBrush(QtGui.QColor(0, 160, 255))

        for x, y in positions:
            sx = (x - cx) * scale
            sy = (y - cy) * scale
            self.addEllipse(sx - 3, sy - 3, 6, 6, pen, brush)

        self.addText(
            f"{len(positions)} cameras  |  "
            f"X: [{min_x:.1f}, {max_x:.1f}]  Y: [{min_y:.1f}, {max_y:.1f}]"
        )


# ---------------------------------------------------------------------------
# Import dialog
# ---------------------------------------------------------------------------

class ImportDialog(QtWidgets.QDialog):
    """Dialog for importing a photogrammetry scan zip file.

    Presents file selection, image preview, metadata display, and a 2D
    camera pose visualization.
    """

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self.setWindowTitle("Import Photogrammetry Scan")
        self.setMinimumSize(600, 500)

        self._zip_path: Optional[str] = None
        self._metadata: Dict[str, Any] = {}
        self._image_names: List[str] = []

        self._build_ui()
        self._connect_signals()

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------

    def _build_ui(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(8, 8, 8, 8)
        root.setSpacing(6)

        file_row = QtWidgets.QHBoxLayout()
        self._path_edit = QtWidgets.QLineEdit()
        self._path_edit.setPlaceholderText("Select a photogrammetry scan .zip file")
        self._path_edit.setReadOnly(True)
        self._browse_btn = QtWidgets.QPushButton("Browse...")
        self._browse_btn.setMaximumWidth(80)
        file_row.addWidget(self._path_edit, 1)
        file_row.addWidget(self._browse_btn)
        root.addLayout(file_row)

        self._status_label = _make_label("")
        self._status_label.setStyleSheet("color: grey; font-size: 10px;")
        root.addWidget(self._status_label)

        self._tabs = QtWidgets.QTabWidget()

        preview_tab = QtWidgets.QWidget()
        preview_layout = QtWidgets.QVBoxLayout(preview_tab)
        self._preview_scroll = QtWidgets.QScrollArea()
        self._preview_scroll.setWidgetResizable(True)
        self._preview_container = QtWidgets.QWidget()
        self._preview_grid = QtWidgets.QGridLayout(self._preview_container)
        self._preview_grid.setSpacing(4)
        self._preview_scroll.setWidget(self._preview_container)
        preview_layout.addWidget(self._preview_scroll)
        self._tabs.addTab(preview_tab, "Preview")

        info_tab = QtWidgets.QWidget()
        info_layout = QtWidgets.QVBoxLayout(info_tab)
        self._info_table = QtWidgets.QTableWidget()
        self._info_table.setColumnCount(2)
        self._info_table.setHorizontalHeaderLabels(["Property", "Value"])
        self._info_table.horizontalHeader().setStretchLastSection(True)
        self._info_table.setEditTriggers(QtWidgets.QAbstractItemView.NoEditTriggers)
        self._info_table.setSelectionMode(QtWidgets.QAbstractItemView.SingleSelection)
        self._info_table.verticalHeader().setVisible(False)
        info_layout.addWidget(self._info_table)
        self._tabs.addTab(info_tab, "Info")

        cameras_tab = QtWidgets.QWidget()
        cameras_layout = QtWidgets.QVBoxLayout(cameras_tab)
        self._camera_scene = _CameraPoseScene()
        self._camera_view = QtWidgets.QGraphicsView(self._camera_scene)
        self._camera_view.setRenderHint(QtGui.QPainter.Antialiasing)
        self._camera_view.setDragMode(QtWidgets.QGraphicsView.ScrollHandDrag)
        cameras_layout.addWidget(self._camera_view)
        self._tabs.addTab(cameras_tab, "Cameras")

        root.addWidget(self._tabs, 1)

        self._button_box = QtWidgets.QDialogButtonBox(
            QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
        )
        self._button_box.button(QtWidgets.QDialogButtonBox.Ok).setEnabled(False)
        root.addWidget(self._button_box)

    def _connect_signals(self) -> None:
        self._browse_btn.clicked.connect(self._on_browse)
        self._button_box.accepted.connect(self.accept)
        self._button_box.rejected.connect(self.reject)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_browse(self) -> None:
        path, _ = QtWidgets.QFileDialog.getOpenFileName(
            self,
            "Select Photogrammetry Scan",
            "",
            "Zip Archives (*.zip);;All Files (*)",
        )
        if not path:
            return
        self._load_zip(path)

    def _load_zip(self, path: str) -> None:
        self._zip_path = None
        self._metadata = {}
        self._image_names = []

        try:
            z = zipfile.ZipFile(path, "r")
        except (zipfile.BadZipFile, OSError) as exc:
            self._status_label.setText(f"Error: {exc}")
            self._status_label.setStyleSheet("color: red; font-size: 10px;")
            self._button_box.button(QtWidgets.QDialogButtonBox.Ok).setEnabled(False)
            return

        with z:
            names = z.namelist()
            image_files = [n for n in names if Path(n).suffix.lower() in _IMAGE_EXTENSIONS]
            json_files = [n for n in names if n.endswith(".json")]
            total_size = sum(i.file_size for i in z.infolist())

            if not image_files:
                self._status_label.setText("Error: No images found in archive")
                self._status_label.setStyleSheet("color: red; font-size: 10px;")
                self._button_box.button(QtWidgets.QDialogButtonBox.Ok).setEnabled(False)
                return

            self._image_names = image_files
            self._zip_path = path
            self._path_edit.setText(path)

            metadata: Dict[str, Any] = {
                "Archive": path,
                "Total size": _format_file_size(total_size),
                "Image count": str(len(image_files)),
                "Total files": str(len(names)),
            }

            for jf in json_files[:3]:
                try:
                    with z.open(jf) as f:
                        data = json.loads(f.read().decode("utf-8", errors="replace"))
                    if isinstance(data, dict):
                        for key, val in data.items():
                            if isinstance(val, (str, int, float, bool)):
                                metadata[f"{Path(jf).stem}/{key}"] = str(val)
                except Exception:
                    pass

            self._metadata = metadata

            self._fill_info_table(metadata)
            self._load_thumbnails(z, image_files)
            self._load_camera_poses(z)

            self._status_label.setText(
                f"Loaded {len(image_files)} images from {Path(path).name}"
            )
            self._status_label.setStyleSheet("color: green; font-size: 10px;")
            self._button_box.button(QtWidgets.QDialogButtonBox.Ok).setEnabled(True)

    def _fill_info_table(self, metadata: Dict[str, Any]) -> None:
        self._info_table.setRowCount(len(metadata))
        for row, (key, val) in enumerate(metadata.items()):
            self._info_table.setItem(row, 0, QtWidgets.QTableWidgetItem(key))
            self._info_table.setItem(row, 1, QtWidgets.QTableWidgetItem(val))
        self._info_table.resizeColumnsToContents()

    def _load_thumbnails(self, z: zipfile.ZipFile, image_files: List[str]) -> None:
        while self._preview_grid.count():
            item = self._preview_grid.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        cols = max(1, (self._preview_scroll.viewport().width() - 20) // (_THUMBNAIL_SIZE + 8))
        for idx, name in enumerate(image_files[:_MAX_PREVIEW_IMAGES]):
            try:
                with z.open(name) as f:
                    raw = f.read()
                pixmap = QtGui.QPixmap()
                pixmap.loadFromData(raw)
                if not pixmap.isNull():
                    widget = _ThumbnailWidget(pixmap, Path(name).name)
                    row, col = divmod(idx, cols)
                    self._preview_grid.addWidget(widget, row, col)
            except Exception:
                pass

    def _load_camera_poses(self, z: zipfile.ZipFile) -> None:
        positions: List[tuple[float, float]] = []
        for name in z.namelist():
            if not name.endswith(".json"):
                continue
            try:
                with z.open(name) as f:
                    data = json.loads(f.read().decode("utf-8", errors="replace"))
                if isinstance(data, dict):
                    cameras = data.get("cameras") or data.get("poses") or []
                    if isinstance(cameras, list):
                        for cam in cameras:
                            if isinstance(cam, dict):
                                pos = cam.get("position") or cam.get("translation")
                                if isinstance(pos, (list, tuple)) and len(pos) >= 2:
                                    positions.append((float(pos[0]), float(pos[1])))
            except Exception:
                pass
        self._camera_scene.draw_camera_poses(positions)

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def get_zip_path(self) -> Optional[str]:
        """Return the selected zip path, or None if nothing was selected."""
        return self._zip_path
