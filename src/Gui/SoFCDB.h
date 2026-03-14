#ifndef GUI_SOFCDB_H
#define GUI_SOFCDB_H

#include <iosfwd>
#include <map>
#include <string>
#include <Inventor/C/basic.h>

#include <FCGlobal.h>

class SoNode;
class SoGroup;
class SoVRMLGroup;

namespace Gui
{
/**
 * The FreeCAD database class to initialize all our own Inventor nodes.
 * @author Werner Mayer
 */
class GuiExport SoFCDB
{
public:
    static SbBool isInitialized();
    static void init();
    static void finish();
    static SoNode* replaceSwitches(SoNode* node);
    /// helper to apply a SoWriteAction to a node and write it to a string
    static const std::string& writeNodesToString(SoNode* root);
    static bool writeToVRML(SoNode* node, const char* filename, bool binary);
    static void writeToVRML(SoNode* node, std::string& buffer);
    static bool writeToX3D(SoNode* node, const char* filename, bool binary);
    static bool writeToX3D(SoNode* node, bool exportViewpoints, std::string& buffer);
    static bool writeToX3DOM(SoNode* node, std::string& buffer);
    // Write to Inventor, VRML, X3D or XHTML (based on X3DOM) file
    static bool writeToFile(SoNode* node, const char* filename, bool binary);
    /*! container for app lifetime storage. See SoTransformDragger for details
     * on why this is needed.
     */
    static SoGroup* getStorage();

private:
    static void writeX3D(SoVRMLGroup* node, bool exportViewpoints, std::ostream& out);
    static void writeX3DChild(
        SoNode* node,
        std::map<SoNode*, std::string>& nodeMap,
        int& numDEF,
        int spaces,
        std::ostream& out
    );
    static void writeX3DFields(
        SoNode* node,
        std::map<SoNode*, std::string>& nodeMap,
        bool isRoot,
        int& numDEF,
        int spaces,
        std::ostream& out
    );
};

}  // namespace Gui

#endif  // GUI_SOFCDB_H
