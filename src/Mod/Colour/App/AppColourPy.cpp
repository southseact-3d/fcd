// SPDX-License-Identifier: LGPL-2.1-or-later

#include <algorithm>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObjectPy.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <Base/PyWrapParseTupleAndKeywords.h>

#include "ColourMapObject.h"

FC_LOG_LEVEL_INIT("Colour")

namespace Colour
{

class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("Colour")
    {
        add_varargs_method("createColourMap", &Module::createColourMap,
            "createColourMap(source, [name])\n"
            "Create a ColourMap object for the given source shape.\n"
            "source: The source Part shape object to create a colour map for\n"
            "name: Optional name for the new ColourMap object (default: auto-generated)");
        
        add_varargs_method("getColourMap", &Module::getColourMap,
            "getColourMap(object)\n"
            "Get the ColourMap data from a ColourMapObject.\n"
            "Returns the ColourMap instance or None if not found.");
        
        add_keyword_method("setFaceColour", &Module::setFaceColour,
            "setFaceColour(colourMap, faceIndex, r, g, b, [a=255])\n"
            "Set a face colour in the colour map.\n"
            "colourMap: The ColourMapObject to modify\n"
            "faceIndex: The face index to colour\n"
            "r, g, b: Red, green, blue components (0-255)\n"
            "a: Alpha component (0-255, default 255)");
        
        add_varargs_method("exportTextured", &Module::exportTextured,
            "exportTextured(colourMap, filename, [format='3mf'])\n"
            "Export the coloured object to a textured mesh file.\n"
            "colourMap: The ColourMapObject to export\n"
            "filename: Output file path\n"
            "format: Export format ('3mf', 'glb', or 'obj')");
        
        add_varargs_method("setFacePbr", &Module::setFacePbr,
            "setFacePbr(colourMap, faceIndex, metallic, roughness, [alpha=1.0])\n"
            "Set PBR material properties for a face.\n"
            "colourMap: The ColourMapObject to modify\n"
            "faceIndex: The face index\n"
            "metallic: Metallic value (0.0 - 1.0)\n"
            "roughness: Roughness value (0.0 - 1.0)\n"
            "alpha: Alpha/transparency (0.0 - 1.0, default 1.0)");
        
        add_varargs_method("addLayer", &Module::addLayer,
            "addLayer(colourMap, faceIndex, name, [colour=(128,128,255,255)], [zOrder=0])\n"
            "Add a paint layer to a face.\n"
            "colourMap: The ColourMapObject to modify\n"
            "faceIndex: The face index\n"
            "name: Layer name\n"
            "colour: RGBA tuple (default: semi-transparent blue)\n"
            "zOrder: Layer z-order for stacking");
        
        add_varargs_method("getLayers", &Module::getLayers,
            "getLayers(colourMap, faceIndex)\n"
            "Get all layers for a face.\n"
            "colourMap: The ColourMapObject\n"
            "faceIndex: The face index\n"
            "Returns list of layer dictionaries");
        
        add_varargs_method("removeLayer", &Module::removeLayer,
            "removeLayer(colourMap, faceIndex, layerIndex)\n"
            "Remove a layer from a face.\n"
            "colourMap: The ColourMapObject to modify\n"
            "faceIndex: The face index\n"
            "layerIndex: Index of the layer to remove");

        initialize("This module is the Colour paint module.");
    }

private:
    Py::Object createColourMap(const Py::Tuple& args)
    {
        PyObject* sourceObj;
        char* name = nullptr;
        
        if (!PyArg_ParseTuple(args.ptr(), "O|s", &sourceObj, &name)) {
            throw Py::Exception();
        }

        App::Document* doc = App::GetApplication().getActiveDocument();
        if (!doc) {
            throw Py::RuntimeError("No active document");
        }

        // Get the source object
        App::DocumentObject* source = nullptr;
        if (PyObject_TypeCheck(sourceObj, &App::DocumentObjectPy::Type)) {
            source = static_cast<App::DocumentObjectPy*>(sourceObj)->getDocumentObjectPtr();
        } else {
            throw Py::TypeError("First argument must be a DocumentObject");
        }

        if (!source) {
            throw Py::RuntimeError("Invalid source object");
        }

        // Generate unique name if not provided
        std::string objectName;
        if (name && *name) {
            objectName = name;
        } else {
            objectName = doc->getUniqueObjectName("ColourMap");
        }

        // Create the ColourMapObject
        Colour::ColourMapObject* colourMap = new Colour::ColourMapObject();
        colourMap->Source.setValue(source);

        // Initialize with default colour map
        Colour::ColourMap map;
        map.setFaceColour(0, {255, 255, 255, 255});  // Default white face
        colourMap->setColourMap(map);

        // Add to document
        doc->addObject(colourMap, objectName.c_str());
        
        colourMap->recompute();
        
        return Py::asObject(colourMap->getPyObject());
    }

