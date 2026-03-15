// SPDX-License-Identifier: LGPL-2.1-or-later

#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection.h>

#include <cstdint>

#include <Mod/Colour/App/ColourMap.h>
#include <Mod/Colour/App/ColourMapObject.h>
#include <Mod/Colour/App/Exporter.h>
#include <Mod/Colour/App/TextureCompositor.h>
#include <Mod/Colour/App/UVAtlas.h>
#include <Mod/Colour/App/BRepTessellator.h>
#include <Mod/Part/App/PartFeature.h>
#include <App/PropertyTopoShape.h>

#include <TopoDS_Shape.hxx>

#include "PanelManager.h"
#include "ColourPickerDialog.h"
#include "ExportDialog.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QString>
#include <QRegExp>

DEF_STD_CMD_A(CmdColourFaceFill)
 
CmdColourFaceFill::CmdColourFaceFill()
    : Command("Colour_FaceFill")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Face Fill");
    sToolTipText = QT_TR_NOOP("Assign a base colour to selected faces.");
    sStatusTip = sToolTipText;
}

int getSelectedFaceIndex()
{
    const QString& selStr = Gui::Selection().getSelection();
    if (selStr.contains(QStringLiteral("Face"))) {
        QRegExp rx(QStringLiteral("Face(\\d+)"));
        if (rx.indexIn(selStr) != -1) {
            return rx.cap(1).toInt() - 1;
        }
    }
    return 0;
}

namespace {

bool extractMeshFromObject(App::DocumentObject* obj, std::vector<Colour::Vertex>& vertices, std::vector<Colour::Triangle>& triangles)
{
    if (!obj) {
        return false;
    }

    const TopoDS_Shape* shapePtr = nullptr;
    
    if (obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
        const Part::Feature* partFeature = static_cast<const Part::Feature*>(obj);
        shapePtr = &partFeature->Shape.getValue();
    }
    else {
        App::Property* shapeProp = obj->getPropertyByName("Shape");
        if (!shapeProp) {
            return false;
        }
        if (shapeProp->getTypeId() == App::PropertyTopoShape::getClassTypeId()) {
            const App::PropertyTopoShape& propShape = static_cast<const App::PropertyTopoShape&>(*shapeProp);
            shapePtr = &propShape.getValue();
        }
    }

    if (!shapePtr || shapePtr->IsNull()) {
        return false;
    }

    const TopoDS_Shape& shape = *shapePtr;

    Colour::BRepTessellator tessellator;
    tessellator.setQuality(0.5);

    BRepTessellator::TessellationOptions opts;
    opts.linearDeflection = 0.01;
    opts.angularDeflection = 0.5;
    opts.relativeDeflection = true;

    return tessellator.tessellate(shape, vertices, triangles, opts);
}

} // anonymous namespace

void CmdColourFaceFill::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    int faceIndex = getSelectedFaceIndex();

    ColourPickerDialog dlg(QColor(255, 255, 255), Gui::getMainWindow());
    dlg.setWindowTitle(tr("Face Fill Colour"));

    if (!selection.empty()) {
        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
        Colour::ColourMap map = colourMap->getColourMap();

        Colour::RGBA faceColour = map.getFaceColour(faceIndex);
        dlg.setSelectedColor(QColor(faceColour.r, faceColour.g, faceColour.b, faceColour.a));
    }

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    QColor colour = dlg.selectedColor();

    if (selection.empty()) {
        QMessageBox::information(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Selected face fill colour: R=%1 G=%2 B=%3.\nSelect a ColourMap object to apply.")
                .arg(colour.red())
                .arg(colour.green())
                .arg(colour.blue())
        );
        return;
    }

    openCommand("Face Fill");

    for (App::DocumentObject* obj : selection) {
        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(obj);
        Colour::ColourMap map = colourMap->getColourMap();
        
        map.setFaceColour(faceIndex, {
            static_cast<std::uint8_t>(colour.red()),
            static_cast<std::uint8_t>(colour.green()),
            static_cast<std::uint8_t>(colour.blue()),
            static_cast<std::uint8_t>(colour.alpha())
        });
        
        colourMap->setColourMap(map);
    }

    commitCommand();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Applied face fill colour to face %1: R=%2 G=%3 B=%4.")
            .arg(faceIndex)
            .arg(colour.red())
            .arg(colour.green())
            .arg(colour.blue())
    );
}

bool CmdColourFaceFill::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourExport)

