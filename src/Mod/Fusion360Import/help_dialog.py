# SPDX-License-Identifier: LGPL-2.1-or-later
"""In-app help dialog showing the Fusion 360 import setup instructions.

The same content is also available as :file:`README.md` for users who
prefer to read the docs in their editor.
"""

from __future__ import annotations

from PySide import QtWidgets


README_TEXT = """\
<h2>Fusion 360 Import for Tungsten CAD</h2>
<p>
This workbench lets you open a Fusion 360 design and rebuild it as a
fully parametric Tungsten CAD document - sketches, parameters and
feature tree are all reconstructed.
</p>

<h3>One-time setup</h3>
<ol>
  <li>Make sure Fusion 360 is installed and licensed on this machine.</li>
  <li>Download the
    <a href="https://github.com/faust-machines/fusion360-mcp-server">fusion360-mcp-server</a>
    repository.</li>
  <li>Copy the <code>Fusion360MCP</code> folder from the repo into the
    Fusion add-ins directory:
    <ul>
      <li><b>Windows:</b>
        <code>%APPDATA%\\Autodesk\\Autodesk Fusion 360\\API\\AddIns\\</code></li>
      <li><b>macOS:</b>
        <code>~/Library/Application Support/Autodesk/Autodesk Fusion 360/API/AddIns/</code></li>
    </ul>
  </li>
  <li>In Fusion 360, press <b>Shift+S</b> to open the Add-Ins dialog,
    select <b>Fusion360MCP</b> and tick <i>Run on Startup</i>.</li>
  <li>Confirm the add-in is running - the TEXT COMMANDS window should
    show <code>[MCP] Server listening on localhost:9876</code>.</li>
  <li>Open the design you want to import in Fusion 360 and leave it
    open in the foreground.</li>
</ol>

<h3>Importing a design</h3>
<ol>
  <li>Switch to the <b>Fusion 360 Import</b> workbench in Tungsten CAD.</li>
  <li>Click <b>Connect to Fusion 360...</b> and verify the host
    (default <code>127.0.0.1</code>) and port (default <code>9876</code>).</li>
  <li>Click <b>Test connection</b> to confirm the add-in responds.</li>
  <li>Click <b>Import design from Fusion 360</b>. The progress
    dialog will show each step:
    <ul>
      <li>Connecting to the MCP add-in</li>
      <li>Sending the extraction script to Fusion</li>
      <li>Parsing the JSON timeline</li>
      <li>Copying the exported STEP file</li>
      <li>Rebuilding sketches, then features</li>
    </ul>
  </li>
  <li>The result dialog tells you how many features and sketches were
    created, and lists anything that was skipped (for example
    rectangular patterns, which are reconstructed as comments because
    their internal references are not yet mapped).</li>
</ol>

<h3>What gets reconstructed</h3>
<table border="1" cellpadding="4" cellspacing="0">
  <tr><th>Fusion 360 feature</th><th>Tungsten CAD feature</th></tr>
  <tr><td>Extrude (add)</td><td>PartDesign::Pad</td></tr>
  <tr><td>Extrude (cut)</td><td>PartDesign::Pocket</td></tr>
  <tr><td>Revolve (add)</td><td>PartDesign::Revolution</td></tr>
  <tr><td>Revolve (cut)</td><td>PartDesign::Groove</td></tr>
  <tr><td>Fillet</td><td>PartDesign::Fillet</td></tr>
  <tr><td>Chamfer</td><td>PartDesign::Chamfer</td></tr>
  <tr><td>Hole</td><td>PartDesign::Hole</td></tr>
  <tr><td>Shell</td><td>PartDesign::Thickness</td></tr>
  <tr><td>Box primitive</td><td>PartDesign::AdditiveBox</td></tr>
  <tr><td>Cylinder primitive</td><td>PartDesign::AdditiveCylinder</td></tr>
  <tr><td>Sketch + constraints</td><td>Sketcher::SketchObject</td></tr>
  <tr><td>User/Model parameters</td><td>Spreadsheet parameters + properties</td></tr>
</table>

<h3>Limitations</h3>
<ul>
  <li>Rectangular and circular patterns are recorded but not yet
    rebuilt - the deep reference mapping needs to be added.</li>
  <li>Mirrored features are recorded but not yet rebuilt.</li>
  <li>Loft, sweep and coil features are recorded but not yet rebuilt.</li>
  <li>Fillet/chamfer edge selection is set to "use all edges" as a
    first pass - refine the selection in the GUI if needed.</li>
  <li>Assemblies are flattened into a single body. Multi-body
    reconstruction is on the roadmap.</li>
</ul>

<h3>Troubleshooting</h3>
<ul>
  <li><b>Cannot connect:</b> Check that Fusion 360 is open and the
    Fusion360MCP add-in is running. The TEXT COMMANDS window in
    Fusion should show the listening port.</li>
  <li><b>Empty timeline:</b> Switch to the Design workspace in Fusion
    360 - the script only reads from the active parametric design.</li>
  <li><b>STEP export failed:</b> Some custom material databases can
    break Fusion's STEP exporter. Try exporting manually first; if it
    fails, simplify the material in Fusion.</li>
  <li><b>Sketch misaligned:</b> If Fusion's reference plane is not
    XY/XZ/YZ the rebuilder falls back to XY. Reattach the sketch in
    the GUI after import.</li>
</ul>
"""


def show(parent: QtWidgets.QWidget | None = None) -> None:
    """Open the help dialog modally."""
    dialog = QtWidgets.QDialog(parent)
    dialog.setWindowTitle("Fusion 360 Import - Help")
    dialog.resize(640, 640)

    layout = QtWidgets.QVBoxLayout(dialog)
    text = QtWidgets.QTextBrowser()
    text.setOpenExternalLinks(True)
    text.setHtml(README_TEXT)
    layout.addWidget(text)

    button_box = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok)
    button_box.accepted.connect(dialog.accept)
    layout.addWidget(button_box)

    dialog.exec_()