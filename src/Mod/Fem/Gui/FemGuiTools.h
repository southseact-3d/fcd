#ifndef FEM_GUI_TOOLS_H
#define FEM_GUI_TOOLS_H

#include <Inventor/SbVec3f.h>
#include <Inventor/SbRotation.h>


class SoNode;
class SoSeparator;

namespace FemGui
{

namespace GuiTools
{

void createPlacement(SoSeparator* sep, const SbVec3f& base, const SbRotation& r);
void updatePlacement(const SoSeparator* sep, const int idx, const SbVec3f& base, const SbRotation& r);
void createCone(SoSeparator* sep, const double height, const double radius);
SoSeparator* createCone(const double height, const double radius);
void updateCone(const SoNode* node, const int idx, const double height, const double radius);
void createCylinder(SoSeparator* sep, const double height, const double radius);
SoSeparator* createCylinder(const double height, const double radius);
void updateCylinder(const SoNode* node, const int idx, const double height, const double radius);
void createCube(SoSeparator* sep, const double width, const double length, const double height);
SoSeparator* createCube(const double width, const double length, const double height);
void updateCube(
    const SoNode* node,
    const int idx,
    const double width,
    const double length,
    const double height
);
void createArrow(SoSeparator* sep, const double length, const double radius);
SoSeparator* createArrow(const double length, const double radius);
void updateArrow(const SoNode* node, const int idx, const double length, const double radius);
void createFixed(SoSeparator* sep, const double height, const double width, const bool gap = false);
SoSeparator* createFixed(const double height, const double width, const bool gap = false);
void updateFixed(
    const SoNode* node,
    const int idx,
    const double height,
    const double width,
    const bool gap = false
);

}  // namespace GuiTools

}  // namespace FemGui


#endif  // FEM_GUI_TOOLS_H