CmdColourExport::CmdColourExport()
    : Command("Colour_ExportTextured")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Export Textured");
    sToolTipText = QT_TR_NOOP("Export textured 3MF/GLB/OBJ outputs.");
    sStatusTip = sToolTipText;
}

void CmdColourExport::activated(int)
{
    ExportDialog dlg(Gui::getMainWindow());
    dlg.setWindowTitle(tr("Export Textured Model"));
    
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    ExportDialog::ExportFormat format = dlg.getFormat();
    ExportDialog::Preset preset = dlg.getPreset();
    
    QString filter;
    if (format == ExportDialog::Format3MF) {
        filter = QStringLiteral("3MF (*.3mf)");
    }
    else if (format == ExportDialog::FormatGLB) {
        filter = QStringLiteral("GLB (*.glb)");
    }
    else {
        filter = QStringLiteral("OBJ (*.obj)");
    }

    const QString selected = QFileDialog::getSaveFileName(
        Gui::getMainWindow(),
        QObject::tr("Export Textured Model"),
        QString(),
        filter
    );
    if (selected.isEmpty()) {
        return;
    }

    const bool isObj = (format == ExportDialog::FormatOBJ);
    const bool isGlb = (format == ExportDialog::FormatGLB);
    const bool is3mf = (format == ExportDialog::Format3MF);

    std::vector<App::DocumentObject*> sel = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (sel.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object to export.")
        );
        return;
    }

    auto* colourMapObj = static_cast<Colour::ColourMapObject*>(sel.front());
    App::DocumentObject* source = colourMapObj->Source.getValue();

    if (!source) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("The ColourMap object has no source shape linked. Please set a source object.")
        );
        return;
    }

    std::vector<Colour::Vertex> vertices;
    std::vector<Colour::Triangle> triangles;

    if (!extractMeshFromObject(source, vertices, triangles)) {
        QMessageBox::critical(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Failed to extract mesh from source object. Make sure the source has a valid shape.")
        );
        return;
    }

    if (vertices.empty() || triangles.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("The source shape produced an empty mesh.")
        );
        return;
    }

    int resolution = dlg.getAtlasResolution();
    Colour::UVAtlas atlas;
    if (!atlas.generateFacePerIsland(vertices, triangles, resolution, 2)) {
        QMessageBox::critical(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Failed to generate UV atlas for export.")
        );
        return;
    }

    const Colour::ColourMap& colourMap = colourMapObj->getColourMap();

    Colour::TextureCompositor compositor;
    compositor.setPerformanceStatsEnabled(true);
    Colour::AtlasImage base = compositor.compose(colourMap, atlas);
    Colour::AtlasImage mr = compositor.composeMetallicRoughness(colourMap, atlas);

    Colour::Exporter exporter;
    Colour::Exporter::ExportOptions options;
    options.jlcWjpPreset = dlg.isJlcWjpPreset();
    options.forceOpaqueAlpha = dlg.isForceOpaqueAlpha();
    options.srgbTextures = dlg.isSrgbTextures();
    options.validateWallThickness = dlg.isValidateWallThickness();

    if (options.validateWallThickness && options.jlcWjpPreset) {
        Colour::Exporter::ValidationResult validation = exporter.validateForJlc3dp(vertices, triangles, atlas);
        if (!validation.warnings.empty()) {
            QString warningMsg = QString::fromStdString("JLC3DP Validation Warnings:\n");
            for (const auto& warn : validation.warnings) {
                warningMsg += QString::fromStdString(warn) + "\n";
            }
            QMessageBox::warning(
                Gui::getMainWindow(),
                QObject::tr("JLC3DP Validation"),
                warningMsg
            );
        }
    }

    bool ok = false;
    const std::string out = selected.toStdString();
    if (isObj) {
        const std::string stem = out.substr(0, out.size() - 4U);
        ok = exporter.exportObjMtl(stem, vertices, triangles, atlas, base);
    }
    else if (isGlb) {
        ok = exporter.exportGlb(out, vertices, triangles, atlas, base, mr, options);
    }
    else {
        ok = exporter.export3mf(out, vertices, triangles, atlas, base, options);
    }

    if (!ok) {
        QMessageBox::critical(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Export failed. See console/log for details.")
        );
        return;
    }

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Export completed: %1").arg(selected)
    );
}

bool CmdColourExport::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourLayerAdd)

CmdColourLayerAdd::CmdColourLayerAdd()
    : Command("Colour_LayerAdd")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Add Layer");
    sToolTipText = QT_TR_NOOP("Add a new paint layer to the selected face.");
    sStatusTip = sToolTipText;
}

