"""Tungsten CAD Update module — GUI registration.

Adds a "Check for Updates" command to the Help menu across all workbenches.
"""

import threading

try:
    import FreeCAD
    import FreeCADGui

    class _UpdateCommand:
        """Command that checks for and applies dev patch updates."""

        def Activated(self):
            from PySide import QtCore, QtGui

            mw = FreeCADGui.getMainWindow()

            # Run the check in a thread to avoid blocking the UI
            def do_check():
                from .updater import check_for_updates
                result = check_for_updates()
                QtCore.QMetaObject.invokeMethod(
                    _main, "onCheckDone", QtCore.Qt.QueuedConnection,
                    QtCore.Q_ARG(QtCore.QVariant, result),
                )

            # Worker object for cross-thread signal delivery
            class _Worker(QtCore.QObject):
                @QtCore.Slot(QtCore.QVariant)
                def onCheckDone(self, result):
                    result = result.toVariant() if hasattr(result, "toVariant") else result
                    self._show_result(mw, result)

                def _show_result(self, mw, result):
                    if not result.get("available"):
                        QtGui.QMessageBox.information(
                            mw, "Check for Updates", result.get("message", "No updates found.")
                        )
                        return

                    reply = QtGui.QMessageBox.question(
                        mw,
                        "Check for Updates",
                        result["message"] + "\n\nDownload and apply?",
                        QtGui.QMessageBox.Yes | QtGui.QMessageBox.No,
                    )

                    if reply == QtGui.QMessageBox.Yes:
                        self._apply_patch(mw)

            _main = _Worker()
            _main.setParent(mw)

            t = threading.Thread(target=do_check, daemon=True)
            t.start()

        def _apply_patch(self, mw):
            from PySide import QtCore, QtGui

            progress = QtGui.QProgressDialog("Downloading update...", "Cancel", 0, 0, mw)
            progress.setWindowTitle("Applying Update")
            progress.setWindowModality(QtCore.Qt.WindowModal)
            progress.show()

            def do_apply():
                from .updater import download_and_apply_patch

                def on_progress(msg):
                    QtCore.QMetaObject.invokeMethod(
                        _main2, "updateProgress",
                        QtCore.Qt.QueuedConnection,
                        QtCore.Q_ARG(str, msg),
                    )

                success, message = download_and_apply_patch(progress_callback=on_progress)
                QtCore.QMetaObject.invokeMethod(
                    _main2, "onApplyDone",
                    QtCore.Qt.QueuedConnection,
                    QtCore.Q_ARG(bool, success),
                    QtCore.Q_ARG(str, message),
                )

            class _ApplyWorker(QtCore.QObject):
                @QtCore.Slot(str)
                def updateProgress(self, msg):
                    progress.setLabelText(msg)

                @QtCore.Slot(bool, str)
                def onApplyDone(self, success, message):
                    progress.close()
                    if success:
                        QtGui.QMessageBox.information(mw, "Update", message)
                    else:
                        QtGui.QMessageBox.warning(mw, "Update Failed", message)

            _main2 = _ApplyWorker()
            _main2.setParent(mw)

            t = threading.Thread(target=do_apply, daemon=True)
            t.start()

        def GetResources(self):
            return {
                "Pixmap": os.path.join(os.path.dirname(__file__), "icon.svg"),
                "MenuText": "Check for Updates...",
                "ToolTip": "Check for and install dev patch updates from GitHub Actions",
            }

        def IsActive(self):
            return True

    import os

    FreeCADGui.addCommand("Tungsten_UpdateCheck", _UpdateCommand())

    # ---------------------------------------------------------------------------
    # Add the command to the Help menu across all workbenches
    # ---------------------------------------------------------------------------
    class _UpdateMenuManipulator:
        def modifyMenuBar(self):
            return [
                {"insert": "Tungsten_UpdateCheck", "menuItem": "Std_About", "after": ""},
            ]

    FreeCADGui.addWorkbenchManipulator(_UpdateMenuManipulator())

except ImportError:
    # Running headless or FreeCADGui not available — skip GUI registration
    pass
