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
fully parametric Tungsten CAD document - sketches, parameters,
patterns, mirror, boolean, fillet, chamfer, hole, primitives, datum
geometry, sub-components and more.
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
      <li>Parsing the JSON timeline (parameters, sketches,
        features, bodies, occurrences)</li>
      <li>Copying the exported STEP file</li>
      <li>Rebuilding parameters (Spreadsheet), then datums,
        sketches, features and patterns</li>
    </ul>
  </li>
  <li>The result dialog tells you how many features, sketches,
    datums and bodies were created, and lists anything that was
    skipped (for example ReplaceFace, which has no Tungsten CAD
    equivalent).</li>
</ol>

<h3>What gets reconstructed (full parametric)</h3>
<table border="1" cellpadding="4" cellspacing="0">
  <tr><th>Fusion 360 feature</th><th>Tungsten CAD feature</th></tr>
  <tr><td>Extrude (add/cut/intersect)</td><td>PartDesign::Pad / Pocket</td></tr>
  <tr><td>Revolve (add/cut)</td><td>PartDesign::Revolution / Groove</td></tr>
  <tr><td>Loft (add/cut)</td><td>PartDesign::AdditiveLoft / SubtractiveLoft</td></tr>
  <tr><td>Sweep (add/cut)</td><td>PartDesign::AdditivePipe / SubtractivePipe</td></tr>
  <tr><td>Coil (helix)</td><td>PartDesign::AdditiveHelix / SubtractiveHelix</td></tr>
  <tr><td>Hole (with thread)</td><td>PartDesign::Hole</td></tr>
  <tr><td>Fillet</td><td>PartDesign::Fillet</td></tr>
  <tr><td>Chamfer</td><td>PartDesign::Chamfer</td></tr>
  <tr><td>Draft</td><td>PartDesign::Draft</td></tr>
  <tr><td>Shell</td><td>PartDesign::Thickness</td></tr>
  <tr><td>Rectangular Pattern</td><td>PartDesign::LinearPattern</td></tr>
  <tr><td>Circular Pattern</td><td>PartDesign::PolarPattern</td></tr>
  <tr><td>Path Pattern</td><td>PartDesign::PatternOnPath</td></tr>
  <tr><td>Mirror</td><td>PartDesign::Mirrored</td></tr>
  <tr><td>Scale</td><td>PartDesign::Scaled</td></tr>
  <tr><td>Boolean / Combine (Join/Cut/Intersect)</td><td>PartDesign::Boolean</td></tr>
  <tr><td>Box primitive</td><td>PartDesign::AdditiveBox</td></tr>
  <tr><td>Cylinder primitive</td><td>PartDesign::AdditiveCylinder</td></tr>
  <tr><td>Sphere primitive</td><td>PartDesign::AdditiveSphere</td></tr>
  <tr><td>Torus primitive</td><td>PartDesign::AdditiveTorus</td></tr>
  <tr><td>Cone primitive</td><td>PartDesign::AdditiveCone</td></tr>
  <tr><td>Construction plane</td><td>Part::DatumPlane</td></tr>
  <tr><td>Construction axis</td><td>Part::DatumLine</td></tr>
  <tr><td>Construction point</td><td>Part::DatumPoint</td></tr>
  <tr><td>Sketch + constraints</td><td>Sketcher::SketchObject</td></tr>
  <tr><td>User/Model parameters</td><td>Spreadsheet::Sheet</td></tr>
  <tr><td>Occurrences (assemblies)</td><td>PartDesign::Body + transform</td></tr>
</table>

<h3>Recorded but not rebuilt</h3>
<ul>
  <li>ReplaceFace, BoundaryFill, Patch, Stitch, Unstitch, Thicken,
    Form, Rip, SilhouetteSplit, RuleFillet, Pipe-wall, Rib, Web -
    Fusion surface-only operations with no PartDesign equivalent.
  </li>
  <li>Cosmetic Thread features (ThreadFeature on existing cylindrical
    faces) - add a PartDesign Hole with Threaded=True manually.
  </li>
  <li>Components inside assemblies are flattened to a single Body;
    parametric children cannot be reconstructed from F3D alone.
  </li>
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
  <li><b>Pattern direction unresolved:</b> Some Fusion patterns
    reference custom datum planes or faces. The pattern is created
    but with no direction - set it manually in the property panel.</li>
</ul>
"""


def show(parent: QtWidgets.QWidget | None = None) -> None:
    """Open the help dialog modally."""
    dialog = QtWidgets.QDialog(parent)
    dialog.setWindowTitle("Fusion 360 Import - Help")
    dialog.resize(680, 720)

    layout = QtWidgets.QVBoxLayout(dialog)
    text = QtWidgets.QTextBrowser()
    text.setOpenExternalLinks(True)
    text.setHtml(README_TEXT)
    layout.addWidget(text)

    button_box = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok)
    button_box.accepted.connect(dialog.accept)
    layout.addWidget(button_box)

    dialog.exec_()