void CmdColourLayerAdd::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object to add a layer.")
        );
        return;
    }

    Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
    Colour::ColourMap map = colourMap->getColourMap();

    bool ok = false;
    QString name = QInputDialog::getText(
        Gui::getMainWindow(),
        QObject::tr("Add Layer"),
        QObject::tr("Layer name:"),
        QLineEdit::Normal,
        QObject::tr("New Layer"),
        &ok
    );

    if (!ok || name.isEmpty()) {
        return;
    }

    Colour::Layer layer;
    layer.name = name.toStdString();
    layer.zOrder = 0;
    layer.visible = true;
    layer.locked = false;
    layer.isImageLayer = false;
    layer.colour = {128, 128, 128, 255};
    layer.pbr = {0.0F, 0.5F, 1.0F};

    map.addLayer(faceIndex, layer);
    colourMap->setColourMap(map);
    colourMap->recompute();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Layer '%1' added.").arg(name)
    );
}

bool CmdColourLayerAdd::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourLayerDelete)

CmdColourLayerDelete::CmdColourLayerDelete()
    : Command("Colour_LayerDelete")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Delete Layer");
    sToolTipText = QT_TR_NOOP("Delete the selected paint layer.");
    sStatusTip = sToolTipText;
}

void CmdColourLayerDelete::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object to delete a layer.")
        );
        return;
    }

    Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
    const Colour::ColourMap& map = colourMap->getColourMap();
    
    int faceIndex = getSelectedFaceIndex();
    const auto& layers = map.getLayers(faceIndex);

    if (layers.empty()) {
        QMessageBox::information(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("No layers to delete.")
        );
        return;
    }

    bool ok = false;
    int layerIndex = QInputDialog::getInt(
        Gui::getMainWindow(),
        QObject::tr("Delete Layer"),
        QObject::tr("Layer index (0-%1):").arg(layers.size() - 1),
        0, 0, static_cast<int>(layers.size()) - 1,
        1, &ok
    );

    if (!ok) {
        return;
    }

    Colour::ColourMap mutableMap = map;
    mutableMap.removeLayer(faceIndex, static_cast<std::size_t>(layerIndex));
    colourMap->setColourMap(mutableMap);
    colourMap->recompute();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Layer deleted.")
    );
}

bool CmdColourLayerDelete::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourLayerToggle)

CmdColourLayerToggle::CmdColourLayerToggle()
    : Command("Colour_LayerToggle")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Toggle Layer Visibility");
    sToolTipText = QT_TR_NOOP("Toggle visibility of the selected paint layer.");
    sStatusTip = sToolTipText;
}

void CmdColourLayerToggle::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object.")
        );
        return;
    }

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Layer visibility toggle: Use the Layer Panel UI for full layer management.")
    );
}

bool CmdColourLayerToggle::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourLayerPanel)

CmdColourLayerPanel::CmdColourLayerPanel()
    : Command("Colour_LayerPanel")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Layer Panel");
    sToolTipText = QT_TR_NOOP("Show/hide the Layer Panel for managing paint layers.");
    sStatusTip = sToolTipText;
}

void CmdColourLayerPanel::activated(int)
{
    ColourGui::PanelManager::instance().toggleLayerPanel();
}

bool CmdColourLayerPanel::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourMaterialPanel)

CmdColourMaterialPanel::CmdColourMaterialPanel()
    : Command("Colour_MaterialPanel")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Material Panel");
    sToolTipText = QT_TR_NOOP("Show/hide the Material Panel for PBR properties.");
    sStatusTip = sToolTipText;
}

void CmdColourMaterialPanel::activated(int)
{
    ColourGui::PanelManager::instance().toggleMaterialPanel();
}

bool CmdColourMaterialPanel::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourSetMetallic)

CmdColourSetMetallic::CmdColourSetMetallic()
    : Command("Colour_SetMetallic")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Set Metallic");
    sToolTipText = QT_TR_NOOP("Set metallic value for selected face.");
    sStatusTip = sToolTipText;
}