    Py::Object getColourMap(const Py::Tuple& args)
    {
        PyObject* obj;
        
        if (!PyArg_ParseTuple(args.ptr(), "O", &obj)) {
            throw Py::Exception();
        }

        if (!PyObject_TypeCheck(obj, &App::DocumentObjectPy::Type)) {
            throw Py::TypeError("Argument must be a DocumentObject");
        }

        App::DocumentObject* docObj = static_cast<App::DocumentObjectPy*>(obj)->getDocumentObjectPtr();
        
        if (!docObj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            throw Py::TypeError("Object is not a ColourMapObject");
        }

        // Return the ColourMap data as a Python dict
        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(docObj);
        const Colour::ColourMap& map = colourMap->getColourMap();
        
        Py::Dict result;
        
        // Add face colours
        Py::Dict faceColours;
        for (const auto& [faceIndex, colour] : map.faceColours()) {
            Py::Tuple colourTuple(4);
            colourTuple.setItem(0, Py::Long(colour.r));
            colourTuple.setItem(1, Py::Long(colour.g));
            colourTuple.setItem(2, Py::Long(colour.b));
            colourTuple.setItem(3, Py::Long(colour.a));
            faceColours.setItem(Py::Long(faceIndex), colourTuple);
        }
        result.setItem("faceColours", faceColours);
        
        // Add source object
        App::DocumentObject* source = colourMap->Source.getValue();
        if (source) {
            result.setItem("source", Py::asObject(source->getPyObject()));
        }
        
        return result;
    }

    Py::Object setFaceColour(const Py::Tuple& args, const Py::Dict& kwds)
    {
        PyObject* colourMapObj;
        int faceIndex;
        int r, g, b;
        int a = 255;
        
        static const char* keywords[] = {"colourMap", "faceIndex", "r", "g", "b", "a", nullptr};
        
        if (!Base::Wrapped_ParseTupleAndKeywords(args.ptr(), kwds.ptr(), "Oiiii|i", keywords,
            &colourMapObj, &faceIndex, &r, &g, &b, &a)) {
            throw Py::Exception();
        }

        if (!PyObject_TypeCheck(colourMapObj, &App::DocumentObjectPy::Type)) {
            throw Py::TypeError("First argument must be a DocumentObject");
        }

        App::DocumentObject* docObj = static_cast<App::DocumentObjectPy*>(colourMapObj)->getDocumentObjectPtr();
        
        if (!docObj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            throw Py::TypeError("Object is not a ColourMapObject");
        }

        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(docObj);
        Colour::ColourMap map = colourMap->getColourMap();
        
        // Clamp values
        r = std::max(0, std::min(255, r));
        g = std::max(0, std::min(255, g));
        b = std::max(0, std::min(255, b));
        a = std::max(0, std::min(255, a));
        
        map.setFaceColour(faceIndex, {
            static_cast<std::uint8_t>(r),
            static_cast<std::uint8_t>(g),
            static_cast<std::uint8_t>(b),
            static_cast<std::uint8_t>(a)
        });
        
        colourMap->setColourMap(map);
        colourMap->recompute();
        
        return Py::None();
    }