void CmdColourSetMetallic::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object.")
        );
        return;
    }

    bool ok = false;
    double metallic = QInputDialog::getDouble(
        Gui::getMainWindow(),
        QObject::tr("Set Metallic"),
        QObject::tr("Metallic (0.0 - 1.0):"),
        0.5, 0.0, 1.0, 2, &ok
    );

    if (!ok) {
        return;
    }

    Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
    Colour::ColourMap map = colourMap->getColourMap();

    int faceIndex = getSelectedFaceIndex();

    Colour::PBR pbr;
    pbr.metallic = static_cast<float>(metallic);
    pbr.roughness = 0.5F;
    pbr.alpha = 1.0F;

    map.setFacePbr(faceIndex, pbr);
    colourMap->setColourMap(map);
    colourMap->recompute();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Metallic set to %1").arg(metallic)
    );
}

bool CmdColourSetMetallic::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourSetRoughness)

CmdColourSetRoughness::CmdColourSetRoughness()
    : Command("Colour_SetRoughness")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Set Roughness");
    sToolTipText = QT_TR_NOOP("Set roughness value for selected face.");
    sStatusTip = sToolTipText;
}

void CmdColourSetRoughness::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object.")
        );
        return;
    }

    bool ok = false;
    double roughness = QInputDialog::getDouble(
        Gui::getMainWindow(),
        QObject::tr("Set Roughness"),
        QObject::tr("Roughness (0.0 - 1.0):"),
        0.5, 0.0, 1.0, 2, &ok
    );

    if (!ok) {
        return;
    }

    Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
    Colour::ColourMap map = colourMap->getColourMap();

    int faceIndex = getSelectedFaceIndex();

    Colour::PBR pbr;
    pbr.metallic = 0.0F;
    pbr.roughness = static_cast<float>(roughness);
    pbr.alpha = 1.0F;

    map.setFacePbr(faceIndex, pbr);
    colourMap->setColourMap(map);
    colourMap->recompute();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Roughness set to %1").arg(roughness)
    );
}

bool CmdColourSetRoughness::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourSetAlpha)

CmdColourSetAlpha::CmdColourSetAlpha()
    : Command("Colour_SetAlpha")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Set Alpha");
    sToolTipText = QT_TR_NOOP("Set alpha/transparency value for selected face.");
    sStatusTip = sToolTipText;
}

void CmdColourSetAlpha::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object.")
        );
        return;
    }

    bool ok = false;
    double alpha = QInputDialog::getDouble(
        Gui::getMainWindow(),
        QObject::tr("Set Alpha"),
        QObject::tr("Alpha/Transparency (0.0 - 1.0):"),
        1.0, 0.0, 1.0, 2, &ok
    );

    if (!ok) {
        return;
    }

    Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
    Colour::ColourMap map = colourMap->getColourMap();

    int faceIndex = getSelectedFaceIndex();

    Colour::PBR pbr;
    pbr.metallic = 0.0F;
    pbr.roughness = 0.5F;
    pbr.alpha = static_cast<float>(alpha);

    map.setFacePbr(faceIndex, pbr);
    colourMap->setColourMap(map);
    colourMap->recompute();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Alpha set to %1").arg(alpha)
    );
}

bool CmdColourSetAlpha::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourImportTexture)

CmdColourImportTexture::CmdColourImportTexture()
    : Command("Colour_ImportTexture")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Import Image Texture");
    sToolTipText = QT_TR_NOOP("Import an image to use as a texture on selected faces.");
    sStatusTip = sToolTipText;
}

void CmdColourImportTexture::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    const QString selected = QFileDialog::getOpenFileName(
        Gui::getMainWindow(),
        QObject::tr("Select Texture Image"),
        QString(),
        QObject::tr("Images (*.png *.jpg *.jpeg *.bmp)")
    );

    if (selected.isEmpty()) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object to apply the texture.")
        );
        return;
    }

    QImage qimg(selected);
    if (qimg.isNull()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Failed to load image: %1").arg(selected)
        );
        return;
    }

    QImage rgbaImg = qimg.convertToFormat(QImage::Format_RGBA8888);
    if (rgbaImg.isNull()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Failed to convert image to RGBA format.")
        );
        return;
    }

    Colour::ImageData imgData;
    imgData.width = rgbaImg.width();
    imgData.height = rgbaImg.height();
    imgData.rgba.resize(static_cast<std::size_t>(imgData.width) * static_cast<std::size_t>(imgData.height) * 4);

    for (int y = 0; y < imgData.height; ++y) {
        for (int x = 0; x < imgData.width; ++x) {
            QRgb pixel = rgbaImg.pixel(x, y);
            const std::size_t i = static_cast<std::size_t>(y * imgData.width + x) * 4;
            imgData.rgba[i + 0] = static_cast<std::uint8_t>(qRed(pixel));
            imgData.rgba[i + 1] = static_cast<std::uint8_t>(qGreen(pixel));
            imgData.rgba[i + 2] = static_cast<std::uint8_t>(qBlue(pixel));
            imgData.rgba[i + 3] = static_cast<std::uint8_t>(qAlpha(pixel));
        }
    }

    Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
    Colour::ColourMap map = colourMap->getColourMap();

    int faceIndex = getSelectedFaceIndex();

    QFileInfo fileInfo(selected);
    std::string imageId = fileInfo.fileName().toStdString();

    map.registerImage(imageId, imgData);

    Colour::Layer layer;
    layer.name = fileInfo.fileName().toStdString();
    layer.zOrder = 1;
    layer.visible = true;
    layer.locked = false;
    layer.isImageLayer = true;
    layer.imageRef = imageId;
    layer.colour = {255, 255, 255, 255};

    map.addLayer(faceIndex, layer);
    colourMap->setColourMap(map);
    colourMap->recompute();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Texture '%1' imported (%2x%3).")
            .arg(fileInfo.fileName())
            .arg(imgData.width)
            .arg(imgData.height)
    );
}