    Py::Object exportTextured(const Py::Tuple& args)
    {
        PyObject* colourMapObj;
        char* filename;
        char* format = nullptr;
        
        if (!PyArg_ParseTuple(args.ptr(), "Os|s", &colourMapObj, &filename, &format)) {
            throw Py::Exception();
        }

        if (!PyObject_TypeCheck(colourMapObj, &App::DocumentObjectPy::Type)) {
            throw Py::TypeError("First argument must be a DocumentObject");
        }

        App::DocumentObject* docObj = static_cast<App::DocumentObjectPy*>(colourMapObj)->getDocumentObjectPtr();
        
        if (!docObj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            throw Py::TypeError("Object is not a ColourMapObject");
        }

        // Determine format from filename or parameter
        std::string fmt = format ? format : "";
        if (fmt.empty()) {
            std::string fname = filename;
            if (fname.size() > 4) {
                std::string ext = fname.substr(fname.size() - 4);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".obj") fmt = "obj";
                else if (ext == ".glb") fmt = "glb";
                else if (ext == ".3mf") fmt = "3mf";
            }
        }

        if (fmt.empty()) {
            throw Py::ValueError("Could not determine export format from filename. Use .obj, .glb, or .3mf extension.");
        }

        // For now, just log the export request
        // Full implementation would call the exporter with proper mesh data
        FC_LOG("Export " << fmt << " to " << filename);
        
        // TODO: Implement full export with tessellation and UV generation
        Base::Console().Message("Export to %s format not yet fully implemented\n", fmt.c_str());
        
        return Py::None();
    }
    
    Py::Object setFacePbr(const Py::Tuple& args, const Py::Dict& kwds)
    {
        PyObject* colourMapObj;
        int faceIndex;
        double metallic;
        double roughness;
        double alpha = 1.0;
        
        static const char* keywords[] = {"colourMap", "faceIndex", "metallic", "roughness", "alpha", nullptr};
        
        if (!Base::Wrapped_ParseTupleAndKeywords(args.ptr(), kwds.ptr(), "Oidd|d", keywords,
            &colourMapObj, &faceIndex, &metallic, &roughness, &alpha)) {
            throw Py::Exception();
        }

        if (!PyObject_TypeCheck(colourMapObj, &App::DocumentObjectPy::Type)) {
            throw Py::TypeError("First argument must be a DocumentObject");
        }

        App::DocumentObject* docObj = static_cast<App::DocumentObjectPy*>(colourMapObj)->getDocumentObjectPtr();
        
        if (!docObj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            throw Py::TypeError("Object is not a ColourMapObject");
        }

        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(docObj);
        Colour::ColourMap map = colourMap->getColourMap();
        
        // Clamp values
        metallic = std::max(0.0, std::min(1.0, metallic));
        roughness = std::max(0.0, std::min(1.0, roughness));
        alpha = std::max(0.0, std::min(1.0, alpha));
        
        Colour::PBR pbr;
        pbr.metallic = static_cast<float>(metallic);
        pbr.roughness = static_cast<float>(roughness);
        pbr.alpha = static_cast<float>(alpha);
        
        map.setFacePbr(faceIndex, pbr);
        colourMap->setColourMap(map);
        colourMap->recompute();
        
        return Py::None();
    }
    
    Py::Object addLayer(const Py::Tuple& args, const Py::Dict& kwds)
    {
        PyObject* colourMapObj;
        int faceIndex;
        char* name;
        int r = 128, g = 128, b = 255, a = 255;
        int zOrder = 0;
        
        static const char* keywords[] = {"colourMap", "faceIndex", "name", "colour", "zOrder", nullptr};
        
        if (!Base::Wrapped_ParseTupleAndKeywords(args.ptr(), kwds.ptr(), "Oi s|(iiii)i", keywords,
            &colourMapObj, &faceIndex, &name, &r, &g, &b, &a, &zOrder)) {
            throw Py::Exception();
        }

        if (!PyObject_TypeCheck(colourMapObj, &App::DocumentObjectPy::Type)) {
            throw Py::TypeError("First argument must be a DocumentObject");
        }

        App::DocumentObject* docObj = static_cast<App::DocumentObjectPy*>(colourMapObj)->getDocumentObjectPtr();
        
        if (!docObj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            throw Py::TypeError("Object is not a ColourMapObject");
        }

        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(docObj);
        Colour::ColourMap map = colourMap->getColourMap();
        
        // Clamp colour values
        r = std::max(0, std::min(255, r));
        g = std::max(0, std::min(255, g));
        b = std::max(0, std::min(255, b));
        a = std::max(0, std::min(255, a));
        
        Colour::Layer layer;
        layer.name = name;
        layer.zOrder = zOrder;
        layer.visible = true;
        layer.locked = false;
        layer.isImageLayer = false;
        layer.colour = {
            static_cast<std::uint8_t>(r),
            static_cast<std::uint8_t>(g),
            static_cast<std::uint8_t>(b),
            static_cast<std::uint8_t>(a)
        };
        layer.pbr = {0.0F, 0.5F, 1.0F};
        
        map.addLayer(faceIndex, layer);
        colourMap->setColourMap(map);
        colourMap->recompute();
        
        return Py::Long(map.getLayers(faceIndex).size() - 1);
    }
    
    Py::Object getLayers(const Py::Tuple& args)
    {
        PyObject* colourMapObj;
        int faceIndex;
        
        if (!PyArg_ParseTuple(args.ptr(), "Oi", &colourMapObj, &faceIndex)) {
            throw Py::Exception();
        }

        if (!PyObject_TypeCheck(colourMapObj, &App::DocumentObjectPy::Type)) {
            throw Py::TypeError("First argument must be a DocumentObject");
        }

        App::DocumentObject* docObj = static_cast<App::DocumentObjectPy*>(colourMapObj)->getDocumentObjectPtr();
        
        if (!docObj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            throw Py::TypeError("Object is not a ColourMapObject");
        }

        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(docObj);
        const Colour::ColourMap& map = colourMap->getColourMap();
        const std::vector<Colour::Layer>& layers = map.getLayers(faceIndex);
        
        Py::List result;
        for (const auto& layer : layers) {
            Py::Dict layerDict;
            layerDict.setItem("name", Py::String(layer.name));
            layerDict.setItem("zOrder", Py::Long(layer.zOrder));
            layerDict.setItem("visible", Py::Boolean(layer.visible));
            layerDict.setItem("locked", Py::Boolean(layer.locked));
            layerDict.setItem("isImageLayer", Py::Boolean(layer.isImageLayer));
            
            Py::Tuple colourTuple(4);
            colourTuple.setItem(0, Py::Long(layer.colour.r));
            colourTuple.setItem(1, Py::Long(layer.colour.g));
            colourTuple.setItem(2, Py::Long(layer.colour.b));
            colourTuple.setItem(3, Py::Long(layer.colour.a));
            layerDict.setItem("colour", colourTuple);
            
            Py::Dict pbrDict;
            pbrDict.setItem("metallic", Py::Float(layer.pbr.metallic));
            pbrDict.setItem("roughness", Py::Float(layer.pbr.roughness));
            pbrDict.setItem("alpha", Py::Float(layer.pbr.alpha));
            layerDict.setItem("pbr", pbrDict);
            
            result.append(layerDict);
        }
        
        return result;
    }
    
    Py::Object removeLayer(const Py::Tuple& args)
    {
        PyObject* colourMapObj;
        int faceIndex;
        int layerIndex;
        
        if (!PyArg_ParseTuple(args.ptr(), "Oii", &colourMapObj, &faceIndex, &layerIndex)) {
            throw Py::Exception();
        }

        if (!PyObject_TypeCheck(colourMapObj, &App::DocumentObjectPy::Type)) {
            throw Py::TypeError("First argument must be a DocumentObject");
        }

        App::DocumentObject* docObj = static_cast<App::DocumentObjectPy*>(colourMapObj)->getDocumentObjectPtr();
        
        if (!docObj->isDerivedFrom(Colour::ColourMapObject::getClassTypeId())) {
            throw Py::TypeError("Object is not a ColourMapObject");
        }

        Colour::ColourMapObject* colourMap = static_cast<Colour::ColourMapObject*>(docObj);
        Colour::ColourMap map = colourMap->getColourMap();
        
        if (!map.removeLayer(faceIndex, static_cast<std::size_t>(layerIndex))) {
            throw Py::IndexError("Layer index out of range");
        }
        
        colourMap->setColourMap(map);
        colourMap->recompute();
        
        return Py::None();
    }
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

} // namespace Colour