bool CmdColourImportTexture::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourPaintMode)

CmdColourPaintMode::CmdColourPaintMode()
    : Command("Colour_PaintMode")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Paint Mode");
    sToolTipText = QT_TR_NOOP("Enter interactive paint mode - click faces to paint them.");
    sStatusTip = sToolTipText;
}

void CmdColourPaintMode::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::information(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Select a ColourMap object to enter paint mode.")
        );
        return;
    }

    ColourPickerDialog dlg(QColor(255, 255, 255), Gui::getMainWindow());
    dlg.setWindowTitle(tr("Select Paint Colour"));

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    QColor colour = dlg.selectedColor();

    Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(selection.front());
    Colour::ColourMap map = colourMap->getColourMap();

    const QString& selStr = Gui::Selection().getSelection();
    int faceIndex = 0;
    if (selStr.contains(QStringLiteral("Face"))) {
        QRegExp rx(QStringLiteral("Face(\\d+)"));
        if (rx.indexIn(selStr) != -1) {
            faceIndex = rx.cap(1).toInt() - 1;
        }
    }

    openCommand("Paint Face");

    map.setFaceColour(faceIndex, {
        static_cast<std::uint8_t>(colour.red()),
        static_cast<std::uint8_t>(colour.green()),
        static_cast<std::uint8_t>(colour.blue()),
        static_cast<std::uint8_t>(colour.alpha())
    });

    colourMap->setColourMap(map);

    commitCommand();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Painted face %1 with colour R=%2 G=%3 B=%4.\n\nUse Face Fill command for more options.")
            .arg(faceIndex)
            .arg(colour.red())
            .arg(colour.green())
            .arg(colour.blue())
    );
}

bool CmdColourPaintMode::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

void CmdColourCreateColourMap::activated(int)
{
    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        App::DocumentObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select an object to create a colour map for.")
        );
        return;
    }

    App::DocumentObject* source = selection.front();
    App::Document* doc = App::GetApplication().getActiveDocument();

    if (!doc) {
        return;
    }

    openCommand("Create Colour Map");

    std::string name = doc->getUniqueObjectName("ColourMap");
    Colour::ColourMapObject* colourMap = new Colour::ColourMapObject();
    colourMap->Source.setValue(source);

    // Add initial face colour
    Colour::ColourMap map;
    map.setFaceColour(0, {255, 255, 255, 255});
    colourMap->setColourMap(map);

    doc->addObject(colourMap, name.c_str());

    commitCommand();

    updateActive();
}

bool CmdColourCreateColourMap::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourSketchOnFace)

CmdColourSketchOnFace::CmdColourSketchOnFace()
    : Command("Colour_SketchOnFace")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Sketch on Face");
    sToolTipText = QT_TR_NOOP("Create a sketch layer on selected face for painting.");
    sStatusTip = sToolTipText;
}

void CmdColourSketchOnFace::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object to create a sketch layer.")
        );
        return;
    }

    int faceIndex = getSelectedFaceIndex();

    ColourPickerDialog dlg(QColor(255, 0, 0), Gui::getMainWindow());
    dlg.setWindowTitle(tr("Sketch Colour"));

    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    QColor colour = dlg.selectedColor();

    Colour::ColourMapObject* colourMapObj = static_cast<Colour::ColourMapObject*>(selection.front());
    Colour::ColourMap map = colourMapObj->getColourMap();

    std::string sketchId = doc->getUniqueObjectName("Sketch");
    
    Colour::SketchData sketchData;
    sketchData.sketchId = sketchId;
    sketchData.faceIndex = faceIndex;
    sketchData.referencedObject = colourMapObj->getNameInDocument();

    Colour::SketchGeometry lineGeom;
    lineGeom.type = Colour::SketchElementType::Line;
    lineGeom.isConstruction = false;
    lineGeom.points.push_back({0.1f, 0.1f});
    lineGeom.points.push_back({0.9f, 0.9f});
    sketchData.geometries.push_back(lineGeom);

    map.registerSketch(sketchId, sketchData);

    Colour::Layer layer;
    layer.name = sketchId;
    layer.zOrder = 2;
    layer.visible = true;
    layer.locked = false;
    layer.isSketchLayer = true;
    layer.sketchRef = sketchId;
    layer.colour = {
        static_cast<std::uint8_t>(colour.red()),
        static_cast<std::uint8_t>(colour.green()),
        static_cast<std::uint8_t>(colour.blue()),
        static_cast<std::uint8_t>(colour.alpha())
    };
    layer.pbr = {0.0F, 0.5F, 1.0F};

    map.addLayer(faceIndex, layer);
    colourMapObj->setColourMap(map);
    colourMapObj->recompute();

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Sketch layer created on face %1 with colour R=%2 G=%3 B=%4.\n\n"
            "Workflow:\n"
            "1. Switch to SketcherWorkbench to edit the sketch\n"
            "2. Draw your design and press ESC to finish\n"
            "3. Return to Colour workbench and use 'Refresh Sketch' to capture geometry\n\n"
            "Note: This creates a placeholder sketch. Full automatic integration with Sketcher\n"
            "requires additional FreeCAD API work.")
            .arg(faceIndex)
            .arg(colour.red())
            .arg(colour.green())
            .arg(colour.blue())
    );
}

bool CmdColourSketchOnFace::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

DEF_STD_CMD_A(CmdColourRefreshSketch)

CmdColourRefreshSketch::CmdColourRefreshSketch()
    : Command("Colour_RefreshSketch")
{
    sAppModule = "Colour";
    sGroup = QT_TR_NOOP("Colour");
    sMenuText = QT_TR_NOOP("Refresh Sketch");
    sToolTipText = QT_TR_NOOP("Refresh sketch geometry from the selected sketch object.");
    sStatusTip = sToolTipText;
}

void CmdColourRefreshSketch::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    std::vector<App::DocumentObject*> selection = getSelection().getObjectsOfType(
        Colour::ColourMapObject::getClassTypeId()
    );

    if (selection.empty()) {
        QMessageBox::warning(
            Gui::getMainWindow(),
            QObject::tr("Colour Paint"),
            QObject::tr("Please select a ColourMap object to refresh sketch from.")
        );
        return;
    }

    QMessageBox::information(
        Gui::getMainWindow(),
        QObject::tr("Colour Paint"),
        QObject::tr("Refresh Sketch workflow:\n\n"
            "1. Select the sketch object in the tree (not the ColourMap)\n"
            "2. Switch to SketcherWorkbench to edit\n"
            "3. Draw your design and press ESC to finish\n"
            "4. Return to Colour workbench and use this command again\n\n"
            "Note: Full automatic integration is planned for future releases.")
    );
}

bool CmdColourRefreshSketch::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr;
}

void CreateColourCommands()
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdColourCreateColourMap());
    rcCmdMgr.addCommand(new CmdColourFaceFill());
    rcCmdMgr.addCommand(new CmdColourPaintMode());
    rcCmdMgr.addCommand(new CmdColourSketchOnFace());
    rcCmdMgr.addCommand(new CmdColourRefreshSketch());
    rcCmdMgr.addCommand(new CmdColourExport());
    rcCmdMgr.addCommand(new CmdColourLayerAdd());
    rcCmdMgr.addCommand(new CmdColourLayerDelete());
    rcCmdMgr.addCommand(new CmdColourLayerToggle());
    rcCmdMgr.addCommand(new CmdColourLayerPanel());
    rcCmdMgr.addCommand(new CmdColourMaterialPanel());
    rcCmdMgr.addCommand(new CmdColourSetMetallic());
    rcCmdMgr.addCommand(new CmdColourSetRoughness());
    rcCmdMgr.addCommand(new CmdColourSetAlpha());
    rcCmdMgr.addCommand(new CmdColourImportTexture());
}
